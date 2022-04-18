#include "commun.h"
#include "menu.h"


int main();

// Commande pour changer la couleur du prochain texte qui sera écrit
void couleur(int couleur) {
    printf("\33[%dm",couleur);
}

// Affiche n espaces (utile pour l'interface)
void espaces(int n) {
    for(int i=0;i<n;i++) {
        printf(" ");
    }
}

// Commande pour effacer le texte de la console
void effacerConsole() {
    // Pour Windows
    #if defined _WIN32
    system("cls");
    #endif

    // Pour Linux ou macOS
    #if defined __linux__ || __APPLE__
    system("clear");
    #endif
}

int main()
{
    #if defined _WIN32
    system("@chcp 65001 > nul"); // On passe la console en UTF-8 au lieu du jeu de caractères ISO 850 pour avoir plus de carcactères spéciaux
    system("@mode con cols=68 lines=36 > nul"); // On met aussi un nombre de lignes et colonnes définies pour la console pour s'assurer que la taille de la fenêtre ne soit pas trop petite
    // Pour éviter que la commande affiche du texte dans notre programme on redirige la sortie dans le vide
    system("@title Belote coinchee");
    #endif

    // Affichage du menu
    couleur(97);
    couleur(40);
    effacerConsole();
    menu();
    effacerConsole();
    printf("Merci d'avoir joué !");
    return EXIT_SUCCESS;
}
