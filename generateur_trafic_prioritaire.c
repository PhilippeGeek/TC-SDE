//
// Created by pvienne on 11/12/15.
//

#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "headers/voiture.h"
#include "headers/generateur_trafic.h"
#include "headers/lib.h"

bool stop;
int msg_box = 0;
int coordinateur = 0;

void catch_alarm(int sig){
    printf("Test\n");
    alarm(1);
    signal(SIGALRM,catch_alarm);
}

voiture* generate_car(){
    voiture* v=malloc(sizeof(voiture));
    v->type = VPRIO; // Prioritaire
    v->origine = (rand()%4);
    v->destination = rand_without(0,4,v->origine);
    v->id = rand();
    return v;
}

void stop_my_while(int sig){
    stop = 1;
}

int main(){
    printf("Generateur de véhicules prioritaires démarré,\n en attente du coordinateur ...\n");
    msg_box = msg_open(key_generateur_trafic_prioritaire);
    puts("Envoie du PID\n");
    msg_send_pid(msg_box, key_generateur_trafic_prioritaire);
    puts("Reception du PID\n");
    int pid = msg_recieve_pid(msg_box, key_coordinateur);
    printf("Coordinateur: %d\n", pid);

    signal(SIGQUIT, stop_my_while);

    while(!stop){
        usleep(temps_unitaire*20);
        printf("Envoie la gendarmerie ... ");
        if(msg_send_voiture(msg_box, generate_car())>=0) {
            kill(pid, SIGUSR1);
            printf("Sended\n");
        }
    }

    return 0;
}
