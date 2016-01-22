
#ifndef VOITURE_H
#define VOITURE_H

#include <sys/types.h>

#define VPRIO 1
#define VNORM 0

typedef struct voiture {
    int id;
    int origine;
    int destination;
    int type;
} voiture;

typedef struct elem_embouteillage{
    struct voiture* voiture;
    struct elem_embouteillage* next;
} elem_embouteillage;

typedef elem_embouteillage* VOITURES;


typedef struct demande_feux{
    long type; // Should be 99
    int feux;
} demande_feux;

void afficher_voiture(voiture* v);
int generate_type(voiture* v);
char origin_to_char(int origin);

#define key_generateur_trafic 50
#define key_generateur_trafic_prioritaire 51
#define key_feux 52
#define key_coordinateur 53
// Le temps unitaire doit être supérieur à 1000, sinon des concurences frapperont
#define temps_unitaire 500000

#endif