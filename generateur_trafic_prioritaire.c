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
#include "voiture.h"
#include "generateur_trafic.h"
#include "lib.h"

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
    msg_send_pid(msg_box, key_generateur_trafic_prioritaire);
    int pid = msg_recieve_pid(msg_box, key_coordinateur);
    printf("Coordinateur: %d\n", pid);

    signal(SIGQUIT, stop_my_while);

    while(!stop){
        usleep(500000);
        printf("Sending to msg_box ... ");
        if(msg_send_voiture(msg_box, generate_car())==-1) { // Rejected, it's full !
            sleep(2);
        } else {
            printf("Sended\n");
        }
    }

    return 0;
}
