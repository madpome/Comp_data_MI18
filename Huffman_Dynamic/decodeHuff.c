#include  "arbre.h"
#include <unistd.h>
#include <sys/types.h>
unsigned char bitTab[8];
int read_header(int desc){
    char *s = calloc(5,sizeof(char));
    read(desc, s, 4*sizeof(char));
    s[4]='\0';
    return (strcmp(s,"HDMI")!=0)?-1:1;
}
int truc =0;
/*
int readLetter(int descR, int descW, noeud **arbre, int * nbNode, int decalage,char * cs){
    char c = *cs;
    int nbZero = 0;
    printf("SEEK_CUR = %ld\n",lseek(descR,0,SEEK_CUR));
    printf("C = %d, decalage = %d\n",c,decalage);
    for(int i = decalage+1; i<8; i++){
        if((c & bitTab[i]) == bitTab[i]){
            printf("OHLALA %d\n",i);
            return -1;
        }
        nbZero ++;
    }
    printf("nbZero %d\n",nbZero);
    read(descR,cs,1);
    printf("2EME LECTURE %c\n",*cs);
    int idx = 0;
    if(nbZero != 8){
        for(int i = 0; i<8-nbZero; i++){
            if((c & bitTab[i]) == bitTab[i]){
                return -1;
            }
            idx ++;
        }
    }
    int zero = idx;
    for(int i = idx ;i<8;i++){
        *cs |= (c & bitTab[i]);
    }
    if(zero!=0){
        read(descR,&c,1);
        for(int i = 0; i<8-zero;i++){
            *cs|=(c & bitTab[i]);
        }
    }
    printf("3EME LECTURE %c\n",*cs);
    //Et on l'écrit bien sur
    printf("LETTER WROTE : |%c|\n",*cs);
    if(write(descW, cs, 1) < 0){
        perror("");
        exit(-1);
    }
    *cs = c;

    return 8-zero+1;
}
*/
int readLetter(int descR, int descW, noeud **arbre, int *nbNode, int decalage, char *lu){
    int nbZero = 0;
    for(int i = decalage+1; i<8; i++){
        if((*lu & bitTab[7-i])==bitTab[7-i]){
            return -1;
        }
        nbZero ++;
    }
    read(descR,lu,1);
    int idx = 0;
    if(nbZero!=8){
        for(int i = 7 ;i>=nbZero; i--){
            if((*lu & bitTab[i])==bitTab[i]){
                return -1;
            }
            idx++;
        }
    }
    //A partir de la on lit pour la lettre
    printf("IDX = %d LU \n",idx);
    puts("lecture en cours");
    for(int i = 7;i>=0;i--){
        printf("%d",((*lu & bitTab[i])!=0)?1:0);
    }
    puts("");
    char c = 0;
    for(int i = 7-idx; i>=0;i--){
        if((*lu & bitTab[i])!=0){
            c|=(bitTab[i+idx]);
        }
    }
    if(idx != 0){
        read(descR, lu, 1);
        for(int i = 7;i>=8-idx;i--){
            if((*lu & bitTab[i])!=0){
                c|=(bitTab[7-i]);
            }
        }
    }
    for(int i = 7;i>=0;i--){
        printf("%d",((c & bitTab[i])!=0)?1:0);
    }
    printf("LETTRE WROTE : %c\n",c);
    *arbre = addCharInTree(arbre, c, nbNode);
    if(write(descW, &c, 1) <0){
        perror("");
        exit(-1);
    }
    return idx;
}

int new_file(){
    char *s = calloc(100,1);
        sprintf(s,"tree%d.png",truc++);
    return open(s,O_CREAT|O_RDWR,0777);
}
int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr,"2 arguments : entrée et sortie\n");
        return -1;
    }
    int descRead = open(argv[1], O_RDONLY);

    int descWrite = open(argv[2], O_CREAT|O_RDWR, 0777);
    if(descRead <0){
        fprintf(stderr, "Impossible d'ouvrir en lecture %s\n",argv[1]);
    }
    if(descWrite < 0){
        printf("%s\n",argv[2]);
        fprintf(stderr, "Impossible d'ouvrir en écriture %s\n",argv[2]);
    }
    for(int i = 0;i<8;i++){
        bitTab[i] = 0 | (1<<i);
    }

    if(read_header(descRead)!=1){
        fprintf(stderr,"Header incorrect\n");
        return -1;
    }

    noeud *arbre = setArbre();
    int * nbNode = malloc(sizeof(int));
    *nbNode = 1;

    char *c= calloc(1,1);
    read(descRead,c,1);
    int r =readLetter(descRead, descWrite, &arbre, nbNode,-1, c);
    printf("poids arbre[2] %d\n",arbre[2].poids);
    if(r==-3){
        close(descRead);
        close(descWrite);
        return 0;
    }else if(r == -2){
        return -1;
    }
    *c= 0;
    int idx =0;
    int pos = *nbNode-1;
    noeud cur = arbre[pos];
    printf("SEEK_CURHORS = %ld\n",lseek(descRead,0,SEEK_CUR));
    createDotFile (new_file(), arbre,*nbNode);
    while(1){
        read(descRead, &c, 1);
        for(idx = 0;idx<8 ;idx++){
            //Si le bit a la pos idx est en 1
            //Peut etre tester des cas ou y'a des erreurs ?
            if((*c & bitTab[7-idx])== bitTab[7-idx]){
                pos += cur.dfd;
            }else{
                pos += cur.dfg;
            }
            cur = arbre[pos];
            //On est a une lettre
            printf("pos = %d,idx = %d\n",pos,idx);
            if(cur.dfg == 0 && cur.dfd==0){
                if(pos == 0){
                    //Il faut lire une lettre
                    printf("SEEK_CURHORS = %ld\n",lseek(descRead,0,SEEK_CUR));
                    idx = readLetter(descRead, descWrite, &arbre, nbNode, idx,c);
                    printf("SEEK_CURHORS APRES = %ld et IDX =%d\n",lseek(descRead,0,SEEK_CUR),idx);
                    if(idx == -1){
                        puts("ici");
                        close(descRead);
                        close(descWrite);
                        return 0;
                    }else if(idx < 0){
                        puts("1");
                        return -1;
                    }
                }else{
                    incrementChar(arbre, *nbNode, searchChar(arbre, *c, *nbNode));
                    write(descWrite, &cur.lettre, 1);
                    cur=arbre[pos];
                }
                pos = *nbNode-1;
                createDotFile (new_file(), arbre,*nbNode);

            }
        }
        printf("poids arbre[2]oula %d\n",arbre[2].poids);

    }
}
