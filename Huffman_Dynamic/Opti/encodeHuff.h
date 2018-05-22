#include "arbre.h"

void writeStringOfBit (FILE *fout, char * chain);
void writeAChar (FILE *fout, char c);
void writeHeader (FILE *fout);
void writeACharWithPredec (FILE *fout, char c, int fin);
int getLen (noeud* arbre);
char* reverseString (char* str);