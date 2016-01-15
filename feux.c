//
// Created by pvienne on 11/12/15.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
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

void trap_urgence(int sig){
    switch(sig){
        case SIGUSR1:
            urgence = 1;
            // TODO: Organiser la réception de la demande de passage
            break;
        case SIGUSR2:
            urgence = 0;
            ecrire_etat_feux(1,0,1,0);
            break;
        default:
            logger("feux","Signal étrange reçu : %d", sig);
    }
}

void afficher_etat(){
    int state = lire_feux();
    clear_console();

    printf("Nord  ");
    if(state&0b1){
        printf("GREEN");
    } else {
        printf("RED");
    }
    printf("\n");

    printf("Sud   ");
    if(state&0b100){
        printf("GREEN");
    } else {
        printf("RED");
    }
    printf("\n");

    printf("Est   ");
    if(state&0b10){
        printf("GREEN");
    } else {
        printf("RED");
    }
    printf("\n");

    printf("Ouest ");
    if(state&0b1000){
        printf("GREEN");
    } else {
        printf("RED");
    }
    printf("\n");
}

int main(){

    // On trap les signaux d'interruption pour la gestion de la mémoire partagé.
    signal(SIGQUIT, quit);
    signal(SIGINT, quit);

    printf("== Traffic Lights Management (%d) == \n\n", getpid());

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
        if(sem_feux<=0){
            printf("Le sémaphore n'a pas été libéré correctement!\n");
            sem_feux = open_semaphore(key_feux);
            if(sem_feux<=0){
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

    // On peux enfin s'amuser avec notre feux
    while(1){
        usleep(temps_unitaire*4); // Toutes les 4 instances
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
    }
}
#pragma clang diagnostic pop