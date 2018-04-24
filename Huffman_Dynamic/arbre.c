#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "arbre.h"



void afficheNod (noeud nod) {
	printf("lettre = %d, dfg = %d, dfd = %d, dp = %d, poids = %d, pointeur = %p\n",(int)nod.lettre, nod.dfg, nod.dfd, nod.dp, nod.poids, &nod);
}

void afficheArbre (noeud* n, int len) {
	for (int i = 0; i<len; i++) {
		printf("%d : ", i+1);
		afficheNod(n[i]);
	}
}

noeud* setArbre () {
	noeud* root = calloc (1, sizeof (noeud));
	root->lettre = -1;
	root->dfg = 0;
	root->dfd = 0;
	root->dp = 0;
	root->poids = 0;

	return root;
}


void createDotNod (int desc, noeud* arbre, int index) {
	char * ligne = calloc (1024, sizeof(char));
	char * ligneFils = calloc (1024, sizeof(char));
	char * characn = calloc(10, sizeof(char));
	char * characf = calloc(10, sizeof(char));
	int ig = index+arbre[index].dfg;
	int id = index+arbre[index].dfd;

	if (ig != index) {
		if (arbre[index].lettre == '"') {
			characn = strcat (characn, "\\\"");
		} else if (arbre[index].lettre == 0) {
			characn = strcat (characn, "-1");
		} else if (arbre[index].lettre != -1) {
			sprintf(characn, "%c", arbre[index].lettre);
		} else {
			characn = strcat(characn, "");
		}

		sprintf(ligne, "\"%d,%s,%d,%d\" -> ", index, characn, arbre[index].poids, index+arbre[index].dp);
		write(desc, ligne, strlen(ligne)*sizeof(char));
		
		if (arbre[ig].lettre == '"') {
			characf = strcat (characf, "\\\""); 
		} else if (arbre[ig].lettre == 0) {
			characf = strcat (characf, "-1");
		} else if (arbre[ig].lettre != -1) {
			sprintf(characf, "%c", arbre[ig].lettre);
		} else {
			characf = strcat(characf, "");
		}

		sprintf(ligneFils, "\"%d,%s,%d,%d\" [label = \"0\"];\n", ig, characf, arbre[ig].poids, ig+arbre[ig].dp);
		write(desc, ligneFils, strlen(ligneFils)*sizeof(char));	


		write(desc, ligne, strlen(ligne)*sizeof(char));
		memset(characf, '\0', 10*sizeof(char));
		
		if (arbre[id].lettre == '"') {
			characf = strcat (characf, "\\\"");
		} else if (arbre[id].lettre == 0) {
			characf = strcat (characf, "-1");
		} else if (arbre[id].lettre != -1) {
			sprintf(characf, "%c", arbre[id].lettre);
		} else {
			characf = strcat(characf, "");
		}
		sprintf(ligneFils, "\"%d,%s,%d,%d\" [label = \"1\"];\n", id, characf, arbre[id].poids, id+arbre[id].dp);
		write(desc, ligneFils, strlen(ligneFils)*sizeof(char));	

		//free(characn);
		//free(characf);
		//free(ligne);
		//free(ligneFils);

		createDotNod (desc, arbre, ig);
		createDotNod (desc, arbre, id);
	}
}


void createDotFile (int desc, noeud* arbre, int len) {
	char* ligne = calloc (1024, sizeof(char));
	sprintf(ligne, "digraph G {\n");
	write (desc, ligne, strlen(ligne));
	createDotNod(desc, arbre, len-1);


	sprintf(ligne, "}\n");
	write (desc, ligne, strlen(ligne));
	//free(ligne);
}

void swap (int index1, int index2, noeud* arbre) {

	noeud* noeud1 = &(arbre[index1]);
	noeud* noeud2 = &(arbre[index2]);
	noeud tmp = {noeud1->lettre, noeud1->dfg, noeud1->dfd,noeud1->dp,noeud1->poids};
	


	int n1fg = noeud1->dfg + index1;
	int n1fd = noeud1->dfd + index1;
	int n2fg = noeud2->dfg + index2;
	int n2fd = noeud2->dfd + index2;

	// On met noeud1 dans noeud2
	noeud1->lettre = noeud2->lettre;
	if (noeud2->dfg != 0) {
		// On est pas dans une feuille
		noeud1->dfg = (index2 - index1 + noeud2->dfg);
		noeud1->dfd = (index2 - index1 + noeud2->dfd);
	} else {
		noeud1->dfg = 0;
		noeud1->dfd = 0;
	}
	if (n1fg != index1) {
		arbre[n1fg].dp = index2 - n1fg;
		arbre[n1fd].dp = index2 - n1fd;
		//printf("Pas feuille 1\n");

	} else {
		// noeud1 est une feuille
		/*printf("************************* 88 LOL 88 ******************* \n");

		afficheNod(arbre[n1fg]);
		afficheNod(arbre[n1fd]);
		printf("\n\n");
		*/
	}
	noeud1->poids = noeud2->poids;


	// On met tmp dans noeud2
	noeud2->lettre = tmp.lettre;
	if (tmp.dfg != 0) {
		// noeud1 n'est pas une feuille
		noeud2->dfg = index1 - index2 + tmp.dfg;
		noeud2->dfd = index1 - index2 + tmp.dfd;
	} else {
		// noeud1 est une feuille
		noeud2->dfg = 0;
		noeud2->dfd = 0;
	}
	if (n2fg != index2) {
		//noeud2 n'est pas une feuille
		arbre[n2fg].dp = index1 - n2fg;
		arbre[n2fd].dp = index1 - n2fd;
		//printf("Pas feuille 2\n");
	} else {
		// noeud2 est une feuille
		/*
		printf("$$$$$$$$$$$$$$$$$$ 99 LOL 99 $$$$$$$$$$$$$$$$$$$$$$ \n");
		afficheNod(arbre[n2fg]);
		afficheNod(arbre[n2fd]);
		printf("\n\n");
		*/
	}
	noeud2->poids = tmp.poids;
}

