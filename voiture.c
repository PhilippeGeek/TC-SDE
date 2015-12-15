#include <stdio.h>
#include "voiture.h"

void afficher_voiture(voiture* v){
    printf("[Voiture origine=%d destination=%d ", v->origine, v->origine);
    switch(v->type){
        case VPRIO:
            printf("prioritaire");
            break;
        case VNORM:
            printf("normal");
            break;
        default:
            printf("??");
            break;
    }
    printf("]");
}