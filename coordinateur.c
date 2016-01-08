//
// Created by pvienne on 11/12/15.
//

#include <stdio.h>
#include <signal.h>
#include <sys/msg.h>
#include <unistd.h>
#include "voiture.h"
#include "coordinateur.h"
#include "lib.h"

Carrefour carrefour = {0,0,0,0,0,0};
int stopped = 0;

void quit_handle(int sig);

void quit_handle(int sig){
    printf("Pest\n");
    msg_close(carrefour.msqid_generateur_trafic);
    msg_close(carrefour.msqid_feux);
    msg_close(carrefour.msqid_generateur_trafic_prioritaire);
    logger("coordinateur.quit","Successfuly cleared message box\n");
    stopped = 1;
}

int main(){
    signal(SIGQUIT, quit_handle);
    logger("coordinateur", "Create a shared mailbox");
    carrefour.msqid_generateur_trafic = msg_create(key_generateur_trafic);
    logger("coordinateur", "Wait a PID from sub-process");
    carrefour.pid_generateur_trafic = msg_recieve_pid(carrefour.msqid_generateur_trafic, key_generateur_trafic);
    printf("Gene_Traf: %d", carrefour.pid_generateur_trafic);
    logger("coordinateur", "Send my PID from sub-process");
    msg_send_pid(carrefour.msqid_generateur_trafic, key_coordinateur);
    carrefour.msqid_generateur_trafic_prioritaire = msg_create(key_generateur_trafic_prioritaire);
    carrefour.msqid_feux = msg_create(key_feux);
    printf("Test\n");
    while(!stopped){
        printf("Test\n");
        sleep(1);
        kill(getpid(),SIGQUIT);
    }
    return 0;
}