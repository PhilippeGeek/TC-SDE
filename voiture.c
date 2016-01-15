#include <stdio.h>
#include "headers/voiture.h"

char origin_to_char(int origin){
    switch (origin){
        case 0:
            return 'N';
        case 1:
            return 'E';
        case 2:
            return 'S';
        case 3:
            return 'W';
        default:
            return '?';
    }
}

void afficher_voiture(voiture* v){
    printf("[Voiture mType=%d origine=%c destination=%c ", generate_type(v), origin_to_char(v->origine), origin_to_char(v->destination));
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

/**
 * Génère un type dépendant des caractéristiques d'origine et de destination du véhicule.
 */
int generate_type(voiture* v){

    // Check data given to this function
    if(v->destination<0||v->destination>3)
        perror("Destination must be between 0 and 3");
    if(v->origine<0||v->origine>3)
        perror("Origin must be between 0 and 3");
    if(v->type<0||v->type>1)
        perror("Type must be 0 (normal) or 1 (priority)");

    // Generate code
    return v->type + (2 << v->origine) + (32 << v->destination);
}