//
// Created by pvienne on 11/12/15.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>
#include "lib/shmem.h"
#include "headers/lib.h"
#include "headers/feux.h"
#include "lib/semaphore.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int coordinator_pid;
int coordinator_msg_box;
/**
 * Différent de 0 si on est dans un état d'urgence
 */
int urgence;
int shmem_feux;
int* feux;
int sem_feux;

void quit(int sig){
    printf("\n======\n");
    printf("On prend la main sur les feux ... ");
    down(sem_feux);
    printf("OK\n");
    printf("On dit au coordinateur de s'arrêter ... ");
    kill(coordinator_pid, SIGQUIT);
    printf("OK\n");
    printf("On éteint les feux ... ");
    detach_shmem(feux);
    remove_shmem(shmem_feux);
    remove_semaphore(sem_feux);
    msg_close(coordinator_msg_box);
    printf("OK\n");
    printf("\nGracefully exit ;-)\n");
    exit(0);
}

int lire_feux(){
    int r_buffer = 0;
    down(sem_feux);
    r_buffer = (*feux);
    up(sem_feux);
    return r_buffer;
}

void ecrire_etat_feux(int north, int east, int south, int west){
    down(sem_feux);
    // Il faudrait checker le sémaphore avant !
    (*feux) = (north)+(east<<1)+(south<<2)+(west<<3);
    up(sem_feux);
}

void afficher_etat(){
    int state = lire_feux();
    clear_console();

    printf("== Gestion des feux (%d) == \n\n", getpid());

    printf("    ║ N ║\n");
    printf("    ║   ║\n");
    printf("════╝");
    if(state&0b1){
        printf(" ▼ ");
    } else {
        printf("---");
    }
    printf("╚════\n");
    printf(" W  ");
    if(state&0b1000){
        printf("▶");
    } else {
        printf("┊");
    }
    printf("   ");
    if(state&0b10){
        printf("◀");
    } else {
        printf("┊");
    }
    printf("  E\n");
    printf("════╗");
    if(state&0b100){
        printf(" ▲ ");
    } else {
        printf("---");
    }
    printf("╔════\n");
    printf("    ║   ║\n");
    printf("    ║ S ║  ");
    if(urgence){
        printf("URGENCE!\n");
    } else {
        printf("standard\n");
    }
}

void trap_urgence(int sig){
    switch(sig){
        case SIGUSR1:
            urgence = 1;
            afficher_etat();
            while(urgence){

                // Réception d'une demande de changement de feux unique
                demande_feux demande = {99l, -1};
                msgrcv(coordinator_msg_box, (void*) &demande, sizeof(int), demande.type, 0);
                if(demande.type != -1){
                    down(sem_feux);
                    (*feux) = 1<<(demande.feux);
                    up(sem_feux);
                    demande.type -= 1;
                    msgsnd(coordinator_msg_box, &demande, sizeof(int), 0);

                    // Affichage de l'état
                    afficher_etat();
                }

                // Vérification des demandes toute les 500 micro-secondes
                usleep(500);
            }
            break;
        case SIGUSR2:
            urgence = 0;
            clear_console();
            ecrire_etat_feux(1,0,1,0);
            afficher_etat();
            break;
        default:
            logger("feux","Signal étrange reçu : %d", sig);
    }
}

int main(){

    // On trap les signaux d'interruption pour la gestion de la mémoire partagé.
    signal(SIGQUIT, quit);
    signal(SIGINT, quit);

    printf("== Gestion des feux (%d) == \n\n", getpid());

    // Gestion de la création de la mémoire partagé
    shmem_feux = create_shmem(key_feux, sizeof(int));
    if(shmem_feux<=0){
        printf("La mémoire n'a pas été libéré correctement!\n");
        shmem_feux = open_shmem(key_feux, sizeof(int));
        if(shmem_feux<=0){
            fprintf(stderr, "Impossible de créer les feux !\nOn stope le massacre !");
            exit(2);
        }
    }
    {   // Et le sémaphore ?
        sem_feux = create_semaphore(key_feux);
        if(sem_feux<0){
            printf("Le sémaphore n'a pas été libéré correctement!\n");
            sem_feux = open_semaphore(key_feux);
            if(sem_feux<0){
                fprintf(stderr, "Impossible de créer les feux !\nOn stope le massacre !");
                exit(3);
            }
        }
        init_semaphore(sem_feux, 1);
    }
    down(sem_feux);
    feux = attach_shmem(shmem_feux);
    up(sem_feux);
    printf("Mémoire attachée, on cherche le coordinateur maintenant ...\n");

    // Connexion au coordinateur
    coordinator_msg_box = msg_open(key_feux);
    msg_send_pid(coordinator_msg_box, key_feux);
    coordinator_pid = msg_recieve_pid(coordinator_msg_box, key_coordinateur);
    printf("Coordinateur: %d\n\n", coordinator_pid);

    // On configure la prise en charge des SIGUSR1 et SIGUSR2
    signal(SIGUSR1, trap_urgence);
    signal(SIGUSR2, trap_urgence);

    printf("On attend que le coordinateur soit pret ...\n");
    wait_for_pid(coordinator_msg_box, coordinator_pid);


    // On peux enfin s'amuser avec notre feux
    while(1){
        if(urgence == (int) 0){
            switch (lire_feux()){
                case 5:
                    ecrire_etat_feux(0,1,0,1);
                    break;
                case 10:
                default:
                    ecrire_etat_feux(1,0,1,0);
            }
        }
        afficher_etat();
        usleep(temps_unitaire*4); // Toutes les 4 instances
    }
}
#pragma clang diagnostic pop