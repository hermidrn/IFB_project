#ifndef COMMUN_H_INCLUDED
#define COMMUN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pour la fonction getch
#ifdef _WIN32
#include <conio.h>
#endif

void effacerConsole();
void couleur(int couleur);
void espaces(int n);

#endif
