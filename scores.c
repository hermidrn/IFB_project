#include "scores.h"

void interfaceScore() {
    printf("═════════════════════════ MEILLEURS SCORES ═════════════════════════\n");
    FILE *fichierScores = fopen("sauvegarde.txt","r");
    if(fichierScores!=NULL) {
        char nom[256];
        int victoires, record;

        do {
            fscanf(fichierScores,"%s %d %d\n",&nom,&victoires,&record);
            printf("%s a gagne %d parties avec un record de %d.\n",nom,victoires,record);
        } while (!(feof(fichierScores)));
    }
    fclose(fichierScores);
    printf("\nAppuyez sur \33[44m[ECHAP]\33[40m pour sortir de ce menu");
    while(getch()!=27){} // Ne rien faire tant que la touche appyuée n'est pas échap
}