int plusCroissant (noeud* arbre, int len) {
	for (int i = 1; i<len; i++) {
		if (arbre[i-1].poids > arbre[i].poids)
			return 1;
	}
	return -1;
}

int rechercheEquilibre (noeud* arbre, int index, int len) {
	// On cherche le plus grand index de poids inferieur a celui de arbre[index]
	for (int i = len-1; i>index; i--) {
		if (arbre[i].poids == arbre[index].poids) {
			return i;
		}
	}
	return -1;
}


void reequilibre (noeud* arbre, int index, int len, int cas) {
	while (arbre[index].dp != 0) {
		int indexPereSame = rechercheEquilibre(arbre, index, len);
		if (indexPereSame > 0) {
			swap(indexPereSame, index, arbre);
			int tmp = index;
			index = indexPereSame;
			indexPereSame = tmp;
		}
		arbre[index].poids++;
		index = arbre[index].dp + index;

	}
	arbre[index].poids++;

}



void incrementChar(noeud* arbre, int len, int indexDeC) {
	arbre[indexDeC].poids++;
	reequilibre(arbre, indexDeC, len, 2);
}


void deplacement (noeud* arbre, int nbNod, int k) {
	// Enfaite c'est un memmove, sauf que la fonction memmove ne fonctionnait pas
	// deplace les cases de k
	// On suppose que la memoire est bien allouee
	for (int i = nbNod-1; i>=0; i--) {
		arbre[i+k] = arbre[i];
	}

}

noeud* addCharInTree (noeud** arbre, char c, int *nbNod) {

	*arbre = realloc (*arbre, ((*nbNod)+2) * sizeof(noeud));
	/*
	if (tempArbre == NULL) {
		printf("On a un realloc NULL");
	}
	*arbre = &tempArbre;
	*/
	deplacement(*arbre, *nbNod, 2);
	(*nbNod)+=2;


	//Initialisation de (*arbre)[0]
	(*arbre)[0].lettre = -1;
	(*arbre)[0].dfg = 0;
	(*arbre)[0].dfd = 0;
	(*arbre)[0].dp = 2;
	(*arbre)[0].poids = 0;

	//Initialisation de (*arbre)[1] (C'est la qu'on va foutre le caractere)
	(*arbre)[1].lettre = c;
	(*arbre)[1].dfg = 0;
	(*arbre)[1].dfd = 0;
	(*arbre)[1].dp = 1;
	(*arbre)[1].poids = 1;


	//On modifie les valeurs de fils de (*arbre)[2] (l'ancien (*arbre)[0])
	(*arbre)[2].dfg = -2;
	(*arbre)[2].dfd = -1;
	//(*arbre)[2].poids++;
	//afficheArbre(*arbre, *nbNod);


	return *arbre;

}

int searchChar(noeud* arbre, char c, int len) {
	// On fait un simple parcours de tableau, on pourrait parcourir en tant qu'arbre,
	// Mais on pourrait rechercher dans tout l'arbre a la fin.
	for (int i = len-1; i >= 0; i--) {
		if (arbre[i].lettre == c && arbre[i].dfg == 0) {
			//On verifie que les caracteres sont identiques, et qu'on est bien dans une feuille
			return i;
		}
	}
	// On ne peut pas acceder ici
	return -1;
}

int contain (char *alreadyRead, char c, int len) {
	for (int i = 0; i<len; i++) {
		if (alreadyRead[i] == c) {
			return 1;
		}
	}
	return -1;

}

char* addCharInAlreadyRead(char ** alreadyRead, char c, int* len) {
	if (*len != 0) {
		*alreadyRead = realloc (*alreadyRead, sizeof(char)*((*len)+1));
	}
	(*alreadyRead)[*len] = c;
	(*len)++;
	return *alreadyRead;
}
