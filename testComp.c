#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int compressionUnType (char *type, float** ratioPoint, int nbAlloc, char *algo);
char * determineDossier (char *algo);
float calculTauxCompression (char *input, char *algo);



int main (int taille, char *args[]) {
	if ((taille == 1)  || (strcmp(args[1], "HDMI") != 0 && strcmp(args[1], "HSMI") != 0 && strcmp(args[1], "LZMI") != 0)) {
		fprintf(stderr, "Missing args : \n");
		fprintf(stderr, "HSMI pour Huffman Static\n");
		fprintf(stderr, "HDMI pour Huffman Dynamic\n");
		fprintf(stderr, "LZMI pour Lempel-Ziv\n");
		exit(-1);
	}



	char *algo = args[1];

	int nbAlloc = 1024;

	float *ratioTxt = calloc(nbAlloc, sizeof(float));
	int nbTxt = compressionUnType("txt", &ratioTxt, nbAlloc, algo);

	float *ratioImg = calloc(nbAlloc, sizeof(float));
	int nbImg = compressionUnType("img", &ratioImg, nbAlloc, algo);

	float *ratioAudio = calloc (nbAlloc, sizeof(float));
	int nbAudio = 0;//compressionUnType("audio", &ratioAudio, nbAlloc, algo);

	float sumTotale = 0.;
	float sumImg = 0.;
	float sumAudio = 0.;
	float sumTxt = 0.;

	int flag = 0;

	if (nbImg != 0) {
		flag++;
		for (int i = 0; i<nbImg; i++) {
			sumImg += ratioImg[i];
			sumTotale += ratioImg[i];
		}
		printf("nbImage = %d\n", nbImg);
		printf("Taux compression moyen pour les images = %f\n", (1-(sumImg / nbImg))*100);
	}

	if (nbAudio != 0) {
		flag++;
		for (int i = 0; i<nbAudio; i++) {
			sumAudio += ratioAudio[i];
			sumTotale += ratioAudio[i];
		}
		printf("Taux compression moyen pour les audios = %f\n", (1-(sumAudio / nbAudio))*100);
	}
	if (nbTxt != 0) {
		flag++;
		for (int i = 0; i<nbTxt; i++) {
			sumTxt += ratioTxt[i];
			sumTotale += ratioTxt[i];
		}
		printf("Taux compression moyen pour les textes = %f\n", (1-(sumTxt / nbTxt))*100);
	}

	if (flag != 0) {
		printf("Taux de compression moyen en tout = %f\n", (1-(sumTotale/(nbImg + nbAudio + nbTxt)))*100);
	}

	if(strcmp(algo,"HSMI")==0){
		system("rm test_compression/*/*.cmp.*");
	}
	free(ratioImg);
	free(ratioAudio);
	free(ratioTxt);
  	return 0;
}

int compressionUnType (char *type, float** ratioPoint, int nbAlloc, char *algo) {
	char *pathOfType = calloc (strlen(type) + 100, sizeof(char));
	sprintf(pathOfType, "test_compression/%s", type);
	float *ratio = *ratioPoint;


	DIR *dir = opendir(pathOfType);
	if (dir == NULL) {
		printf("Error opening %s ", pathOfType);
		perror(" : ");
		exit(-1);
	}

	//nbType = nombre de fichier dans le directory
	int nbType = 0;

	struct dirent * file = NULL;


	while ((file = readdir(dir)) != NULL) {
		if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
			continue;
		}
		if (nbType == nbAlloc) {
			nbAlloc *= 2;
			ratio = realloc(ratio, nbAlloc*sizeof(float));
		}
		char *inputFilename = calloc(strlen(file->d_name) + strlen(pathOfType) + 10, sizeof(char));
		sprintf(inputFilename, "%s/%s", pathOfType, file->d_name);
		ratio[nbType++] = calculTauxCompression(inputFilename, algo);
	}

	ratio = realloc(ratio, nbType*sizeof(float));

	closedir(dir);

	return nbType;
}


char * determineDossier (char *algo) {
	if (strcmp(algo, "HSMI") == 0) {
		return "Huffman_Static";
	} else if (strcmp(algo, "HDMI") == 0) {
		return "Huffman_Dynamic";
	} else if (strcmp(algo, "LZMI") == 0) {
		return "Lempel_Ziv";
	} else {
		exit(-1);
	}
}

float calculTauxCompression (char *input, char *algo) {

    printf("\ninput = %s\n", input);

	char *directoryAlgo = determineDossier(algo);


    char *cmpFilename = calloc(strlen(input)+10, sizeof(char));
    sprintf(cmpFilename, "%s.%s.cmp", input, algo);

    char *uncmpFilename = calloc (strlen(cmpFilename)+10, sizeof(char));
    sprintf(uncmpFilename, "%s.uncmp", input);


    char *cmdCmp = calloc(1024, sizeof(char));
    sprintf(cmdCmp, "%s/encodeHuff %s %s", directoryAlgo, input, cmpFilename);
    system(cmdCmp);



    char *cmdUncmp = calloc(1024, sizeof(char));
    sprintf(cmdUncmp, "%s/decodeHuff %s %s", directoryAlgo, cmpFilename, uncmpFilename);
    system(cmdUncmp);


    struct stat inputFile;
    if (stat(input, &inputFile) < 0) {
      printf("%s : ", input);
      perror("Error : ");
      exit(-1);
    }

    struct stat cmpFile;
    if (stat(cmpFilename, &cmpFile) < 0) {
      printf("%s : ", cmpFilename);
      perror("Error : ");
      exit(-1);
    }


    float ratio = ((float) cmpFile.st_size)/inputFile.st_size;
    printf("Taille reelle = %ld\n", inputFile.st_size);
    printf("Taille compresse = %ld\n", cmpFile.st_size);
    printf("Taux de compression : %f%%\n", (1-ratio)*100.);



    char *cmdDiff = calloc (strlen(input) + strlen(uncmpFilename) + 10, sizeof(char));
    sprintf(cmdDiff, "diff %s %s", input, uncmpFilename);
    printf("diff : \n");
    system(cmdDiff);
    printf("Fin diff \n");



    remove(cmpFilename);
    remove(uncmpFilename);


    free(input);
    free(cmpFilename);
    free(uncmpFilename);
    free(cmdCmp);
    free(cmdUncmp);
    free(cmdDiff);
    return ratio;
}
