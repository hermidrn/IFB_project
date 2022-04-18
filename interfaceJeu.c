#include "interfaceJeu.h"

// Permet de voir les cartes des autres
// A utiliser uniquement pour tester le fonctionnement de l'IA car l'interface ne prévoit pas l'affichage complet des cartes
#define AFFICHER_CARTES_ADVERSAIRES 0

// Demander au joueur son nom et faire quelques contrôles
void demandeNom(joueur *joueurHumain) {
    effacerConsole();
    printf("═════════════════════════ NOUVELLE PARTIE ══════════════════════════\n Entrez votre nom: ");
    gets(joueurHumain->pseudo);
    if (strlen(joueurHumain->pseudo)<=0) {
        printf("Vous n'avez saisi de nom. Vous serez appelé Sud.\n[Appuyez sur une touche]");
        strcpy(joueurHumain->pseudo,"Sud");
        getch(); // Attendre l'appui sur une touche
    }
    else while (strlen(joueurHumain->pseudo)>41) {
        printf("Votre nom est trop long pour être affiché dans l'interface !\nEntrez un autre nom plus court : ");
        gets(joueurHumain->pseudo);
    }
    effacerConsole();
}

// Actualiser l'interface principale de jeu
void actualiserInterface(unsigned short atout,
                         char description[32],
                         joueur *nord,
                         joueur *ouest,
                         joueur *sud,
                         joueur *est,
                         int meilleureEnchere,
                         carte pli[4],
                         unsigned short numPli
                         ) {
    effacerConsole();
    printf("\33[97m╔═════════════════╤════════════╤════════════════╤══════════════════╗\n║ BELOTE COINCHÉE │ ");
    if (strcmp(description,"Pli ?/8")==0) {
        printf(" Pli %d/8",numPli+1);
    }
    else {
        printf(" \33[96m%s\33[97m ",description);
    }
    for(int i=0;i<10-strlen(description);i++) {printf(" ");}
    printf("│ ");
    if(meilleureEnchere!=0) {
        printf("%d",meilleureEnchere);
        switch(atout) {
            case 0:
                printf(" sans atout ");
                break;
            case 1:
                printf(" ♠          ");
                break;
            case 2:
                printf(" ♣          ");
                break;
            case 3:
                printf(" \33[91m♥\33[97m          ");
                break;
            case 4:
                printf(" \33[91m♦\33[97m          ");
                break;
            case 5:
                printf(" ♠♣\33[91m♥♦\33[97m       ");
                break;
        }
        if(meilleureEnchere<100) {printf(" ");}
    }
    else {
        printf("Aucun contrat  ");
    }
    printf("│                  ║\n╟─────────────────┴────────────┴────────────────┴──────────────────╢\n║                         ▼ %s                                   ║\n║                         ",nord->pseudo);
    afficherCartesMain(nord);
    printf("                         ║\n║                                                                  ║\n║   ► %s                                     ◄ %s              ║\n║   ",ouest->pseudo,est->pseudo);
    afficherCartesMain(ouest);
    printf("                            ");
    afficherCartesMain(est);
    printf("   ║\n║                                                                  ║\n║                       ┌ PLI ────────────┐                        ║\n║                       │ ");
    afficherCartesPli(pli);
    for(int i=0;i<(15-longChaineCartesPli(pli));i++) {printf(" ");}
    printf(" │                        ║\n║                       └─────────────────┘                        ║\n║                     ▲ %s",sud->pseudo);
    for(int i=0;i<43-strlen(sud->pseudo);i++) {printf(" ");}
    printf("║\n║                     ");
    afficherCartesMain(sud);
    for(int i=0;i<29-longChaineCartesMain(sud);i++) {printf(" ");}
    printf("║\n╚══════════════════════════════════════════════════════════════════╝\n");
}

