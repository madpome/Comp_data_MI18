#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "arbre.h"


void afficheNod (noeud nod) {
	printf("lettre = %c, dfg = %d, dfd = %d, dp = %d, poids = %d\n",nod.lettre, nod.dfg, nod.dfd, nod.dp, nod.poids);
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
	char * charac = calloc(10, sizeof(char));
	int ig = index+arbre[index].dfg;
	int id = index+arbre[index].dfd;

	if (ig != index) {
		if (arbre[index].lettre != -1) {
			sprintf(charac, "%c", arbre[index].lettre);
		} else if (arbre[index].lettre == 0) {
			charac = strcat (charac, "-1");
		} else {
			charac = strcat(charac, "");
		}
		sprintf(ligne, "\"%d,%s,%d,%d\" -> ", index, charac, arbre[index].poids, index+arbre[index].dp);
		write(desc, ligne, strlen(ligne)*sizeof(char));
		memset(charac, '\0', 10*sizeof(char));
		
		if (arbre[ig].lettre != -1) {
			sprintf(charac, "%c", arbre[ig].lettre);
		} else if (arbre[ig].lettre == 0) {
			charac = strcat (charac, "-1");
		} else {
			charac = strcat(charac, "");
		}
		sprintf(ligneFils, "\"%d,%s,%d,%d\" [label = \"0\"];\n", ig, charac, arbre[ig].poids, ig+arbre[ig].dp);
		write(desc, ligneFils, strlen(ligneFils)*sizeof(char));	


		write(desc, ligne, strlen(ligne)*sizeof(char));
		memset(charac, '\0', 10*sizeof(char));
		
		if (arbre[id].lettre != -1) {
			sprintf(charac, "%c", arbre[id].lettre);
		} else if (arbre[ig].lettre == 0) {
			charac = strcat (charac, "-1");
		} else {
			charac = strcat(charac, "");
		}
		sprintf(ligneFils, "\"%d,%s,%d,%d\" [label = \"1\"];\n", id, charac, arbre[id].poids, id+arbre[id].dp);
		write(desc, ligneFils, strlen(ligneFils)*sizeof(char));	

		free(charac);
		free(ligne);
		free(ligneFils);

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
	free(ligne);
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
	}
	noeud1->poids = noeud2->poids;


	// On met tmp dans noeud2
	noeud2->lettre = tmp.lettre;
	if (tmp.dfg != 0) {
		noeud2->dfg = index1 - index2 + tmp.dfg;
		noeud2->dfd = index1 - index2 + tmp.dfd;
	} else {
		noeud2->dfg = 0;
		noeud2->dfd = 0;
	}
	if (n2fg != index2) {
		arbre[n2fg].dp = index1 - n2fg;
		arbre[n2fd].dp = index1 - n2fd;
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
	if (plusCroissant(arbre, len) < 0) {
		return -1;
	}
	for (int i = len-1; i>index; i--) {
		//printf("index = %d, arbre[%d].poids = %d, arbre[%d].poids = %d\n",index,i,arbre[i].poids,index,arbre[index].poids);
		if (arbre[i].poids < arbre[index].poids) {
			return i;
		}
	}
	return -1;
}




void reequilibre (noeud* arbre, int index, int len, int cas) {

	int indexPerePetit = rechercheEquilibre(arbre, index, len);
	if (indexPerePetit > 0) {
		printf("On a perdu l'equilibre, indexPerePetit = %d, index = %d\n", indexPerePetit, index);
		swap(index, indexPerePetit, arbre);
		int tmp = index;
		index = indexPerePetit;	
		indexPerePetit = tmp;
	}


	int indexPere = index+arbre[index].dp;
	if (indexPere != index) {
		// On est pas dans la racine
		if (cas == 1) {
			arbre[indexPere].poids++;
			reequilibre(arbre, indexPere,len, cas);
		} else {
			reequilibre(arbre, indexPere,len, cas);
			arbre[indexPere].poids++;
		}
	}
}



void incrementChar(noeud* arbre, char c, int len) {
	int indexDeC = searchChar(arbre, c, len);
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

noeud* addCharInTree (noeud* arbre, char c, int *nbNod) {
	printf("addCharInTree c = %c\n", c);


	arbre = realloc (arbre, sizeof(noeud)*((*nbNod)+2));	
	deplacement(arbre, *nbNod, 2);
	(*nbNod)+=2;


	//Initialisation de arbre[0]
	arbre[0].lettre = -1;
	arbre[0].dfg = 0;
	arbre[0].dfd = 0;
	arbre[0].dp = 2;
	arbre[0].poids = 0;

	//Initialisation de arbre[1] (C'est la qu'on va foutre le caractere)
	arbre[1].lettre = c;
	arbre[1].dfg = 0;
	arbre[1].dfd = 0;
	arbre[1].dp = 1;
	arbre[1].poids = 1;


	//On modifie les valeurs de fils de arbre[2] (l'ancien arbre[0])
	arbre[2].dfg = -2;
	arbre[2].dfd = -1;
	arbre[2].poids++;


	reequilibre(arbre, 2, *nbNod, 1);
	return arbre;

}

int searchChar(noeud* arbre, char c, int len) {
	// On fait un simple parcours de tableau, on pourrait parcourir en tant qu'arbre,
	// Mais on pourrait rechercher dans tout l'arbre a la fin.
	for (int i = len-1; i >= 0; i--) {
		if (arbre[i].lettre == c) {
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

char* addCharInAlreadyRead(char * alreadyRead, char c, int* len) {
	if (*len != 0) {
		alreadyRead = realloc (alreadyRead, sizeof(char)*((*len)+1));
	}
	alreadyRead[*len] = c;
	(*len)++;
	return alreadyRead;
}










int main (int taille, char *args[]) {
	if (taille == 1) {
		fprintf(stderr, "Missing args : nom du fichier a compresser\n");
		exit(-1);
	}
	int descin = open(args[1], O_RDONLY);
	if (descin == -1) {
		perror("Error opening the input");
		exit(-1);
	}

	noeud* arbre = setArbre();
	// nbNod = longueur de arbre;
	int nbNod = 1;

	for (int i = 0; i<nbNod; i++) {
		printf("%d :: ", i+1);
		afficheNod(arbre[i]);
	}

	char* alreadyRead = calloc (1, sizeof(char));
	//nbChar = longueur de alreadyRead
	int nbChar = 0;
	alreadyRead[0] = -1;

	int step = 0;

	char readLetter = '0';
	while (read (descin, &readLetter, 1) == 1) {
			step++;
			printf("\nNombre de character lu : %d\n", step);
			if (contain(alreadyRead, readLetter, nbChar) < 0) {
				// On a jamais vu le caractere depuis le debut
				printf("Avant ajout : nbChar = %d, nbNod = %d\n", nbChar, nbNod);
				arbre = addCharInTree(arbre, readLetter, &nbNod);
				alreadyRead = addCharInAlreadyRead(alreadyRead, readLetter, &nbChar);
				printf("Apres ajout : nbChar = %d, nbNod = %d\n", nbChar, nbNod);
			} else {
				// On a deja vu le caractere avant
				incrementChar(arbre, readLetter, nbNod);
			}
		
	}


	
	int descdot = open("qwe.dot", O_WRONLY);
	if (descdot < 0) {
		perror("qwe.dot doesn't exit ");
		exit(-1);
	}
	createDotFile(descdot, arbre, nbNod);
	close(descdot);



	char * s = calloc (1024,sizeof(char));
	sprintf(s,"dot -Gcharset=latin1 -Tpng -o qwe.png qwe.dot");
	system(s);
	free(s);
	FILE * lol = fopen("qwe.dot","w'");
	fclose(lol);
	
	free(arbre);
	close(descin);
	return 0;
}
