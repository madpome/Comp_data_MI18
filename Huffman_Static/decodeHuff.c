#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



typedef struct lettreCodeCouple {
  int id, left, right;
  char character;
} lettreCodeCouple;

lettreCodeCouple* construitArbre (int desc, int lentab);
int getNbDernierBits (int desc);
int getNbNoeud (int desc);
int checkEntete (int descriptor);
int power (int i, int n);
int isIn (int i, int a, int b);
void huffmanDecodeur (int desc, FILE *fout);



int main (int taille, char *args[]) {
	if (taille <= 2) {
		fprintf(stderr, "Missing args\n");
		return -1;
	}
	char *filename = args[1];
	int desc = open (filename, O_RDONLY);
	if (desc < 0) {
		perror("Error opening");
		return -1;
	}
	if (checkEntete(desc) < 0) {
		fprintf(stderr, "Header incorrect, can't uncompress\n");
		return -1;
	}
	filename = args[2];
	FILE *fout = fopen(filename, "w+");
	// Ici, on est placé juste avant les premiers octets codés, et on est sur que l'en tete est correct
	if (fout == NULL) {
		perror("Error opening the output");
		return -1;
	}
	huffmanDecodeur(desc, fout);
	close(desc);
	fclose(fout);

	return 0;
}

void huffmanDecodeur (int desc, FILE *fout) {
	int nbDernier = getNbDernierBits(desc);
	int lentab = getNbNoeud (desc);
	lettreCodeCouple *tab = construitArbre(desc, lentab);

	unsigned char oct = 0;
	int index = lentab - 1;
	char nextChar = '\0';
	int nbBits = 8;
	while (read (desc, &oct, 1 * sizeof (unsigned char)) > 0) {
		if (read (desc, &nextChar, 1 * sizeof (char)) == 0) {
			// On est sur le dernier octet, il faut pas lire tout les bits
			nbBits = nbDernier;
		} else {
			lseek (desc, -1, SEEK_CUR);
		}

		//for (int i = 7; i>=0; i--) {
		for (int i = 0; i<nbBits; i++) {
			int dir = (((oct<<(7-i))>>7)&1);
			if (dir == 0) {
				index = tab[index].left;
				if (tab[index].left != -1) { // ie on n'est pas dans une feuille
				} else {
					fputs(&(tab[index].character), fout);
					//fprintf(stderr, "ga : id = %d, char = %c |", tab[index].id, tab[index].character);
					index = lentab - 1;
				}
			} else {
				index = tab[index].right;
				if (tab[index].right != -1) { // ie on n'est pas dans une feuille

				} else {
					fputs(&(tab[index].character), fout);
					//fprintf(stderr, "az : id = %d, char = %c |", tab[index].id, tab[index].character);
					index = lentab - 1;
				}
			}
		}
	}
}


int getNbDernierBits (int desc) {
	int nbALire = 1;
	lseek (desc, 4*sizeof(unsigned char), SEEK_SET);
	unsigned char *receptacle = calloc (nbALire, sizeof(unsigned char));
	read (desc, receptacle, nbALire*sizeof(unsigned char));
	int nbBits = receptacle[0] * power(256, 0);
	free(receptacle);
	return nbBits;
}

int getNbNoeud (int desc) {
	// Permet de lire passer les 6 premiers octets, et lire les deux suivants.
	int nbALire = 2;
	lseek (desc, 7*sizeof(unsigned char),SEEK_SET);
	unsigned char *receptacle = calloc (nbALire, sizeof(unsigned char));
	read (desc, receptacle, nbALire*sizeof(unsigned char));
	int nbNoeud = receptacle[0] * power(256, 1) + receptacle[1] * power(256, 0);
	free(receptacle);
	return nbNoeud;
}

