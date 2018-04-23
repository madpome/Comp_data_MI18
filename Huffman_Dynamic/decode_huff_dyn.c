#include  "arbre.h"

int read_header(int desc){
    char *s = calloc(5,sizeof(char));
    read(desc, s, 4*sizeof(char));
    s[4]='\0';
    return (strcmp(s,"HDMI")!=0)?-1:1;
}

int readLetter(int descR, int descW, noeud *arbre, int * nbNode){
    char c;
    //Lecture de 2 fois
    int r = read(descR, &c, 1);
    if(r <= 0){
        //Ca ne devrait pas arriver, il y a une erreur
        fprintf(stderr,"Erreur lecture\n");
        return -3;
    }
    if(c != 0){
        //Fin du fichier
        return -1;
    }
    r = read(descR, &c, 1);
    if(r <= 0){
        //Ca ne devrait pas arriver, il y a une erreur
        fprintf(stderr,"Erreur lecture2\n");
        return -2;
    }
    //On l'ajoute à l'arbre
    addCharInTree(arbre, c, nbNode);
    //Et on l'écrit bien sur
    if(write(descR, &c, 1) < 0){
        perror("");
        exit(-1);
    }
    return 1;
}


int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr,"2 arguments : entrée et sortie\n");
        return -1;
    }
    int descRead = open(argv[1], O_RDONLY);
    int descWrite = open(argv[2], O_WRONLY | O_CREAT);
    if(descRead <0){
        fprintf(stderr, "Impossible d'ouvrir en lecture %s\n",argv[1]);
    }
    if(descWrite < 0){
        fprintf(stderr, "Impossible d'ouvrir en écriture %s\n",argv[2]);

    }
    if(read_header(descRead)!=1){
        fprintf(stderr,"Header incorrect\n");
        return -1;
    }
    noeud *arbre = setArbre();
    int nbNode = 1;
    int r =readLetter(descRead, descWrite, arbre, &nbNode);
    if(r==-3){
        close(descRead);
        close(descWrite);
        return 0;
    }else if(r == -2){
        return -1;
    }

    unsigned char c;
    int idx =0;
    unsigned char bitTab[8];
    for(int i = 0;i<8;i++){
        bitTab[i] = 0 | (1<<i);
    }
    int pos = nbNode-1;
    noeud cur = arbre[pos];
    while(1){
        read(descRead, &c, 1);
        for(idx = 0;idx<8 ;idx++){
            //Si le bit a la pos idx est en 1
            //Peut etre tester des cas ou y'a des erreurs ?
            if((c & bitTab[idx])== bitTab[idx]){
                pos += cur.dfg;
            }else{
                pos += cur.dfd;
            }
            cur = arbre[pos];
            //On est a une lettre
            if(cur.dfg == 0 && cur.dfd==0){
                if(pos == 0){
                    //Il faut lire une lettre
                    r = readLetter(descRead, descWrite, arbre, &nbNode);
                    if(r == -1){
                        close(descRead);
                        close(descWrite);
                        return 0;
                    }else if(r < 0){
                        return -1;
                    }
                }else{
                    incrementChar(arbre, c, nbNode);
                    write(descWrite, &cur.lettre, 1);
                    pos = nbNode-1;
                    cur=arbre[pos];
                }
            }
        }

    }
}
