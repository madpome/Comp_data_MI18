#include  "arbre.h"
#include <unistd.h>
#include <sys/types.h>
unsigned char bitTab[8];
int read_header(FILE * f){
    char *s = calloc(5,sizeof(char));
    fread(s, sizeof(char), 4, f);
    s[4]='\0';
    return (strcmp(s,"HDMI")!=0)?-1:1;
}
int truc = 0;
//Décalage+1 c'est le bit a partir du quel on doit lireZero
int lireZero(FILE * descIn, int decalage, char *lu){
    int nbZero = 0;
    for(int i = decalage+1; i<8; i++){
        if((*lu & bitTab[7-i])==bitTab[7-i]){
            return -1;
        }
        nbZero ++;
    }
    fread(lu,1,1,descIn);
    int idx = 0;
    if(nbZero!=8){
        for(int i = 7 ;i>=nbZero; i--){
            if((*lu & bitTab[i])==bitTab[i]){
                return -1;
            }
            idx++;
        }
    }
    return idx;
}

int readLetter(FILE * descR, FILE * descW, noeud ***arbre, int *nbNode, int decalage, char *lu, indexCode **tableau){

    int idx = lireZero(descR, decalage, lu);
    if(idx == -1){
        return -1;
    }
    //A partir de la on lit pour la lettre
    int bitLu = 0;
    char c = 0;
    for(int i = 7-idx; i>=0;i--){
        bitLu++;
        if((*lu & bitTab[i])!=0){
            c|=(bitTab[i+idx]);
        }
    }
    if(idx != 0){
        fread(lu, 1, 1,descR);
        for(int i = 7;i>=bitLu;i--){
            if((*lu & bitTab[i])!=0){
                c|=(bitTab[i-bitLu]);
            }
        }
    }

    **arbre = addCharInTree(*arbre, c, nbNode, *tableau);
    if(fwrite(&c, 1, 1, descW) <0){
        perror("");
        exit(-1);
    }
    reequilibre (**arbre, 2, *nbNode, *tableau);
    return idx;
}

int new_file(){
    char *s = calloc(100,1);
    sprintf(s,"tree%d.png",truc++);
    return open(s,O_CREAT | O_RDWR,0777);
}
int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr,"2 arguments : entrée et sortie\n");
        return -1;
    }
    FILE * descRead = fopen(argv[1], "r");
    FILE * f = fopen(argv[2],"w");
    fclose(f);
    FILE * descWrite = fopen(argv[2], "w");
    if(descRead <0){
        fprintf(stderr, "Impossible d'ouvrir en lecture %s\n",argv[1]);
        return -1;
    }
    if(descWrite < 0){
        fprintf(stderr, "Impossible d'ouvrir en écriture %s\n",argv[2]);
        return -1;
    }
    for(int i = 0;i<8;i++){
        bitTab[i] = 0 | (1<<i);
    }

    if(read_header(descRead)!=1){
        fprintf(stderr,"Header incorrect\n");
        return -1;
    }
    indexCode *tableau = calloc (256, sizeof(indexCode));
    for (int i = 0; i<=255; i++) {
        tableau[i].index = -1;
        tableau[i].code = calloc (1, sizeof(char));
        tableau[i].code[0] = '0';
        tableau[i].valid = 1;
    }

    noeud **arbre = calloc(sizeof(*arbre),1);
    *arbre = setArbre();
    int * nbNode = calloc(1,sizeof(int));
    *nbNode = 1;


    char *c= calloc(1, sizeof(char));
    fread(c,1,1,descRead);

    int r =readLetter(descRead, descWrite, &arbre, nbNode,-1,c,&tableau);
    if(r==-3){
        fclose(descRead);
        fclose(descWrite);
        return 0;
    }else if(r == -2){
        return -1;
    }
    int idx =0;
    int pos = *nbNode-1;
    noeud cur = (*arbre)[pos];
    while(1){
        fread(c, 1, 1, descRead);
        for(idx = 0;idx<8 ;idx++){
            //Si le bit a la pos idx est en 1
            //Peut etre tester des cas ou y'a des erreurs ?
            if((*c & bitTab[7-idx])== bitTab[7-idx]){
                pos += cur.dfd;
            }else{
                pos += cur.dfg;
            }
            cur = (*arbre)[pos];
            //On est a une lettre
            if(cur.dfg == 0 && cur.dfd==0){
                if(pos == 0){
                    //Il faut lire une lettre
                    idx = readLetter(descRead, descWrite, &arbre, nbNode, idx,c,&tableau)-1;
                    if(idx == -2){
                        fclose(descRead);
                        fclose(descWrite);
                        return 0;
                    }else if(idx < -1){
                        return -1;
                    }
                }else{
                    reequilibre(*arbre,searchChar(*arbre, cur.lettre, *nbNode),*nbNode,tableau);
                    fwrite(&cur.lettre, 1, 1 ,descWrite);
                }
                pos = *nbNode-1;
                cur=(*arbre)[pos];
            }
        }
    }
}
