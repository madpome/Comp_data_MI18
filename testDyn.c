#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>


int main () {
  int len = 11;

  const char *test[len];
  test[0] = "AmerikastaPalatessa.txt";
  test[1] = "Florante.txt";
  test[2] = "Chinois.txt";
  test[3] = "MobyDick.txt";
  test[4] = "OurFriendtheDog.txt";
  test[5] = "Explod.png";
  test[6] = "image1.jpeg";
  test[7] = "image2.jpeg";
  test[8] = "image3.png";
  test[9] = "image4.png";
  test[10] = "image5.png";



  
  float *ratio = calloc(len, sizeof(float));

  for (int i = 0; i<len; i++) {
    char *input = calloc(1024, sizeof(char));
    sprintf(input, "test_file/%s", test[i]);
    printf("input = %s\n", input);

    
    char *cmpFilename = calloc(strlen(input)+10, sizeof(char));
    sprintf(cmpFilename, "%s.HDMI.cmp", input);
    printf("Fichier compresse = %s\n", cmpFilename);
    
    char *uncmpFilename = calloc (strlen(cmpFilename)+10, sizeof(char));
    sprintf(uncmpFilename, "%s.uncmp", input);
    printf("Fichier decompresse = %s\n", uncmpFilename);

    
    char *cmdCmp = calloc(1024, sizeof(char));
    sprintf(cmdCmp, "Huffman_Dynamic/encodeHuff %s", input);

    system(cmdCmp);
    
    char *cmdUncmp = calloc(1024, sizeof(char));
    sprintf(cmdUncmp, "Huffman_Dynamic/decodeHuff %s %s", cmpFilename, uncmpFilename);
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


    ratio[i] = ((float) inputFile.st_size)/cmpFile.st_size;
    printf("Ancienne taille = %ld\n", inputFile.st_size);
    printf("Nouvelle taille = %ld\n", cmpFile.st_size);
    printf("Taux de compression : %f\n", ratio[i]);
    

    
    char *cmdDiff = calloc (strlen(input) + strlen(uncmpFilename) + 10, sizeof(char));
    sprintf(cmdDiff, "diff %s %s", input, uncmpFilename);
    printf("diff : \n");
    system(cmdDiff);
    printf("Fin diff \n");
    
    free(input);
    free(cmpFilename);
    free(uncmpFilename);
    free(cmdCmp);
    free(cmdUncmp);
  }
  
  float sum = 0;
  for (int i = 0; i<len; i++) {
    sum += ratio[i];
  }
  printf("Taux de compression moyen : %f\n\n", sum/len);



  free(ratio);
  return 0;
}
