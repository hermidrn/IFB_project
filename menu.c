#include "menu.h"

void menu() {
    affichMenuSelection(0);
    unsigned short nbElementsMenu=3;
    unsigned short numMenu;
    // Détection des touches directionnelles du clavier
    short demandeQuitter=0;
    while (demandeQuitter==0) {
        switch(getch()) {
            case 80: // Touche flèche du bas
                if (numMenu<nbElementsMenu-1) {numMenu++;}
                break;
            case 72: // Touche flèche du haut
                if (numMenu>0) {numMenu--;}
                break;
            case 13: // Touche entrée
                // Si effectuerAction ne renvoie pas 0, le jeu sort du menu et quitte
                demandeQuitter=effectuerAction(numMenu);
                break;
        }
    effacerConsole();
    affichMenuSelection(numMenu);
    }
}

// Fonction qui affiche le menu avec des indicateurs pour voir quelle option est sélectionnée
void affichMenuSelection(int i) {
    char tableauChaineMenu[7][512] = {
        "╔════════════════ BELOTE COINCHÉE • MENU PRINCIPAL ════════════════╗\n║ Utilisez les flèches haut/bas du clavier pour se déplacer        ║\n║ Appuyez sur [ENTRÉE] pour sélectionner une option                ║\n╟──────────────────────────────────────────────────────────────────╢\n║",
        " ",
        "Démarrer une nouvelle partie\33[97m\33[40m                                     ║\n║",
        " ",
        "Afficher les meilleurs scores\33[97m\33[40m                                    ║\n║",
        " ",
        "Quitter\33[97m\33[40m                                                          ║\n╚══════════════════════════════════════════════════════════════════╝"
    };

    // Affiche un symbole flèche et surligne l'option sélectionée
    strcpy(tableauChaineMenu[i*2+1],"\33[30m\33[107m►");

    for(int i=0;i<=6;i++) {
        printf("%s",tableauChaineMenu[i]);
    }
}

// Effectue l'action sélectionnée dans le menu (déclenchée par la touche entrée)
unsigned short effectuerAction(int i) {
    effacerConsole();
    switch(i) {
        case 0:
            jeu();
            return 0;
        case 1:
            interfaceScore();
            return 0;
        case 2:
            return 1;
        default:
            return 0;
    }
}
