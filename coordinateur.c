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

voiture from_N_to_E = {0,0,1,0};
voiture from_N_to_S = {0,0,2,0};
voiture from_N_to_W = {0,0,3,0};
voiture from_E_to_N = {0,1,0,0};
voiture from_E_to_S = {0,1,2,0};
voiture from_E_to_W = {0,1,3,0};
voiture from_S_to_N = {0,2,0,0};
voiture from_S_to_E = {0,2,1,0};
voiture from_S_to_W = {0,2,3,0};
voiture from_W_to_N = {0,3,0,0};
voiture from_W_to_E = {0,3,1,0};
voiture from_W_to_S = {0,3,2,0};

Carrefour carrefour = {0,0,0,0,0,0};
int stopped = 0;
int* feux;
int sem_feux;

void quit_handle(int sig);

void quit_handle(int sig){
    printf("\n========================\n\nSIGNAL: %d\n\n", sig);

    printf("On dit au générateur de trafic de se stopper ... ");
    kill(carrefour.pid_generateur_trafic, SIGQUIT);
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

int main(){
    signal(SIGQUIT, quit_handle);
    signal(SIGINT, quit_handle);

    puts(" == Bienvenu sur le coordinateur de trafic - v0.1 == \n\n");

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

    puts("OK\n");

    puts("Connexion au générateur de trafic ... ");
    check_open_and_share_pid(key_generateur_trafic, &carrefour.msqid_generateur_trafic, &carrefour.pid_generateur_trafic);
    puts("OK\n");

    /*puts("Connecting to priory traffic generator process ... ");
    check_open_and_share_pid(key_generateur_trafic_prioritaire, &carrefour.msqid_generateur_trafic_prioritaire, &carrefour.pid_generateur_trafic_prioritaire);
    puts("OK\n");*/

    puts("\nReady to run\n\n");

    int voitures = 0;

    while(!stopped){
        usleep(temps_unitaire);
        int i = 0;
        for (i=0; i<512; i++){
            struct car_message message = {0l,0};
            if(0<=msgrcv(carrefour.msqid_generateur_trafic, (void*)&message, sizeof(int), 512l + i, IPC_NOWAIT)) {
                voitures++;
            }
        }
        clear_console();
        printf("%d voitures\n", voitures);

    }

    quit_handle(SIGQUIT);
    return 1;
}