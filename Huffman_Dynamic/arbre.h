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

void afficheNod (noeud nod);
void afficheArbre (noeud* n, int len);
noeud* setArbre ();
void createDotNod (int desc, noeud* arbre, int index);
void createDotFile (int desc, noeud* arbre, int len);
void swap (int index1, int index2, noeud* arbre);
int plusCroissant (noeud* arbre, int len);
int rechercheEquilibre (noeud* arbre, int index, int len);
void reequilibre (noeud* arbre, int index, int len, int cas);
void incrementChar(noeud* arbre, int len, int indexDeC);
void deplacement (noeud* arbre, int nbNod, int k);
int searchChar(noeud* arbre, char c, int len);
noeud* addCharInTree (noeud **arbre, char c, int *nbNod);
int contain (char *alreadyRead, char c, int nbChar);
char* addCharInAlreadyRead(char ** alreadyRead, char c, int* nbChar);