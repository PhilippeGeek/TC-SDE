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
}

int main(){
    signal(SIGQUIT, quit_handle);
    carrefour.msqid_generateur_trafic = msg_create(key_generateur_trafic);
    carrefour.msqid_generateur_trafic_prioritaire = msg_create(key_generateur_trafic_prioritaire);
    carrefour.msqid_feux = msg_create(key_feux);
    printf("Test\n");
    while(!stopped){
        printf("Test\n");
        sleep(1);
        kill(getpid(),SIGQUIT);
    }
    quit_handle(SIGQUIT);
    return 0;
}