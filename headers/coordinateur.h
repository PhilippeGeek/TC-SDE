//
// Created by pvienne on 11/12/15.
//

#ifndef CARREFOUR_COORDINATEUR_H
#define CARREFOUR_COORDINATEUR_H

typedef struct C{
    int msqid_generateur_trafic;
    int pid_generateur_trafic;
    int msqid_generateur_trafic_prioritaire;
    int pid_generateur_trafic_prioritaire;
    int msqid_feux;
    int pid_feux;
} Carrefour;

#endif //CARREFOUR_COORDINATEUR_H
