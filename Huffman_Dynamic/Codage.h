#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void writeStringOfBit (int descout, char * chain);
void writeAChar (int descout, char c);
void writeHeader (int descout);
void writeACharWithPredec (int descout, char c, int fin);
int getLen (noeud* arbre);
char* getCodeFromiToRoot (noeud* arbre, int i);
char* getCodeFrom0ToRoot (noeud* arbre);
char* reverseString (char* str);