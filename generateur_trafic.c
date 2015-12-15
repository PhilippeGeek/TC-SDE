//
// Created by pvienne on 11/12/15.
//

#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "voiture.h"
#include "generateur_trafic.h"

bool stop;

void catch_alarm(int sig){
    printf("Test\n");
    alarm(1);
    signal(SIGALRM,catch_alarm);
}

int main(){
    
    signal(SIGALRM, catch_alarm);
    alarm(1);
    stop = 0;
    while(!stop) {
        sleep(3600);
    }
    return 0;
}
