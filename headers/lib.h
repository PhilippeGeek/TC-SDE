//
// Created by pvienne on 11/12/15.
//

#include "voiture.h"

#ifndef CARREFOUR_LIB_H
#define CARREFOUR_LIB_H

typedef struct car_message{ long type; int dest; } car_message;
typedef struct pid_message { long type; int pid; } pid_message;

void clear_console();
int msg_close(int msqid);
int msg_open(int key);
int msg_create(int key);
int msg_send_voiture(int msqid, voiture* v);
voiture* msg_recieve_voiture(int msqid, int type, int origin);
int msg_send_pid(int msqid, int id);
int msg_recieve_pid(int msqid, int from_id);
void logger(const char *tag, const char *message, ...);
int rand_without(int from, int to, int not);

#endif //CARREFOUR_LIB_H
