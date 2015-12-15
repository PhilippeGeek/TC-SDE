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
#include "lib.h"

int msg_close(int msqid){
    if(msqid<=0) return -2;
    int r=msgctl(msqid, IPC_RMID, NULL);
    if(r<0){
        logger("lib.msg_close", "Unable to close a message box");
        logger("lib.msg_close", strerror(errno));
        return -1;
    } else {
        return 0;
    }
}
int msg_open(int key){
    int msqid = msgget(key, 0666);
    if(msqid<0){
        logger("lib.msg_open", "Unable to open a message box");
        logger("lib.msg_open", strerror(errno));
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
int msg_send_voiture(int msqid, voiture v);
voiture msg_recieve_voiture(int msqid);
int msg_send_pid(int msqid);
int msg_recieve_pid(int msqid);

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