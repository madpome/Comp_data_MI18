#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



typedef struct noeud {
	// lettre code, -1 si vide
	char lettre;
	// dfg = id noeud fils gauche - id noeud courant, 0 si non existant
	int dfg;
	// dfd = id noeud fils droit - id noeud courant, 0 si non existant
	int dfd;
	// dp = id noeud pere - id noeud courant, 0 si non existant
	int dp;
	// poids du noeud
	int poids;

} noeud;


typedef struct indexCode {
	// index = index dans le tableau arbre;
	int index;
	// code = code de arbre[index] a la racine
	char *code;
	// valid = 1 si on a bouge le noeud, 0 sinon
	int valid;

} indexCode;



void afficheNod (noeud nod);
void afficheArbre (noeud* n, int len);
noeud* setArbre ();
void createDotNod (int desc, noeud* arbre, int index);
void createDotFile (int desc, noeud* arbre, int len);
void swap (int index1, int index2, noeud* arbre, indexCode *tableau);
int plusCroissant (noeud* arbre, int len);
void spreadNotValid(noeud *arbre, int index, indexCode *tableau);
int rechercheEquilibre (noeud* arbre, int index, int len);
int rechercheDicho (noeud *arbre, int poids, int d, int f, int index);
void reequilibre (noeud* arbre, int index, int len, indexCode *tableau);
void incrementChar(noeud* arbre, int len, int indexDeC, indexCode *tab);
void deplacement (noeud* arbre, int nbNod, int k);
int searchChar(noeud* arbre, char c, int len);
noeud* addCharInTree (noeud **arbre, char c, int *nbNod, indexCode *tableau);
char* addCharInAlreadyRead(char ** alreadyRead, char c, int* nbChar);
int searchChar(noeud* arbre, char c, int len);
char* getCodeFromiToRoot (noeud* arbre, int i, int len);
char* getCodeFrom0ToRoot (noeud* arbre, int len);