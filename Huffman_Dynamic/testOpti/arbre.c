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

void swap (int index1, int index2, noeud* arbre, indexCode *tableau) {
	noeud* noeud1 = &(arbre[index1]);
	noeud* noeud2 = &(arbre[index2]);
	noeud tmp = {noeud1->lettre, noeud1->dfg, noeud1->dfd,noeud1->dp,noeud1->poids};



	int n1fg = noeud1->dfg + index1;
	int n1fd = noeud1->dfd + index1;
	int n2fg = noeud2->dfg + index2;
	int n2fd = noeud2->dfd + index2;
	char c1 = noeud1->lettre;
	char c2 = noeud2->lettre;

	// On met noeud1 dans noeud2
	noeud1->lettre = noeud2->lettre;

	if (noeud2->dfg != 0) {
		// On est pas dans une feuille
		noeud1->dfg = (index2 - index1 + noeud2->dfg);
		noeud1->dfd = (index2 - index1 + noeud2->dfd);

	} else {
		//
		tableau[c2+128].index = index1;
		tableau[c2+128].valid = 1;


		noeud1->dfg = 0;
		noeud1->dfd = 0;
	}
	if (n1fg != index1) {
		arbre[n1fg].dp = index2 - n1fg;
		arbre[n1fd].dp = index2 - n1fd;
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
		tableau[c1+128].index = index2;
		tableau[c2+128].valid = 1;


		noeud2->dfg = 0;
		noeud2->dfd = 0;
	}
	if (n2fg != index2) {
		//noeud2 n'est pas une feuille
		arbre[n2fg].dp = index1 - n2fg;
		arbre[n2fd].dp = index1 - n2fd;
		//printf("Pas feuille 2\n");
	}
	noeud2->poids = tmp.poids;
	spreadNotValid(arbre, index2, tableau);
	spreadNotValid(arbre, index1, tableau);
}

void spreadNotValid(noeud *arbre, int index, indexCode *tableau) {
	if (arbre[index].dfg != 0) {
		// On n'est pas dans une feuille, on va spread un 1 dans tout les valid des fils de index dans tout les fils de index, dans le tableau
		int dfg = arbre[index].dfg;
		int dfd = arbre[index].dfd;
		spreadNotValid (arbre, dfg + index, tableau);
		spreadNotValid (arbre, dfd + index, tableau);
	} else {
		// On est dans une feuille
		// Si index == 0, on est dans la feuille 0, donc on ne fait rien,
		// Sinon, on change
		if (index != 0) {
			tableau[arbre[index].lettre + 128].valid = 1;
		}
	}
}

int rechercheEquilibre (noeud* arbre, int index, int len) {
	// On cherche le plus grand index de poids inferieur egale a celui de arbre[index]

	for (int i = len-1; i>index; i--) {
		// i != arbre[index].dp + index <=> i n'est pas le pere de index
		if (arbre[i].poids <= arbre[index].poids && i != arbre[index].dp + index) {
			return i;
		}
	}
	return -1;

	// Contre-intuitivement, la version d'en haut (en recherche lineaire) est plus rapide que la recherche dicho (en dessous)
	/*
	if (index == len-1) {
		return -1;
	} else {
		return rechercheDicho (arbre, arbre[index].poids, index, len-1, index);
	}
	*/

}
/*
int rechercheDicho (noeud *arbre, int poids, int debut, int fin, int index) {
	int imilieu = (debut+fin)/2;
	noeud milieu = arbre[imilieu];
	if(milieu.poids <= poids && arbre[imilieu+1].poids>poids && ((imilieu) != (arbre[index].dp + index))){
		return imilieu;
	}else if(milieu.poids > poids){
		return rechercheDicho(arbre, poids, debut, imilieu, index);
	}else{
		return rechercheDicho(arbre, poids, imilieu, fin, index);
	}
}
*/
//int indexPereSame = rechercheDicho(arbre, arbre[index].poids, index, len-1, index);

void reequilibre (noeud* arbre, int index, int len, indexCode *tableau) {
	// Le flag > 0 indique qu'il y a eu un swap
	int flag = 0;
	while (arbre[index].dp != 0) {
		int indexPereSame = rechercheEquilibre(arbre, index, len);
		if (indexPereSame > 0) {
			flag++;
			swap(indexPereSame, index, arbre, tableau);
			int tmp = index;
			index = indexPereSame;
			indexPereSame = tmp;
		}
		arbre[index].poids++;
		index = arbre[index].dp + index;


		if (arbre[index].dp == 0 && flag != 0) {
			// On est a la racine
			// Il faut mettre a jour les codes d'arbres dans tableau;
			// Si flag == 0, il n'y a pas eu de reequilibrage, on a seulement incrementer les poids
			// et donc, pas de changement de code
			for (int i = 0; i < 256; i++) {
				// tableau[i].index != -1 => on a lu i-128 avant
				// tableau[i].valid == 1 => on a bouge la feuille durant le reequilibrage
				if (tableau[i].index != -1 && tableau[i].valid == 1) {
					free(tableau[i].code);
					tableau[i].code = getCodeFromiToRoot(arbre, tableau[i].index, len);
					tableau[i].valid = 0;
				}
			}

		}



	}
	arbre[index].poids++;

}



void incrementChar(noeud* arbre, int len, int indexDeC, indexCode *tab) {
	arbre[indexDeC].poids++;
	reequilibre(arbre, indexDeC, len, tab);
}


void deplacement (noeud* arbre, int nbNod, int k) {
	// Enfaite c'est un memmove, sauf que la fonction memmove ne fonctionnait pas
	// deplace les cases de k
	// On suppose que la memoire est bien allouee
	for (int i = nbNod-1; i>=0; i--) {
		arbre[i+k] = arbre[i];
	}

}

noeud* addCharInTree (noeud** arbre, char c, int *nbNod, indexCode *tableau) {

	*arbre = realloc (*arbre, ((*nbNod)+2) * sizeof(noeud));

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

	tableau[c+128].index = 1;

	for (int i = -128; i < 128; i++) {
		if (tableau[i+128].index != -1 && i != c) {
			// On est dans une case de caractere deja rencontree, et differente de celle qu'on a
			tableau[i+128].index += 2;
		}

	}


	tableau[c+128].code = getCodeFromiToRoot(*arbre, 1,*nbNod);
	tableau[c+128].valid = 0;

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

char* addCharInAlreadyRead(char ** alreadyRead, char c, int* len) {
	if (*len != 0) {
		*alreadyRead = realloc (*alreadyRead, sizeof(char)*((*len)+1));
	}
	(*alreadyRead)[*len] = c;
	(*len)++;
	return *alreadyRead;
}


char* getCodeFromiToRoot (noeud* arbre, int i, int len) {
	// l represente le nomble re de bloc allouee pour
	int l = 10;
	char *res = calloc (l,sizeof(char));
	int inteRes = 0;
	noeud n1 = arbre[i];


	int id = i;
	int oldid = i;
	// Tant qu'on atteint pas la racine
	while (n1.dp != 0) {
		id = id + arbre[id].dp;
		n1 = arbre[id];

		if (inteRes == l) {
			res = realloc (res, (l = 2*l)*sizeof(char));
		}

		if (id + arbre[id].dfg == oldid) {
			res[inteRes++] = '0';
		} else {
			res[inteRes++] = '1';
		}
		oldid = id;
	}
	res = realloc (res, (inteRes)*sizeof(char));
	return res;
}

char* getCodeFrom0ToRoot (noeud* arbre, int len) {
	return getCodeFromiToRoot(arbre, 0, len);
}
