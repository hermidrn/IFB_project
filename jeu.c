#include "jeu.h"
#define IA_SAIT_ENCHERIR 1

// Couleurs : 0=SA 1=♠, 2=♣, 3=♥, 4=♦, 5=TA

// Fonction principale permettant le déroulement de chaque phase du jeu
void jeu() {
    // Initialisation de l'aléatoire
    srand(time(0));

    // Fixer la taille de la console
    #ifdef _WIN32
    system("mode con cols=68 lines=36 > nul");
    #endif

    // Déclaration des variables et des structures
    unsigned short atout=0; // Couleur de l'atout
    int valeurContrat = 0; // 250=capot, 500=générale, 0=pas encore de contrat
    joueur *meilleurEncherisseur=NULL;
    unsigned short coinche = 0; // 0: pas de coinche, 1: l'équipe adverse du meilleur enchérisseur à coinché
    unsigned short surcoinche = 0; // 0: pas de surcoinche: 1: l'équipe du meilleur encherisseur a surcoinché
    unsigned short numPli=0;
    carte pli[4] = {0};
    unsigned int nbContratsPasses=0;
    unsigned int nbContratsTotal=0;
    int ptsPliEquipe[2] = {0};
    int ptsTotalEquipe[2] = {0};

    // Initialisation des joueurs
    joueur est={"Est",0,1};
    joueur nord={"Nord",0,0};
    joueur ouest={"Ouest",0,1};
    joueur sud={"Votre nom",1,0}; // Le joueur sud est le joueur non controlé par une AI
    //joueur *(equipes[2][2])={{&sud,&nord},{&ouest,&est}};

    endroitSiAucunContrat:;
    // Création de l'ordre de jeu
    joueur *sensAntihoraire[4] = {&est,&nord,&ouest,&sud};
    joueur *ordreJoueur[4];
    joueur *ancienOrdreJoueur[4];
    unsigned short numDonneur=rand()%4;
    for(int i=0;i<4;i++) {
        ordreJoueur[i]=sensAntihoraire[(i+numDonneur)%4];
    }

    // Demande au joueur non IA (placé au sud) son nom
    // pour la sauvegarde du score
    demandeNom(&sud);

    // DEBUT DISTRIBUTION CARTES *******************************************************************

    // Création d'une liste vide pour accueillir toutes les cartes du jeu
    carte listeJeuCartes[32];

    // Remplissage de la liste par les cartes
    unsigned short i=0;
    for(unsigned short c=1;c<=4;c++) {
        for(unsigned short v=7;v<=14;v++) {
            listeJeuCartes[i].coul=c;
            listeJeuCartes[i].num=v;
            i++;
        }
    }

    // Mélange du jeu de cartes en tirant une carte aléatoire et en modifiant sa valeur à 99
    // Si on tire une carte 99, on avance dans la liste jusqu'a la suivante car elle à déjà été piochée
    unsigned short posJeuCartes;
    for(int j=0;j<4;j++) {
        for(int i=0;i<8;i++) {
            posJeuCartes=rand()%32;
            while (listeJeuCartes[posJeuCartes].num==99) {
                posJeuCartes=(posJeuCartes+1)%32;
            }
            ordreJoueur[j]->main[i]=listeJeuCartes[posJeuCartes];
            listeJeuCartes[posJeuCartes].num=99;
        }
    }

    // FIN DISTRIBUTION CARTES *********************************************************************

    // Actualiser l'interface et afficher le donneur
    actualiserInterface(atout,"Enchères",&nord,&ouest,&sud,&est,valeurContrat,pli,numPli);
    printf("Le donneur %s a distribué les cartes...\n\n",ordreJoueur[3]->pseudo);

    // PHASE D'ENCHERES ****************************************************************************
    nbContratsPasses=0; // Nombre de fois où un joueur a passé son tour
    nbContratsTotal=0; // Nombre de fois où un joueur a fait une enchère ou a passé son tour
    do {
        int derniereEnchere=valeurContrat;
        encherir(ordreJoueur[nbContratsTotal%4],
                 &atout,
                 &valeurContrat,
                 &coinche,
                 &surcoinche);
        nbContratsTotal++;
        if (derniereEnchere==valeurContrat) {
            nbContratsPasses++; // Incrémente le nombre de fois où le joueur à passé son tour
        }
        else {
            nbContratsPasses=0;
            meilleurEncherisseur=ordreJoueur[nbContratsTotal%4];
        }
        printf("Appuyez sur \33[44m[ENTRÉE]\33[40m pour passer au joueur suivant.");
        while (getch()!='\r') {} // Tant que la touche entrée n'est pas appuyée, ne rien faire
        actualiserInterface(atout,"Enchères",&nord,&ouest,&sud,&est,valeurContrat,pli,numPli);
    } while (!( (nbContratsPasses>=3 && valeurContrat>0) || (nbContratsPasses>=4 && valeurContrat==0) || coinche));
     // Si 3 personnes de suite passent leur tour, l'enchère se termine

     // Si personne n'a fait d'enchère après un tour,
    if (nbContratsPasses>=4 && nbContratsTotal==4) {
        printf("\33[91mPersonne n'a fait d'enchère !\33[97m\nAppuyez sur \33[44m[ENTRÉE]\33[40m pour redistribuer les cartes.");
        while (getch()!='\r') {}
        goto endroitSiAucunContrat;
    }
    else {
        printf("\33[92mLa phase d'enchères est terminée !\33[97m\n");
    }
    printf("Appuyez sur \33[44m[ENTRÉE]\33[40m pour passer à la phase des plis.");
    while (getch()!='\r') {}
    // FIN PHASE ENCHERES **************************************************************************

    // DEBUT DES 8 PLIS ****************************************************************************
    for(numPli=0;numPli<8;numPli++) {
        for(unsigned short numJoueur=0;numJoueur<4;numJoueur++) {
            actualiserInterface(atout,"Pli ?/8",&nord,&ouest,&sud,&est,valeurContrat,pli,numPli);
            jouerCarte(ordreJoueur[numJoueur],&pli,atout,numJoueur,valeurContrat,numPli);

            // Décale les cartes pour éviter les trous
            for(int i=0;i<=6-numPli;i++) {
                if(ordreJoueur[numJoueur]->main[i].num==0) {
                    for (int j=i;j<=7-numPli;j++) {
                        ordreJoueur[numJoueur]->main[j]=ordreJoueur[numJoueur]->main[j+1];
                    }
                    break; // Dès que le trou à été trouvé et que les cartes ont été décalées il est inutile de continuer à chercher les trous, on quitte la boucle
                }
            }

            printf("Appuyez sur \33[44m[ENTRÉE]\33[40m pour passer au joueur suivant.");
            while (getch()!='\r') {}
        }

        // Comptage de points : vérif gagnant
        // Gagnant : atout sinon couleur demandée

        #ifndef classementMeilleureCartePli
        short classementMeilleureCartePli;
        unsigned short indiceMeilleureCartePli;
        #endif
        classementMeilleureCartePli=0;

        // Rechercher la carte gagnante du pli avec priorité sur l'atout

        // Pour un atout unique
        if(atout>=1 && atout<=4) {
            for(int i=0;i<4;i++) {
                //printf("Meilleure carte trouvée i%d classement %d\n",indiceMeilleureCartePli,classementMeilleureCartePli);
                //printf("Dernière carte scannée : i%d classement %d en atout\n",i,classementParAtout(pli[i].num,1));
                // Si la carte n'est pas un emplacement vide
                if(pli[i].num>=7) {
                    // Chercher meilleur atout avec 100 rangs de plus pour que l'atout soit toujours gagant s'il existe
                    if(pli[i].coul==atout) {
                        if(classementParAtout(pli[i].num,1)+100>classementMeilleureCartePli) {
                            classementMeilleureCartePli=classementParAtout(pli[i].num,1)+100;
                            indiceMeilleureCartePli=i;
                        }
                    }
                    // Chercher meilleure carte de la couleur d'entame
                    else {
                       if(classementParAtout(pli[i].num,2)>classementMeilleureCartePli && pli[i].coul==pli[0].coul) {
                            classementMeilleureCartePli=classementParAtout(pli[i].num,2);
                            indiceMeilleureCartePli=i;
                        }
                    }
                }
            }
        }
        // Pour tout atout/sans atout
        else {
            for(int i=0;i<4;i++) {
                // Si la carte n'est pas un emplacement vide
                if(pli[i].num>=7) {
                    if (atout==0 && classementParAtout(pli[i].num,0)>classementMeilleureCartePli && pli[i].coul==pli[0].coul) {
                        classementMeilleureCartePli=classementParAtout(pli[i].num,0);
                        indiceMeilleureCartePli=i;
                    }
                    else if (atout==5 && classementParAtout(pli[i].num,3)>classementMeilleureCartePli && pli[i].coul==pli[0].coul) {
                        classementMeilleureCartePli=classementParAtout(pli[i].num,0);
                        indiceMeilleureCartePli=i;
                    }
                }
            }
        }
        // Fin de la recherche de la carte/joueur gagnante du pli



        // Déterminer le premier emplacement vide des cartes remportées du joueur gagnant
        #ifndef debutVideCartesRemportees
        unsigned short debutVideCartesRemportees;
        #endif
        debutVideCartesRemportees=0;
        while(ordreJoueur[indiceMeilleureCartePli]->cartesRemportees[debutVideCartesRemportees].num!=0) {
            debutVideCartesRemportees++;
        }


        // Copier les cartes du pli vers les cartes remportées du joueur
        for(i=0;i<4;i++) {
            ordreJoueur[indiceMeilleureCartePli]->cartesRemportees[debutVideCartesRemportees+i]=pli[i];
        }

        // Afficher le gagnant
        actualiserInterface(atout,"Pli ?/8",&nord,&ouest,&sud,&est,valeurContrat,pli,numPli);
        printf("\33[92mTous les joueurs ont joué pour ce pli et %s remporte les cartes\33[97m\n",ordreJoueur[indiceMeilleureCartePli]->pseudo);

        // 10 de der
        if(numPli==7) {
            ptsPliEquipe[ordreJoueur[indiceMeilleureCartePli]->equipe]+=10;
            printf("Son équipe gagne le «10 de der».\n");
        }

        // Changer l'ordre de jeu et commencer par le gagnant
        for(int i=0;i<4;i++) {ancienOrdreJoueur[i]=ordreJoueur[i];}
        for(int i=0;i<4;i++) {ordreJoueur[i]=ancienOrdreJoueur[(i+indiceMeilleureCartePli)%4];}

        if (numPli<7) {
            printf("Appuyez sur \33[44m[ENTRÉE]\33[40m pour passer au pli suivant.");
        }
        else {
            printf("\33[92mLes 8 plis sont terminés !\33[97m\nAppuyez sur \33[44m[ENTRÉE]\33[40m pour passer au comptage de points");
        }


        for(int i=0;i<4;i++) {
            pli[i].coul=0;
            pli[i].num=0;
        }
        while (getch()!='\r') {}
    }
    // FIN DES 8 PLIS ******************************************************************************

    // PHASE COMPTAGE POINTS ******************************************************************************
    #ifndef ptsARemporter
    int ptsARemporter;
    unsigned short equipeEnchereGagne;
    int nbCartesRemporteesEquipe;
    #endif

    // Pour chaque carte
    for(unsigned short i=0;i<32;i++) {
        // Pour chaque joueur
        for (unsigned short j=0;j<4;j++) {
            if (ordreJoueur[j]->cartesRemportees[i].num!=0) {
                // Si atout unique
                if(atout>=1 && atout<=4) {
                    if (ordreJoueur[j]->cartesRemportees[i].coul==atout) {
                        ptsPliEquipe[ordreJoueur[j]->equipe]+=valeurCarte(&ordreJoueur[j]->cartesRemportees[i],1);
                    }
                    else {
                        ptsPliEquipe[ordreJoueur[j]->equipe]+=valeurCarte(&ordreJoueur[j]->cartesRemportees[i],2);
                    }
                }
                else if (atout==0) {
                    ptsPliEquipe[ordreJoueur[j]->equipe]+=valeurCarte(&ordreJoueur[j]->cartesRemportees[i],0);
                }
                else if (atout==5) {
                    ptsPliEquipe[ordreJoueur[j]->equipe]+=valeurCarte(&ordreJoueur[j]->cartesRemportees[i],3);
                }
            }
        }
    }

    ptsARemporter=ptsPliEquipe[meilleurEncherisseur->equipe]+valeurContrat;
    if (coinche) {
        ptsARemporter*=2;
    }

    // Compter le nombre de cartes pour l'équipe de l'enchère
    nbCartesRemporteesEquipe=0;
    for(unsigned short i=0;i<4;i++) {
        if(ordreJoueur[i]->equipe==meilleurEncherisseur->equipe) {
            for(unsigned short j=0;j<32;j++) {
                if (ordreJoueur[i]->cartesRemportees[j].num!=0) {
                    nbCartesRemporteesEquipe++;
                }
            }
        }
    }

    // Capot ou générale = si l'équipe remporte les 32 cartes
    if (valeurContrat==250 || valeurContrat==500) {
        // Si contrat rempli
        if (nbCartesRemporteesEquipe==32) {
            // Générale, si l'enchérisseur remporte les 32 cartes
            if (valeurContrat==500) {
                if (meilleurEncherisseur->cartesRemportees[31].num==0) {equipeEnchereGagne=1;}
                else {equipeEnchereGagne=0;}
            }
            // Capot
            else if (valeurContrat==250) {
               {equipeEnchereGagne=1;}
            }
        }
        else {equipeEnchereGagne=0;}
    }
    // Contrat classique
    else {
    // Si contrat rempli
        if (ptsPliEquipe[meilleurEncherisseur->equipe]>=valeurContrat && ptsPliEquipe[meilleurEncherisseur->equipe]>=82) {equipeEnchereGagne=1;}
        // Si contrat non rempli
        else {equipeEnchereGagne=1;}
    }

    if (equipeEnchereGagne) {
        ptsTotalEquipe[meilleurEncherisseur->equipe]=ptsARemporter;
        ptsTotalEquipe[!meilleurEncherisseur->equipe]=ptsPliEquipe[!meilleurEncherisseur->equipe];
    }
    else {
        ptsTotalEquipe[meilleurEncherisseur->equipe]=162+valeurContrat;
        ptsTotalEquipe[!meilleurEncherisseur->equipe]=ptsARemporter;
    }


    effacerConsole();
    //printf("[TEST] pts pli 0 %d, pts pli equipe 1 %d, pts a remporter %d, pts total 0 : %d, pts total 1 :%d\n",ptsPliEquipe[0],ptsPliEquipe[1],ptsARemporter,ptsTotalEquipe[0],ptsTotalEquipe[1]);
    printf("═════════════════════ RESULTATS DE LA PARTIE ══════════════════════\n");
    if (equipeEnchereGagne) {
        printf("\33[92mL'équipe ayant fait l'enchère remporte la partie\33[97m");
        FILE *fichierScore=fopen("sauvegarde.txt","a");
        if(fichierScore!=NULL) {
            //fscanf("%s : %d");
            fprintf(fichierScore,"%s 1 %d\n",sud.pseudo,ptsTotalEquipe[sud.equipe]);
        }
        fclose(fichierScore);
    }
    else {printf("L'équipe défendante remporte la partie.");}
    printf("\nL'équipe Nord/Sud a gagné un total de \33[93m%d\33[97m points.\nL'équipe Est/Ouest remporte \33[93m%d\33[97m points.\n",ptsTotalEquipe[0],ptsTotalEquipe[1]);
    // FIN COMPTAGE POINTS ******************************************************************************

    // Fin de la partie
    printf("\nPartie terminée. Appuyez sur \33[44m[ENTRÉE]\33[40m pour revenir au menu.");
    while (getch()!='\r') {}
}


