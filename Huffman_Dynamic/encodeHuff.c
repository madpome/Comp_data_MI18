#include "arbre.h"
#include "encodeHuff.h"

/*
	On va definir plusieurs variables globales, qui serviront a ecrire en stream,
	durant la construction de l'arbre

	buff :		c'est le tampon dans lequel on va ecrire
	nbrDeBit :	nombre de bit qu'on a ecrit dans buff, on a 0 <= nbrDeBit <= 8.
				Quand on a nbrDeBit == 8, on ecrit

*/
unsigned char buff = '\0';
int nbrDeBit = 0;



/* chain est un string de 1 et de 0, se terminant par \0
	descout est un descripteur deja ouvert, ou on va ecrire
*/
void writeStringOfBit (int descout, char * chain) {
	for (int i = 0; i<strlen(chain); i++) {
		buff = buff<<1;
		/*
			On a shift les bits de 1 vers la gauche, avec un 0 au debut
			Si on a un 1, on va passer le bit du debut a 1, sinon on ne fait rien
		*/
		if (chain[i] == '1') {
			//chain[i] == 1
			buff = buff|1;
		}
		nbrDeBit++;
		if (nbrDeBit == 8) {
			write(descout, &buff, 1*sizeof(unsigned char));
			buff = '\0';
			nbrDeBit = 0;
		}
	}
}

void writeAChar (int descout, char c) {
	for (int i = 7; i>=0; i--) {
		buff = buff << 1;

		char iemebit = (((c<<(7-i))>>7)&1); // On passe de 00001000 -> 00000001
		buff = buff | iemebit;
		nbrDeBit++;
		if (nbrDeBit == 8) {
			write(descout, &buff, 1*sizeof(unsigned char));
			buff = '\0';
			nbrDeBit = 0;
		}
	}
}

/*
	fin :	si fin == 0, c'est pas le dernier caractere, donc on va ecrire 00000000 avant c
			si fin == 1, c'est le dernier caractere, donc on va ecrire 11111111 avant c
*/
void writeACharWithPredec (int descout, char c, int fin) {

	char premieroct = (fin == 0)?'\0':-1;
	writeAChar(descout, premieroct);
	writeAChar(descout, c);

}

void writeHeader (int descout) {
	write(descout, "HDMI", 4*sizeof(char));
}

int getLen (noeud* arbre) {
	int i = 0;
	while (arbre[i].dp != 0) {
		i++;
	}
	return i;
}

char* getCodeFromiToRoot (noeud* arbre, int i, int len) {
	// l represente le nomble re de bloc allouee pour
	int l = 10;
	char *res = calloc (l,sizeof(char));
	int inteRes = 0;
	noeud n1 = arbre[i];

	if (n1.dp == 0) {
		printf("\n****************** Probleme getCodeFromiToRoot l 95 ******************\n");
		afficheArbre(arbre, len);
		printf("****************** Probleme getCodeFromiToRoot l 95 ******************\n\n");
		int descdot = open("qwe.dot", O_CREAT | O_RDWR,S_IRWXU);
		if (descdot < 0) {
			perror("qwe.dot doesn't exit ");
			exit(-1);
		}
		createDotFile(descdot, arbre, len);
		close(descdot);

		char * s = calloc (1024,sizeof(char));
		sprintf(s,"dot -Gcharset=latin1 -Tpng -o qwe.png qwe.dot");
		system(s);
		exit(-1);
	}


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

char* reverseString (char* str) {
	char* res = calloc (strlen(str)+1, sizeof(char));
	for (int i = 0; i<strlen(str); i++) {
		res[strlen(str)-1-i] = str[i];
	}
	return res;
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

	char* alreadyRead = calloc (1, sizeof(char));
	//nbChar = longueur de alreadyRead
	int nbChar = 0;
	alreadyRead[0] = -1;

	int step = 0;

	char readLetter = '0';


	char destFilename [strlen(args[1]) + 9];
	strcpy(destFilename, args[1]);
	strcat(destFilename, ".HDMI.cmp");

	FILE * lol1 = fopen(destFilename,"w");
	fclose(lol1);


	int descout = open(destFilename, O_CREAT | O_RDWR,S_IRWXU);

	if (descout < 0) {
		perror("Error opening the destination : ");

	}
	writeHeader(descout);



	while (read (descin, &readLetter, 1*sizeof(char)) == 1) {
		step++;


		if (contain(alreadyRead, readLetter, nbChar) < 0) {
			// On a jamais vu le caractere depuis le debut
			if(step == 1) {
				char prev = '\0';
				write(descout, &prev, 1*sizeof(char));
				write(descout, &readLetter, 1*sizeof(char));
				//printf("%c", readLetter);
			} else {
				char* code = reverseString(getCodeFrom0ToRoot(arbre, nbNod));
				//printf("%s%c", code, readLetter);
				writeStringOfBit(descout,code);
				writeACharWithPredec(descout, readLetter, 0);
			}
			arbre = addCharInTree(&arbre, readLetter, &nbNod);
			reequilibre(arbre, 2, nbNod, 0);
			alreadyRead = addCharInAlreadyRead(&alreadyRead, readLetter, &nbChar);
		} else {
			// On a deja vu le caractere avant
			int indexOfReadLetter = searchChar(arbre, readLetter, nbNod);
			char * code = reverseString(getCodeFromiToRoot(arbre, indexOfReadLetter, nbNod));
			//printf("%s", code);
			writeStringOfBit(descout,code);
			reequilibre(arbre, indexOfReadLetter, nbNod, 0);
		}



	}


	//printf("\n");

	// On ecrit le chemin vers 0
	writeStringOfBit(descout, reverseString(getCodeFrom0ToRoot(arbre, nbNod)));
	// On ecrit le caractere de fin
	writeACharWithPredec(descout, '\0', 1);
	writeAChar(descout, '\0');





	FILE * lol = fopen("qwe.dot","w");
	fclose(lol);

	

	

	remove("qwe.dot");
	//free(s);


	close(descout);
	//free(arbre);
	close(descin);
	return 0;
}
