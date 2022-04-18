#ifndef STRUCTURESJEU_H_INCLUDED
#define STRUCTURESJEU_H_INCLUDED

// Header contenant les structures nécéssaires pour le jeu

typedef struct carte {
    unsigned short num; // 7, 8, 9, 10, 11 (V), 12 (D), 13 (R), 14 (As)
    unsigned short coul; // 1=♠, 2=♣, 3=♥, 4=♦
} carte;

typedef struct joueur {
    char pseudo[128]; // Nom personnalisé pour le joueur
    unsigned short humain; // 0=intelligence artificielle, 1=actions contrôlées par un vrai joueur
    unsigned short equipe; // Numéro de l'équipe : 0=NS, 1=EO
    carte main[8]; // Tableau de 8 cartes représentant les cartes possédées par un joueur
    carte cartesRemportees[32]; // Cartes remportées par le joueur pour le comptage de points à la fin
} joueur;

#endif