lettreCodeCouple* construitArbre (int desc, int lentab) {
	// On va se replacer au niveau des octets codant l'id du plus gros noeud, et "construire" l'arbre.
	int nbNoeud = lentab;


	lettreCodeCouple * tab = calloc (nbNoeud, sizeof(lettreCodeCouple));
	int nbALire = 7;
	unsigned char *receptacle;
	for (int i = 0; i<nbNoeud; i++) {
		receptacle = calloc (nbALire, sizeof(unsigned char));
		if (read (desc, receptacle, nbALire*sizeof(unsigned char)) > 0) {
			int id = receptacle[0] * power(256, 1) + receptacle[1] * power (256, 0) - 1;
			tab[id].id = id;
			tab[id].character = (char)(receptacle[2] * power (255,0));
			tab[id].left = receptacle[3] * power(256, 1) + receptacle[4] * power (256, 0) - 1;
			tab[id].right = receptacle[5] * power(256, 1) + receptacle[6] * power (256, 0) - 1;
			//if (tab[i].character == '\0') {
			//}
			// Le -1 c'est pour avoir un indice dans le tableau.
		}
	}

	return tab;
}

int checkEntete (int desc) {
	// Verifie l'entete du fichier, qui est deja ouvert
	int nbALire = 4;
	unsigned char *receptacle = calloc (nbALire, sizeof(unsigned char));
	unsigned char magicNumberHSMI[4] = {'H', 'S', 'M', 'I'};
	if (read (desc, receptacle, nbALire*sizeof(unsigned char)) < nbALire) {
		perror ("Not enough to read");
		return -1;
	} else {
		for (int i = 0; i<nbALire; i++) {
			if (receptacle[i] != magicNumberHSMI[i]) {
				fprintf(stderr, "Not the right magic number\n");
				return -1;
			}
		}
	}
	free(receptacle);
	// Ici on a le bon magic number.

	// On verifie que le nombre final de bits est compris entre 0 et 8
	nbALire = 1;
	receptacle = calloc (nbALire, sizeof(unsigned char));
	int nbBits = 0;
	if (read (desc, receptacle, nbALire*sizeof(unsigned char)) < nbALire) {
		perror ("Not enough to read");
		return -1;
	} else {
		nbBits = receptacle[0] * power(256, 0);
		if (isIn (nbBits, 0, 8) < 0) {
			fprintf(stderr, "Not enough or too much bits to read");
			return -1;
		}
	}
	free(receptacle);

	// On va verifier que l'ID du plus gros noeud est egale au nombre de noeud
	nbALire = 4;
	receptacle = calloc (nbALire, sizeof(unsigned char));
	int nbNods = 0;
	int highestId = 0;
	if (read (desc, receptacle, nbALire*sizeof(unsigned char)) < nbALire) {
		perror ("Not enough to read");
		return -1;
	} else {
		nbNods = receptacle[0] * power(256, 1) + receptacle[1] * power (256, 0);
		highestId = receptacle[2]*power(256, 1) + receptacle[3] * power (256,0);
		if (nbNods != highestId) {
			fprintf(stderr, "Number of nods, and heaviest nod's id don't match.\n : nbNodes = %d, highestId = %d\n", nbNods, highestId);
			return -1;
		}
	}
	free(receptacle);

	// Ici, on est sur d'avoir le bon nombre de noeud.

	// On va verifier que tout les noeuds sont bien codes.
	nbALire = 7;
	for (int i = 0; i<nbNods; i++) {
		receptacle = calloc (nbALire, sizeof(unsigned char));
		if (read (desc, receptacle, nbALire*sizeof(unsigned char)) < nbALire) {
			perror ("Not enough to read");
			return -1;
		} else {
			int id = receptacle[0] * power(256, 1) + receptacle[1] * power (256, 0);
			int character = receptacle[2] * power (256,0);
			int fg = receptacle[3] * power(256, 1) + receptacle[4] * power (256, 0);
			int fd = receptacle[5] * power(256, 1) + receptacle[6] * power (256, 0);
			if (isIn (id, 0, nbNods) < 0 || isIn (character, 0, 256) < 0 || isIn (fg, 0, nbNods) < 0 || isIn (fd, 0, nbNods) < 0) {
				fprintf(stderr, "A nod isn't right\n");
				fprintf(stderr, "%d %c %d %d \n", id, character, fg, fd);
				fprintf(stderr, "%d, %d, %d, %d\n", isIn (id, 0, nbNods), isIn (character, 0, 256), isIn (fg, 0, nbNods), isIn (fd, 0, nbNods));
				return -1;
			}
		}
	}
	return 1;
}

int isIn (int i, int a, int b) {
	return (a <= i && i <= b)?1:-1;
}

int power (int i, int n) {
	if (n <= 0) {
		return 1;
	} else if (n == 1) {
		return i;
	} else {
		return i*power(i, n-1);
	}
}
