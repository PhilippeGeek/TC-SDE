
#ifndef VOITURE_H
#define VOITURE_H

#define VPRIO 1
#define VNORM 0

typedef struct voiture {
    int origine;
    int destination;
    int type;
} voiture;

typedef struct elem_embouteillage{
    voiture* voiture;
    elem_embouteillage* next;
} elem_embouteillage;

typedef elem_embouteillage* VOITURES;

#endif