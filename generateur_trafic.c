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
#include <sys/msg.h>
#include "headers/voiture.h"
#include "headers/generateur_trafic.h"
#include "headers/lib.h"

bool stop;
int msg_box = 0;
int coordinateur = 0;
int pid;

void catch_alarm(int sig){
    printf("Test\n");
    alarm(1);
    signal(SIGALRM,catch_alarm);
}

voiture* generate_car(){
    voiture* v=malloc(sizeof(voiture));
    v->type = VNORM; // Non prioritaire
    v->origine = (rand()%4);
    v->destination = rand_without(0,4,v->origine);
    v->id = rand();
    return v;
}

void stop_my_while(int sig){
    stop = 1;
    kill(pid, SIGQUIT);
}

int main(){
    printf("Generateur de véhicules normaux démarré,\n en attente du coordinateur ...\n");
    msg_box = msg_open(key_generateur_trafic);
    msg_send_pid(msg_box, key_generateur_trafic);
    pid = msg_recieve_pid(msg_box, key_coordinateur);
    printf("Coordinateur: %d\n", pid);

    printf("On attend que le coordinateur soit pret ...\n");
    wait_for_pid(msg_box, pid);

    struct msqid_ds data;
    signal(SIGQUIT, stop_my_while);
    signal(SIGINT, stop_my_while);

    while(!stop){
        msg_send_voiture(msg_box, generate_car());
        clear_console();
        msgctl(msg_box, IPC_STAT, &data);
        printf("%ld voitures en attente de passage\n", data.msg_qnum);
        usleep((temps_unitaire / (unsigned int)(rand() % 4 + 1)));
    }

    return 0;
}