// Gère la pose d'une carte lors du jeu d'une IA ou appelle [???] pour le joueur réel
void jouerCarte(joueur *j,
                carte (*pli)[4],
                unsigned short atout,
                int numJoueur,
                int valEnchereMax,
                unsigned short numPli) {
    if (j->humain) {
        printf("\33[93mC'est à vous de jouer.\33[97m\n");
        int numCarteSelec=interfaceSelecCarte(j,pli,atout,numPli);
        (*pli)[numJoueur]=j->main[numCarteSelec];
        j->main[numCarteSelec].num=0;
        j->main[numCarteSelec].coul=0;
    }
    else {
        // Décision de l'IA pour la carte à poser dans le pli ****************************************************

        printf("C'est à %s de jouer.\n",j->pseudo);
        short numCarteSelec=-1; // Indice de la carte qui sera jouée dans la main de l'AI

        // Numéro de la dernière carte posée
        unsigned short numDerniereCarte=7;
        if (pli[0]->num!=0) {
            numDerniereCarte=(*pli)[numJoueur-1].num;
        }


        for(int i=3;pli[i]->num!=0;i--) {
            numDerniereCarte=pli[i]->num;
        }

        unsigned short nbCartesParCoul[5];
        for(int c=1;c<=4;c++) {
            for(int i=0;i<8;i++) {
                if (j->main[i].coul==c) {nbCartesParCoul[c]++;}
            }
        }

        unsigned short coulPlusNombreuse=0;
        for (int iCouleur=1;iCouleur<=4;iCouleur++) {
            if (nbCartesParCoul[iCouleur]>coulPlusNombreuse) {
                coulPlusNombreuse=nbCartesParCoul[iCouleur];
            }
        }

        // Si c'est le dernier pli, poser la seule carte qu'il reste
        if(numPli==7){
            numCarteSelec=0;
        }
        // Si ce n'est pas le 8e pli
        else {

            // Si joueur IA est le premier à jouer du pli
            if (pli[0]->num==0) {
                // Si l'atout est une couleur unique
                if (atout>=1 && atout<=4) {
                    // Chercher le valet d'atout
                    for(int i=0;i<8;i++) {
                        // Jouer le valet d'atout si dans la main
                        if(j->main[i].num==11 && j->main[i].coul==atout) {
                            numCarteSelec=i;
                            goto finDecisionPli;
                        }
                        // Sinon, si on a un as non atout et qu'on a max 2 cartes de cette couleur
                        else if (j->main[i].num==14 && j->main[i].coul!=atout && nbCartesParCoul[j->main[i].coul]<=2) {
                            numCarteSelec=i;
                            goto finDecisionPli;
                        }
                    }
                    // Si aucune carte choisie à partir d'ici
                    if (numCarteSelec==-1) {
                        // Jouer la pire carte de la couleur la plus nombreuse
                        numCarteSelec=selectionCarteClassement(j,coulPlusNombreuse,atout,numDerniereCarte,0);
                    }
                }
                // En TA/SA
                else if (atout==5 || atout==0){
                    // Chercher un valet
                    for(int i=0;i<8;i++) {
                        if(j->main[i].num==11) {
                            numCarteSelec=i;
                            goto finDecisionPli;
                        }
                    }
                    // Si rien prendre la pire carte
                    if (numCarteSelec==-1) {
                        numCarteSelec=selectionCarteClassement(j,0,atout,numDerniereCarte,0);
                    }
                }
            }
            // Si ce n'est pas la 1ere carte du pli
            else {
                // Si la partie est avec atout
                if (atout>=1 && atout<=4) {
                    // Si le joueur a des cartes de la couleur demandée
                    if (nbCartesParCoul[(*pli)[0].coul]>0) {
                        // Si le joueur possède une carte forte de la couleur demandée
                        if(possedeCarteForte(j,(*pli)[0].coul,atout)) {
                            // Vérifier si valet d'atout déjà posé
                            unsigned short atoutDejaPose=0;
                            for(short iPli=0;iPli<4;iPli++) {
                                if(pli[iPli]->num==11 && pli[iPli]->coul==atout) {atoutDejaPose=1;}
                            }
                            // Jouer la plus forte de la couleur demandée si la meilleure carte n'est pas posée
                            if (atoutDejaPose) {
                                numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,0);
                            }
                            else {
                                numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,1);
                            }

                        }
                        // Si le joueur ne possède pas de carte forte de la couleur demandée
                        else {
                            // Jouer la plus faible de la couleur demandée
                            numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,0);
                        }
                    }
                    // Si le joueur n'a pas la couleur demandée
                    else {
                        // Si le joueur a des cartes de l'atout
                        if (nbCartesParCoul[atout]>0) {
                            // Si il a une carte forte d'atout
                            if(possedeCarteForte(j,atout,atout)) {
                                numCarteSelec=selectionCarteClassement(j,atout,atout,numDerniereCarte,1);
                            }
                            // Si il n'a pas de carte forte d'atout
                            else {
                                numCarteSelec=selectionCarteClassement(j,atout,atout,numDerniereCarte,0);
                            }
                        }
                        // Si le joueur n'a pas de cartes de l'atout
                        else {
                            numCarteSelec=selectionCarteClassement(j,0,atout,numDerniereCarte,0);
                        }
                    }

                }
                // Si tout atout / sans atout
                else {
                    if(atout==0) {
                        if(nbCartesParCoul[(*pli)[0].coul]>0) {
                            numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,0,1);
                        }
                        else {
                            numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,0,0);
                        }
                    }
                    else {
                       if(nbCartesParCoul[(*pli)[0].coul]>0) {
                            numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,1);
                        }
                        else {
                            numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,0);
                        }
                    }

                }
            }
        }


        finDecisionPli:;

        // 2e passe pour certains cas particuliers non couverts, joue la plus faible dans le doute en respectant les conditions de pose

        if (numCarteSelec<0 || numCarteSelec>7-numPli) {
                //printf("[TEST] passe2\n",numCarteSelec);numCarteSelec=0;
                if (atout>=1 || atout<=4) {
                    if (nbCartesParCoul[(*pli)[0].coul]>0) {
                        numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,0);
                    }
                    else if (nbCartesParCoul[atout]>0) {
                        numCarteSelec=selectionCarteClassement(j,atout,atout,numDerniereCarte,0);
                    }
                    else {
                        numCarteSelec=selectionCarteClassement(j,0,atout,numDerniereCarte,0);
                    }
                }
                else if (atout==0) {
                   if (nbCartesParCoul[(*pli)[0].coul]>0) {
                        numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,0,0);
                    }
                    else {
                        numCarteSelec=selectionCarteClassement(j,0,atout,0,0);
                    }
                }
                else {
                   if (nbCartesParCoul[(*pli)[0].coul]>0) {
                        numCarteSelec=selectionCarteClassement(j,(*pli)[0].coul,atout,numDerniereCarte,0);
                    }
                    else {
                        numCarteSelec=selectionCarteClassement(j,0,atout,numDerniereCarte,0);
                    }
                }
        }

        if (numCarteSelec<0 || numCarteSelec>7-numPli) {
            //printf("[TEST] Impossible de choisir\n");
            numCarteSelec=0;
        }
        printf("%s joue ",j->pseudo);
        afficherCarte(&j->main[numCarteSelec]);
        //printf(" (numéro %d)",numCarteSelec);
        printf(".\n");
        // Copier la carte dans le pli et effacer celle de la main
        (*pli)[numJoueur]=j->main[numCarteSelec];
        j->main[numCarteSelec].num=0;
        j->main[numCarteSelec].coul=0;
    }
}

