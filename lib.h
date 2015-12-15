//
// Created by pvienne on 11/12/15.
//

#include "voiture.h"

#ifndef CARREFOUR_LIB_H
#define CARREFOUR_LIB_H

int msg_close(int msqid);
int msg_open(int key);
int msg_create(int key);
int msg_send_voiture(int msqid, voiture v);
voiture msg_recieve_voiture(int msqid);
int msg_send_pid(int msqid);
int msg_recieve_pid(int msqid);
void logger(const char *tag, const char *message, ...);

#endif //CARREFOUR_LIB_H
