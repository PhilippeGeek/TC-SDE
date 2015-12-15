
#ifndef VOITURE_H
#define VOITURE_H

#include <sys/types.h>

#define VPRIO 1
#define VNORM 0

typedef struct voiture {
    int origine;
    int destination;
    int type;
} voiture;

typedef struct elem_embouteillage{
    struct voiture* voiture;
    struct elem_embouteillage* next;
} elem_embouteillage;

typedef elem_embouteillage* VOITURES;

void afficher_voiture(voiture* v);

#define key_generateur_trafic 42
#define key_generateur_trafic_prioritaire 1337
#define key_feux 2048


#endif