// Sélectionner la carte la plus forte/faible de la main du joueur j en prenant en compte les règles
int selectionCarteClassement(joueur *j,
                             unsigned short coulSelec, // Couleur à obligatoirement choisir : 0=pas d'obligation 1=♠ 2=♣ 3=♥ 4=♦
                             unsigned short coulAtout, // Couleur de l'atout
                             unsigned short valeurSeuil, // Numéro dernière carte posée
                             unsigned short sens // 0: la plus faible, 1: la plus forte
                             ) {
    //printf("[TEST] Demandé:%d, seuil=%d, sens=%d\n",coulSelec,valeurSeuil,sens);

    /*
    short classementParTypeAtout[4][8] = {
        {1,2,3,7,4,5,6,8},
        {1,2,7,5,8,3,4,6},
        {1,2,3,7,4,5,6,8},
        {1,2,7,5,7,3,4,6}
    };*/

    //unsigned short typeAtout;



    unsigned short indiceCarteRetenue=100; // un indice de 100 retourné iniquera qu'aucune carte n'a été trouvée
    short classementCarteRetenue=sens*100;


    /* ANCIEN ALGORITHME DE SELECTION
    for(int i=0;i<8;i++) {
        // Déterminer type d'atout
        if (coulAtout==0) {
            typeAtout=coulAtout;
        }
        else if (coulAtout==5) {
            typeAtout=3;
        }
        else if (j->main[i].coul==coulAtout) {
            typeAtout=1;
        }
        else {
            typeAtout=2;
        }

        // Vérifier si ce n'est pas une carte vide
        if(j->main[i].num>=7) {
            // Si il est possible de prendre n'importe quelle couleur
            if (coulSelec==0) {
                // Prendre la carte la plus forte (sens positif)
                if (sens) {
                    if(classementParTypeAtout[typeAtout][j->main[i].num]>classementCarteRetenue) {
                            classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                            indiceCarteRetenue=i;
                    }
                }
                // Prendre la carte la plus faible (sens négatif)
                else {
                    if(classementParTypeAtout[typeAtout][j->main[i].num]<classementCarteRetenue) {
                        classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                        indiceCarteRetenue=i;
                    }
                }
            }
            // Si il faut prendre une carte d'une couleur précise
            else {
                // Meilleure carte pour 1 couleur précise
                if (sens) {
                    if(classementParTypeAtout[typeAtout][j->main[i].num]>classementCarteRetenue && j->main[i].coul==coulSelec) {
                        if (coulSelec==coulAtout) {
                            // Vérifier que la carte dépasse bien la valeur seuil
                             if (classementParTypeAtout[typeAtout][j->main[i].num]>classementParTypeAtout[typeAtout][valeurSeuil]) {
                                classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                                indiceCarteRetenue=i;
                             }
                        }
                        else {
                            classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                            indiceCarteRetenue=i;
                        }
                    }
                }
                // Pire carte pour 1 couleur précise
                else {
                    if(classementParTypeAtout[typeAtout][j->main[i].num]<classementCarteRetenue && j->main[i].coul==coulSelec) {
                        if (coulSelec==coulAtout) {
                             if (classementParTypeAtout[typeAtout][j->main[i].num]>classementParTypeAtout[typeAtout][valeurSeuil]) {
                                classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                                indiceCarteRetenue=i;
                             }
                        }
                        else {
                            classementCarteRetenue=classementParTypeAtout[typeAtout][j->main[i].num];
                            indiceCarteRetenue=i;
                        }
                    }
                }
            }
        }
    }
    */


    unsigned short typeAtoutCalculPts;

    for(int i=0;i<8;i++) {
        if (coulAtout==0) {typeAtoutCalculPts=0;}
        else if (coulAtout==5) {typeAtoutCalculPts=3;}
        else {typeAtoutCalculPts=2;}


        // Sans obligation de couleur
        if (coulSelec==0) {
            if (sens) {
                // Incrémenter si plus fort
                if (classementParAtout(j->main[i].num,typeAtoutCalculPts)>classementCarteRetenue) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,typeAtoutCalculPts);
                    indiceCarteRetenue=i;
                }
            }
            else {
                // Incrémenter si plus faible
                if (classementParAtout(j->main[i].num,typeAtoutCalculPts)<classementCarteRetenue) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,typeAtoutCalculPts);
                    indiceCarteRetenue=i;
                }
            }

        }
        // Couleur demandée est atout
        else if (coulSelec==coulAtout) {
            if (sens) {
                // Incrémenter si plus fort en tant qu'atout et de la bonne couleur
                if (classementParAtout(j->main[i].num,1)>classementCarteRetenue && j->main[i].coul==coulAtout && classementParAtout(j->main[i].num,1)>classementParAtout(valeurSeuil,1)) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,1);
                    indiceCarteRetenue=i;
                }
            }
            else {
                // Incrémenter si plus faible en tant qu'atout et de la bonne couleur
                if (classementParAtout(j->main[i].num,1)<classementCarteRetenue && j->main[i].coul==coulAtout && classementParAtout(j->main[i].num,1)>classementParAtout(valeurSeuil,1)) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,1);
                    indiceCarteRetenue=i;
                }
            }

        }
        // Obligation d'une couleur
        else {
            if (sens) {
                // Incrémenter si plus fort en tant que non atout et de la bonne couleur
                if (classementParAtout(j->main[i].num,2)>classementCarteRetenue && j->main[i].coul==coulSelec) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,2);
                    indiceCarteRetenue=i;
                }
            }
            else {
                // Incrémenter si plus faible en tant que non atout et de la bonne couleur
                if (classementParAtout(j->main[i].num,2)<classementCarteRetenue && j->main[i].coul==coulSelec) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,2);
                    indiceCarteRetenue=i;
                }
            }
        }
    }
    // Fin boucle for

    // 2e passe si la carte n'a pas été trouvée (exemple: carte forte demandée avec carte la plus forte posée)
    // Prendre la plus faible
    if(indiceCarteRetenue==100) {
        for(int i=0;i<8;i++) {
            if (coulSelec==coulAtout) {
                // Incrémenter si plus faible en tant qu'atout et de la bonne couleur
                if (classementParAtout(j->main[i].num,1)<classementCarteRetenue && j->main[i].coul==coulAtout) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,1);
                    indiceCarteRetenue=i;
                }
            }
            else {
               if (classementParAtout(j->main[i].num,2)<classementCarteRetenue &&j->main[i].coul==coulSelec) {
                    classementCarteRetenue=classementParAtout(j->main[i].num,2);
                    indiceCarteRetenue=i;
                }
            }
        }
    }

    if(indiceCarteRetenue==100) {
        for(int i=0;i<8;i++) {
            if (classementParAtout(j->main[i].num,2)<classementCarteRetenue) {
                classementCarteRetenue=classementParAtout(j->main[i].num,2);
                indiceCarteRetenue=i;
            }
        }
    }

    return indiceCarteRetenue;
}

