#include "encodeHuffOpti.h"

/*
	On va definir plusieurs variables globales, qui serviront a ecrire en stream,
	durant la construction de l'arbre

	buff :		c'est le tampon dans lequel on va ecrire
	nbrDeBit :	nombre de bit qu'on a ecrit dans buff, on a 0 <= nbrDeBit <= 8.
				Quand on a nbrDeBit == 8, on ecrit

*/
unsigned char buff = '\0';
int nbrDeBit = 0;



/* chain est un string de 1 et de 0, se terminant par \0, on va l'ecrire a l'envers
	fout est la destination, deja ouverte, ou on va ecrire
*/
void writeStringOfBit (FILE *f, char * chain) {
	int l = strlen(chain);
	for (int i = l-1; i >= 0; i--) {
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
			fwrite(&buff, sizeof(unsigned char), 1, f);
			buff = '\0';
			nbrDeBit = 0;
		}
	}
}

void writeAChar (FILE *f, char c) {
	for (int i = 7; i>=0; i--) {
		buff = buff << 1;
		char iemebit = (((c<<(7-i))>>7)&1); // On passe de 00001000 -> 00000001
		buff = buff | iemebit;
		nbrDeBit++;
		if (nbrDeBit == 8) {
			fwrite(&buff, sizeof(unsigned char), 1, f);
			buff = '\0';
			nbrDeBit = 0;
		}
	}
}

/*
	fin :	si fin == 0, c'est pas le dernier caractere, donc on va ecrire 00000000 avant c
			si fin == 1, c'est le dernier caractere, donc on va ecrire 11111111 avant c
*/
void writeACharWithPredec (FILE *f, char c, int fin) {

	char premieroct = (fin == 0)?'\0':-1;
	writeAChar(f, premieroct);
	writeAChar(f, c);

}

void writeHeader (FILE *fout) {
	fwrite ("HDMI", sizeof(char), 4, fout);
}

int getLen (noeud* arbre) {
	int i = 0;
	while (arbre[i].dp != 0) {
		i++;
	}
	return i;
}




int main (int taille, char *args[]) {
	if (taille < 3) {
		fprintf(stderr, "Missing args : input output\n");
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

	int step = 0;

	char readLetter = '0';

	indexCode *tableau = calloc (256, sizeof(indexCode));
	for (int i = 0; i<=255; i++) {
		tableau[i].index = -1;
		tableau[i].code = calloc (1, sizeof(char));
		tableau[i].code[0] = '0';
		tableau[i].valid = 1;
	}

	char *destFilename = args[2];


	FILE *fout = fopen(destFilename, "w+");

	if (fout == NULL) {
		perror("Error opening the destination : ");
		return -1;
	}
	writeHeader(fout);



	while (read (descin, &readLetter, 1*sizeof(char)) == 1) {
		step++;
		// -128 <= readLetter <= 127
		if (tableau[readLetter+128].index == -1) {

			// On a jamais vu le caractere depuis le debut
			if(step == 1) {
				writeACharWithPredec(fout, readLetter, 0);
				//printf("%c", readLetter);
			} else {
				char* code = getCodeFrom0ToRoot(arbre, nbNod);
				writeStringOfBit(fout,code);
				writeACharWithPredec(fout, readLetter, 0);
				free(code);
			}
			addCharInTree(&arbre, readLetter, &nbNod, tableau);
			reequilibre(arbre, 2, nbNod, tableau);
		} else {
			// On a deja vu le caractere avant
			int indexOfReadLetter = tableau[readLetter + 128].index;
			//int indexOfReadLetter = searchChar(arbre, readLetter, nbNod);
			//char * code = getCodeFromiToRoot(arbre, indexOfReadLetter, nbNod);

			char *code = tableau[readLetter + 128].code;
			writeStringOfBit(fout,code);
			reequilibre(arbre, indexOfReadLetter, nbNod, tableau);

		}
	
	}


	//printf("\n");

	// On ecrit le chemin vers 0
	writeStringOfBit(fout, getCodeFrom0ToRoot(arbre, nbNod));
	// On ecrit le caractere de fin
	writeACharWithPredec(fout, '\0', 1);
	writeAChar(fout, '\0');




	/*
	FILE * lol = fopen("qwe.dot","w");
	fclose(lol);

	
	int descdot = open("qwe.dot", O_CREAT | O_RDWR,S_IRWXU);
	if (descdot < 0) {
		perror("qwe.dot doesn't exit ");
		exit(-1);
	}
	createDotFile(descdot, arbre, nbNod);
	close(descdot);
	char * s = calloc (1024,sizeof(char));
	sprintf(s,"dot -Gcharset=latin1 -Tpng -o qwe.png qwe.dot");
	system(s);
	

	remove("qwe.dot");
	*/
	//free(s);


	fclose(fout);
	free(arbre);
	close(descin);
	return 0;
}
