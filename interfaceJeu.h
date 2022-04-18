#ifndef INTERFACEJEU_H_INCLUDED
#define INTERFACEJEU_H_INCLUDED

#include "commun.h"
#include "structuresJeu.h"


void selectionAtout(unsigned short *atout);
void afficherCarte(carte *c);
void interfaceEnchere(joueur *encherisseur,
                      unsigned short *atout,
                      int *valeurContrat,
                      unsigned short *coinche,
                      unsigned short *surcoinche);
void actualiserInterface(unsigned short atout,
                         char description[32],
                         joueur *nord,
                         joueur *ouest,
                         joueur *sud,
                         joueur *est,
                         int meilleureEnchere,
                         carte pli[4],
                         unsigned short numPli
                         );
void afficherCartesMain(joueur *j);
void actualiserCartesMain(joueur *joueurVisible);
unsigned int longChaineCartesMain(joueur *j);
void demandeNom(joueur *joueurHumain);
void nomAtoutCpy(char (*nomAtout)[32],unsigned short i);
void afficherCartesMain(joueur *j);
void afficherCartesPli(carte pli[4]);
int interfaceSelecCarte(joueur *j,carte (*pli)[4],unsigned short atout,unsigned short numPli);
int longChaineCartesPli(carte pli[4]);
unsigned int nbCaracCarteNMain(joueur *j,int n);

#endif