// Affiche les cartes composant la main du joueur et permet de sélectionner une carte
// à mettre dans le pli avec les flèches du clavier
int interfaceSelecCarte(joueur *j,carte (*pli)[4],unsigned short atout, unsigned short numPli) {
    printf("Choisissez une carte à jouer avec les flèches gauche/droite\npuis appuyez sur [ENTRÉE] :\n");
    unsigned short carteAutorisee=0;
    unsigned short nbCartesCoulEntame, nbCartesAtout;


    afficherCartesMain(j);
    printf("\nAppuyez sur [←] ou [→]");

    int selection=0;
    int quitter=0;
    do {
        switch(getch()) {
            case 'K':
                if(selection>0){
                    selection--;
                }
                else {
                    selection=7-numPli;
                }
                break;
            case 'M':
                selection=(selection+1)%(8-numPli); // Modulo permettant de revenir au début si la fin de la main est atteinte
                break;
            case '\r':
                if (carteAutorisee){quitter=1;}
                break;
        }
        // Vérifier si le joueur a le droit de jouer la carte dans le contexte actuel
        carteAutorisee=0;
        nbCartesCoulEntame=0;
        nbCartesAtout=0;
        // Compter le nombre d'atouts et de couleur demandée
        for(int i=0;i<8-numPli;i++) {
            if(j->main[i].coul==atout) {nbCartesAtout++;}
            if (j->main[i].coul==pli[0]->coul) {nbCartesCoulEntame++;}
        }

        if (pli[0]->num==0) {
            carteAutorisee=1;
        }
        else {
            if (nbCartesCoulEntame>0) {
                if (j->main[selection].coul==pli[0]->coul) {
                    carteAutorisee=1;
                }
                else {
                    carteAutorisee=0;
                }
            }
            else if (nbCartesAtout>0) {
                if (j->main[selection].coul==atout) {
                    carteAutorisee=1;
                }
                else {
                    carteAutorisee=0;
                }
            }
            else {
                carteAutorisee=1;
            }
        }



        espaces(30);
        printf("\r");
        for (int i=0;i<=nbCaracCarteNMain(j,selection);i++) {printf(" ");}
        if (carteAutorisee) {
            printf("▲ CHOISIR ");
            afficherCarte(&j->main[selection]);
        }
        else {
            printf("\33[91m▲ INTERDIT\33[97m");
        }
        espaces(30);
        printf("\r");
    } while(!quitter);
    printf("Vous avez sélectionné ");
    afficherCarte(&j->main[selection]);
    espaces(20);
    printf("\n");
    return selection;
}

void afficherCartesPli(carte pli[4]) {
    for(int i=0;i<4;i++) {
        afficherCarte(&pli[i]);
        printf(" ");
    }
}

int longChaineCartesPli(carte pli[4]) {
    int longueur=0;
    for(int i=0;i<4;i++) {
        if (pli[i].num==10) {longueur+=4;}
        else {longueur+=3;}
    }
    return longueur;
}


// Actualise l'affichage des cartes dans la main du joueur j
void afficherCartesMain(joueur *j) {
    for(int i=0;i<8;i++) {
        if (j->humain || AFFICHER_CARTES_ADVERSAIRES) {
            afficherCarte(&j->main[i]);
        }
        else {
            if (j->main[i].num==0) {
                printf(" ");
            }
            else {
                printf("▓"); // Les carte des autres joueurs sont face retournées
            }

        }
        printf(" ");
    }
}

// Longueur de la chaîne affichée par afficherCartesMain
unsigned int longChaineCartesMain(joueur *j) {
    unsigned int longueur=0;
    for(int i=0;i<8;i++) {
        if (j->main[i].num==10) {
            longueur+=2;
        }
        else {
            longueur++;
        }
    }
    return longueur;
}

// Similaire à longChaineCartesMain mais indique la longueur en caractères qu'il faut pour parvenir au bout de la N ième carte
unsigned int nbCaracCarteNMain(joueur *j,int n) {
    unsigned int longueur=0;
    for(int i=0;i<n;i++) {
        if (j->main[i].num==10) {
                longueur+=4;
        }
        else {longueur+=3;}
    }
    return longueur;
}

// Convertit une strcuture carte en chaîne de caractère lisible comme "R♠"
void afficherCarte(carte *c) {
    couleur(30);
    couleur(107);
    // Couleur de la carte
    char chaineCoul[32] = " ";
    switch (c->coul) {
        case 1:
            strcpy(chaineCoul,"\33[30m♠");
            break;
        case 2:
            strcpy(chaineCoul,"\33[30m♣");
            break;
        case 3:
            strcpy(chaineCoul,"\33[91m♥");
            break;
        case 4:
            strcpy(chaineCoul,"\33[91m♦");
            break;
        default:
            couleur(97);
            couleur(40);
            break;
    }

    // Numéro de la carte
    char chaineNum[3] = {' ',0};
    switch (c->num) {
        case 14:
            strcpy(chaineNum,"A");
            break;
        case 7 ... 9:
            chaineNum[0] = c->num+48; // Convertir l'entier en chaîne (caractère 48 == '0')
            break;
        case 10:
            strcpy(chaineNum,"10");
            break;
        case 11:
            strcpy(chaineNum,"V");
            break;
        case 12:
            strcpy(chaineNum,"D");
            break;
        case 13:
            strcpy(chaineNum,"R");
            break;
        default: // Ne pas afficher le fond de la carte si elle n'existe pas
            couleur(97);
            couleur(40);
    }
    printf("%s%s\33[40m\33[97m",chaineNum,chaineCoul);
}

