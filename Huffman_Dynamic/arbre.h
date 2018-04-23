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

noeud* setArbre ();
void incrementePoidsPereCascade (noeud* arbre, int index, int len);
noeud* addCharInTree (noeud* arbre, char c, int* nbNod);
int contain (char *alreadyRead, char c, int nbChar);
char* addCharInAlreadyRead(char * alreadyRead, char c, int* nbChar);
void incrementChar(noeud* arbre, char c, int len);
int searchChar(noeud* arbre, char c, int len);
void reequilibre (noeud* arbre, int index, int len);
void swap (int index1, int index2, noeud* arbre);
void createDotNod (int desc, noeud* arbre, int index);
void createDotFile (int desc, noeud* arbre, int len);
void deplacement (noeud* arbre, int nbNod, int k);
void afficheNod (noeud nod);
