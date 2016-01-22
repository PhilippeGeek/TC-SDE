//
// Created by pvienne on 11/12/15.
//

#include <stdio.h>
#include <signal.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include "lib/semaphore.h"
#include "lib/shmem.h"
#include "headers/voiture.h"
#include "headers/coordinateur.h"
#include "headers/lib.h"

Carrefour carrefour = {0,0,0,0,0,0};
int stopped = 0;
int* feux;
int sem_feux;
int voitures = 0;

void quit_handle(int sig);

void traiter_urgence(int sig);

void quit_handle(int sig){
    printf("\n========================\n\nSIGNAL: %d\n\n", sig);

    printf("On a traité au total %d véhicules\n\n", voitures);

    printf("On dit au générateur de trafic de se stopper ... ");
    kill(carrefour.pid_generateur_trafic, SIGQUIT);
    printf("OK\n");

    printf("On dit au générateur de trafic prioritaire de se stopper ... ");
    kill(carrefour.pid_generateur_trafic_prioritaire, SIGQUIT);
    printf("OK\n");

    printf("On dit au feu de s'arrêter ... ");
    kill(carrefour.pid_feux, SIGQUIT);
    printf("OK\n");

    printf("On ferme les boites de messages ... ");
    msg_close(carrefour.msqid_generateur_trafic);
    msg_close(carrefour.msqid_feux);
    msg_close(carrefour.msqid_generateur_trafic_prioritaire);
    printf("OK\n");

    stopped = 1;
    printf("Gracefully stopped ;-)\n");
    exit(0);
}

void check_open_and_share_pid(int key, int* msg_box, int* pid) {
    (*msg_box) = msg_create(key);
    (*pid) = msg_recieve_pid((*msg_box), key);
    msg_send_pid(*msg_box, key_coordinateur);
}

int lire_feux(){
    int r_buffer = 0;
    down(sem_feux);
    r_buffer = (*feux);
    up(sem_feux);
    return r_buffer;
}

void new_urgence_in_urgence_mode(int sig){

}

void trap_urgence(int sig){
    switch(sig){
        case SIGUSR1:
            signal(SIGUSR1, traiter_urgence);
            puts("Mode d'urgence !");
            //"Passage des feux en urgence");
            kill(carrefour.pid_feux, SIGUSR1);
            //"Prise en charge des véhicules d'urgences ...");
            traiter_urgence(sig);
            //"Passage des feux en normal");
            kill(carrefour.pid_feux, SIGUSR2);
            signal(SIGUSR1, trap_urgence);
            break;
        default:
            logger("coordinateur","Signal étrange reçu : %d", sig);
    }
}

void traiter_urgence(int sig) {
    int i = 0;
    for (i = 0; i < 4; i++) {
        voiture *v = NULL;
        int j = 0, v_count = 0;
        do {
            v = msg_recieve_voiture(carrefour.msqid_generateur_trafic_prioritaire, VPRIO, i);
            if (v != NULL) {
                printf("Prioritaire arrivant de %c et allant à %c\n", origin_to_char(v->origine), origin_to_char(v->destination));
                demande_feux demande = {99l, i};
                // Envoie de la demande de changement de feu
                msgsnd(carrefour.msqid_feux, &demande, sizeof(int), 0);
                // Aquittement de la demande
                msgrcv(carrefour.msqid_feux, &demande, sizeof(int), 98l, 0);
                printf("Passage du véhicule\n");
                sleep(1);
                v_count++;
                free(v);
            }
            j++;
        } while (v != NULL && j < 40);
    }
}

int main(){
    signal(SIGQUIT, quit_handle);
    signal(SIGINT, quit_handle);

    puts(" == Bienvenu sur le coordinateur de trafic == \n\n");

    puts("Connexion aux feux de circulation ... ");
    check_open_and_share_pid(key_feux, &carrefour.msqid_feux, &carrefour.pid_feux);
    sem_feux = open_semaphore(key_feux);
    int shmem_id = 0;
    int tries = 0;
    do {
        if(tries!=0) usleep(temps_unitaire);
        shmem_id = open_shmem(key_feux, sizeof(int));
        tries++;
    } while(tries<5 && shmem_id <= 0);
    feux = attach_shmem(shmem_id);

    puts("OK");

    puts("Connexion au générateur de trafic ... ");
    check_open_and_share_pid(key_generateur_trafic, &carrefour.msqid_generateur_trafic, &carrefour.pid_generateur_trafic);
    puts("OK");

    puts("Connecting to priory traffic generator process ... ");
    check_open_and_share_pid(key_generateur_trafic_prioritaire, &carrefour.msqid_generateur_trafic_prioritaire, &carrefour.pid_generateur_trafic_prioritaire);
    puts("OK");

    send_ready_to_pid(carrefour.msqid_feux, getpid());
    send_ready_to_pid(carrefour.msqid_generateur_trafic_prioritaire, getpid());
    send_ready_to_pid(carrefour.msqid_generateur_trafic, getpid());

    puts("\nReady to run\n\n");

    signal(SIGUSR1, trap_urgence);



    while(!stopped){
        usleep(temps_unitaire);

        // On vérifie la boite des véhicules normaux
        {
            int i = 0;
            for (i = 0; i < 4; i++) {
                down(sem_feux); // On gère l'itération, donc on evite que les feux soient modifiés
                if (*feux & (1 << i)) {
                    voiture *v = NULL;
                    int j = 0, v_count = 0;
                    do {
                        v = msg_recieve_voiture(carrefour.msqid_generateur_trafic, VNORM, i);
                        if (v != NULL) {
                            v_count++;
                            printf("Passage d'un véhicule allant de %c à %c\n", origin_to_char(v->origine), origin_to_char(v->destination));
                            free(v);
                        }
                        j++;
                    } while (v != NULL && j < 40);
                    voitures += v_count;
                }
                up(sem_feux); // On relache les feux
            }
        }

    }

    quit_handle(SIGQUIT);
    return 1;
}