// Fonction permettant au joueur de choisir la couleur d'atout lorsque applicable
void selectionAtout(unsigned short *atout){
    char nomAtout[32];
    unsigned short quitter=0;
    //unsigned short dernierAtout=*atout;
    do {
                nomAtoutCpy(&nomAtout,*atout);
                printf("\rChoisir une couleur d'atout : \33[30m\33[107m◄\33[47m %s \33[107m►\33[97m\33[40m    ",nomAtout);
                switch(getch()) {
                    case 'K':
                        if (*atout>0) {(*atout)--;}
                        break;
                    case 'M':
                        if (*atout<5) {(*atout)++;}
                        break;
                    case 13:
                        quitter=1;
                        break;
                }
            } while (!quitter);

}

void interfaceEnchere(joueur *encherisseur,
                      unsigned short *atout,
                      int *valeurContrat,
                      unsigned short *coinche,
                      unsigned short *surcoinche) {
    int valeurMin=80; // Valeur minimale du contrat
    unsigned short peutCoincher;

    if (*valeurContrat!=0){
        peutCoincher=1;
    }
    else {
        peutCoincher=0;
    }

    printf("\n\n╔═════ CHOISIR UN TYPE DE CONTRAT ═════╗\n║ [E] Enchère                          ║\n║ [K] Capot [G] Générale               ║");
    if(peutCoincher) {printf("\n║ [C] Coincher                         ║");}
    printf("\n║ [Autre touche] Passer                ║\n╚══════════════════════════════════════╝\n");
    switch(getch()) {
        case 'E':
        case 'e': // ENCHERE NORMALE

            if (*valeurContrat!=0) {valeurMin=*valeurContrat+10;}
            *valeurContrat=valeurMin;
            unsigned short quitter=0;
            printf("Utilisez les touches gauche/droite pour vous déplacer.\n");
            do {
                printf("\rValeur de l'enchère : \33[30m\33[107m◄\33[47m%hu\33[107m►\33[97m\33[40m points",*valeurContrat); // Remplace la ligne à chaque modification de l'enchère
                switch(getch()) {
                    case 'K':
                        if (*valeurContrat>valeurMin) {*valeurContrat-=10;}
                        break;
                    case 'M':
                        if (*valeurContrat<160) {*valeurContrat+=10;}
                        break;
                    case 13:
                        quitter=1;
                        break;
                }
            } while (!quitter);
            selectionAtout(atout);

            char atoutSelec[32];
            nomAtoutCpy(&atoutSelec,*atout);
            printf("\rVous avez annoncé une enchère de %d points en atout %s",*valeurContrat,atoutSelec);


            break;
        case 'K':
        case 'k': // CAPOT
            printf("Vous faites capot.\n");
            *valeurContrat=250;
            break;
        case 'G':
        case 'g': // GENERALE
            printf("Vous faites une générale\n");
            *valeurContrat=500;
            break;
        case 'C':
        case 'c': // COINCHE
            if (peutCoincher) {
                printf("Vous coinchez\n");
                *coinche=1;
            }
            else {
                printf("Vous passez votre tour\n");
            }
            break;
        /*
        case 's': // SURCOINCHE
            printf("Vous surcoinchez\n");
            break;
        */
        default: // PASSER SON TOUR
            printf("Vous passez votre tour\n");
            break;
    }
    printf("\n");
}

void nomAtoutCpy(char (*nomAtout)[32],unsigned short i) {
    char listeNomsAtouts[6][32]={"Sans atout","Pique ♠","Trèfle ♣","Cœur ♥","Carreau ♦","Tout atout"};
    strcpy(*nomAtout,listeNomsAtouts[i]);
}
