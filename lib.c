//
// Created by pvienne on 11/12/15.
//

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include<stdarg.h>
#include <stdbool.h>
#include "headers/lib.h"

void clear_console(){
    printf("\e[1;1H\e[2J");
}

int msg_close(int msqid){
    if(msqid<0) return -2;
    int r=msgctl(msqid, IPC_RMID, NULL);
    if(r<0){
        return -1;
    } else {
        return 0;
    }
}
int msg_open(int key){
    int msqid = msgget(key, 0666|IPC_NOWAIT);
    if(msqid<0){
        sleep(1);
        return msg_open(key);
    }
    return msqid;
}
int msg_create(int key){
    int msqid = msgget(key, 0666|IPC_CREAT);
    if(msqid<0){
        logger("lib.msg_create", "Unable to create a message box [%d] (%s)", key, strerror(errno));
    }
    return msqid;
}

int msg_send_voiture(int msqid, voiture* v) {
    struct car_message message = {512l + (v->type+(2<<v->origine)), v->destination};
    int r = msgsnd(msqid, (void*) &message, sizeof(int), IPC_NOWAIT);
    free(v);
    return r;
}
voiture* msg_recieve_voiture(int msqid, int type, int origin){
    struct car_message message = {0l,0};
    if(0<=msgrcv(msqid, (void*)&message, sizeof(int), 512l + (type+(2<<origin)), IPC_NOWAIT)) {
        voiture* v=malloc(sizeof(voiture));
        v->type = type;
        v->origine = origin;
        v->destination = message.dest;
        return v;
    } else {
        return NULL;
    }
}
int msg_send_pid(int msqid, int identity){
    pid_message message = {100l + identity, getpid()};
    return msgsnd(msqid, (void*) &message, sizeof(int), 0);
}
int msg_recieve_pid(int msqid, int from_identity) {
    pid_message message = {100l + from_identity, -1};
    msgrcv(msqid, (void*) &message, sizeof(int), message.type, 0);
    return message.pid;
}

int wait_for_pid(int msqid, int identity){
    pid_message message = {200l + identity, -1};
    msgrcv(msqid, &message, sizeof(int), message.type, 0);
    return message.pid;
}

int send_ready_to_pid(int msqid, int identity){
    pid_message message = {200l + identity, 1};
    return msgsnd(msqid, &message, sizeof(int), 0);
}

int rand_without(int from, int to, int is_not) {
    if(is_not <= to && is_not >= from && to - from <= 1){
        perror("We can not create an infinite loop");
    }
    int value;
    do {
        value = (rand() % (to-from)) + from;
    } while (value == is_not);
    return value;
}

void logger(const char *tag, const char *message, ...) {
    va_list args;

    va_start(args, message);
    char string[512];
    vsprintf(string, message, args);
    va_end(args);
    time_t now;
    time(&now);
    fprintf(stdout, "%s [%d][%s]: %s\n", ctime(&now), getpid(), tag, string);
}