#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED

#include "commun.h"
#include <time.h>
#include "structuresJeu.h"
#include "interfaceJeu.h"

unsigned int valeurCarte(carte *c,unsigned short typeAtout);
void encherir(joueur *encherisseur,
              unsigned short *atout,
              int *valeurContrat,
              unsigned short *coinche,
              unsigned short *surcoinche);
void distribuer();
void jeu();
void jouerCarte(joueur *j,
                carte (*pli)[4],
                unsigned short atout,
                int numJoueur,
                int valEnchereMax,
                unsigned short numPli);
unsigned short possedeCarteForte(joueur *j,unsigned short coul, unsigned short atout);
int selectionCarteClassement(joueur *j,
                             unsigned short typeAtout,
                             unsigned short coulAtout,
                             unsigned short valeurSeuil,
                             unsigned short sens);
short classementParAtout(unsigned short num,unsigned short typeAtout);

#endif