// Renvoie 0 si le joueur j possède une carte parmi les plus fortes d'une couleur
unsigned short possedeCarteForte(joueur *j,unsigned short coul, unsigned short atout) {
    const unsigned int seuilParAtout[6] = {4,11,4,6};
    for(int i=0;i<8;i++) {
        if (j->main[i].coul==coul && j->main[i].num>=seuilParAtout[atout]) {
            return 1;
        }
    }
    return 0;
}


// Gère l'enchère d'une IA ou appelle interfaceEnchere pour le joueur réel
// Toutes les variables dont les IA ont besoin sont passées à la fonction
void encherir(joueur *encherisseur,
              unsigned short *atout,
              int *valeurContrat,
              unsigned short *coinche,
              unsigned short *surcoinche) {

    if (encherisseur->humain) {
        printf("\33[93mC'est à vous d'enchérir !\33[97m");
        interfaceEnchere(encherisseur, atout, valeurContrat, coinche, surcoinche);
    }
    else {

        char nomAtoutSelect[32];
        // Calcul de la valeur des cartes de la main en fonction de la couleur d'atout
        int ptsParAtout[6] = {0};

        // Pour un jeu avec 1 atout
        for(int cAtout=1;cAtout<=4;cAtout++) {
            for (int i=0;i<8;i++) {
                if (encherisseur->main[i].coul==cAtout) { // Si la carte est atout
                    ptsParAtout[cAtout]+=valeurCarte(&encherisseur->main[i],1);
                }
                else {
                    ptsParAtout[cAtout]+=valeurCarte(&encherisseur->main[i],2);
                }
            }
        }

        // Pour un jeu sans atout et tout atout

        for (int i=0;i<8;i++) {
            ptsParAtout[0]+=valeurCarte(&encherisseur->main[i],0);
            ptsParAtout[5]+=valeurCarte(&encherisseur->main[i],3);
        }

        // Mise à -1 des points pour l'atout demandé par le dernier joueur ayant fait une enchère
        // Evite à cette couleur d'être sélectionnée par l'IA
        if (*valeurContrat!=0) {ptsParAtout[*atout]=-1;}

        // Ne pas choisir une couleur si la main ne contient qu'une seule carte de cette couleur
        int nbCartesParCoul[5]={0};
        for(int i=0;i<8;i++) {
            nbCartesParCoul[encherisseur->main[i].coul]++;
        }
        for(int i=1;i<=4;i++) {
            if(nbCartesParCoul[i]<=1) {ptsParAtout[i]=0;}
        }

        /*
        // Calcul du nombre de cartes par couleur
        // La carte la plus forte compte comme 2 pour éviter qu'un jeu contenant seulement une carte forte pour une couleur soit dévalorisée
        short nbCartesParCoulAtoutUnique[5]={0};
        for (int i=0;i<8;i++) {
            if(encherisseur->main[i].num)
        }

        */



        // Recherche de l'atout donnant un nombre de points max
        unsigned short cAtoutSelec=0;
        int ptsMaxSelec=ptsParAtout[0];

        for(int cAtout=1;cAtout<=5;cAtout++) {
            if ( (ptsParAtout[cAtout]>ptsMaxSelec) || (ptsParAtout[cAtout]==ptsMaxSelec && (cAtout<5)) )  {
                ptsMaxSelec=ptsParAtout[cAtout];
                cAtoutSelec=cAtout;
            }
        }

        nomAtoutCpy(&nomAtoutSelect,cAtoutSelec);

        // Décision de passe ou d'enchère

        if (49<=ptsMaxSelec && ptsMaxSelec<55 && *valeurContrat<80 && IA_SAIT_ENCHERIR) {
            *atout=cAtoutSelec;
            *valeurContrat=80;
            printf("Le joueur %s annonce une enchère de %d en %s.\n",encherisseur->pseudo,*valeurContrat,nomAtoutSelect);
        }
        else if (55<=ptsMaxSelec && ptsMaxSelec<65 && *valeurContrat<90 && IA_SAIT_ENCHERIR) {
            *atout=cAtoutSelec;
            *valeurContrat=90;
            printf("Le joueur %s annonce une enchère de %d en %s.\n",encherisseur->pseudo,*valeurContrat,nomAtoutSelect);
        }
        else if (65<=ptsMaxSelec && ptsMaxSelec<75 && *valeurContrat<100 && IA_SAIT_ENCHERIR) {
            *atout=cAtoutSelec;
            *valeurContrat=100;
            printf("Le joueur %s annonce une enchère de %d en %s.\n",encherisseur->pseudo,*valeurContrat,nomAtoutSelect);
        }
        else if (75<=ptsMaxSelec && ptsMaxSelec<100 && *valeurContrat<120 && IA_SAIT_ENCHERIR) {
            *atout=cAtoutSelec;
            *valeurContrat=120;
            printf("Le joueur %s annonce une enchère de %d en %s.\n",encherisseur->pseudo,*valeurContrat,nomAtoutSelect);
        }
        else if (100<=ptsMaxSelec && *valeurContrat<140 && IA_SAIT_ENCHERIR) {
            *atout=cAtoutSelec;
            *valeurContrat=140;
            printf("Le joueur %s annonce une enchère de %d en %s.\n",encherisseur->pseudo,*valeurContrat,nomAtoutSelect);
        }
        else {
            printf("Le joueur %s passe son tour.\n",encherisseur->pseudo);
        }
    }
}

// Fonction renvoyant la valeur d'une carte c en points
// typeAtout : 0 = SA, 1=A, 2=NA, 3=TA
// Les valeurs sont stockées dans un tableau en 2D
unsigned int valeurCarte(carte *c,unsigned short typeAtout) {
    const unsigned int tabVal[4][8]= {
        {0,0,0,10,2,3,4,19},
        {0,0,14,10,20,3,4,11},
        {0,0,0,10,2,3,4,11},
        {0,0,9,5,14,1,3,6}
    };
    return tabVal[typeAtout][c->num-7];
}

// Pareil que valeurCarte mais pour le classement des cartes
// (deux cartes peuvent avoir la même valeur mais aucune n'est à la même place dans le classement de la force des cartes)
short classementParAtout(unsigned short num,unsigned short typeAtout) {
    const short tabClassement[4][8] = {
        {1,2,3,7,4,5,6,8},
        {1,2,7,5,8,3,4,6},
        {1,2,3,7,4,5,6,8},
        {1,2,7,5,8,3,4,6}
    };
    return tabClassement[typeAtout][num-7];
}
