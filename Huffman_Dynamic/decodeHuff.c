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
int truc = 0;
//Décalage+1 c'est le bit a partir du quel on doit lireZero
int lireZero(int descR, int decalage, char *lu){
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
    return idx;
}

int readLetter(int descR, int descW, noeud ***arbre, int *nbNode, int decalage, char *lu){

    int idx = lireZero(descR, decalage, lu);
    if(idx == -1){
        return -1;
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
    **arbre = addCharInTree(*arbre, c, nbNode);
    if(write(descW, &c, 1) <0){
        perror("");
        exit(-1);
    }
    reequilibre (**arbre, 2, *nbNode, 0);
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




    noeud **arbre = calloc(sizeof(*arbre),1);
    *arbre = setArbre();
    int * nbNode = calloc(1,sizeof(int));
    *nbNode = 1;


    char *c= calloc(1, sizeof(char));
    read(descRead,c,1);

    int r =readLetter(descRead, descWrite, &arbre, nbNode,-1, c);
    if(r==-3){
        close(descRead);
        close(descWrite);
        return 0;
    }else if(r == -2){
        return -1;
    }
    int idx =0;
    int pos = *nbNode-1;
    noeud cur = (*arbre)[pos];
    createDotFile (new_file(), *arbre,*nbNode);
    while(1){
        read(descRead, c, 1);

        for(int i = 7;i>=0;i--){
            printf("%d",((*c & bitTab[i])!=0)?1:0);
        }
        puts("");
        for(idx = 0;idx<8 ;idx++){
            //Si le bit a la pos idx est en 1
            //Peut etre tester des cas ou y'a des erreurs ?
            if((*c & bitTab[7-idx])== bitTab[7-idx]){
                pos += cur.dfd;
                printf("lettre = %c CUR.DFD = %d, pos = %d, idx= %d\n",cur.lettre,cur.dfd,pos,idx);
            }else{
                pos += cur.dfg;
                printf("lettre = %c CUR.DFG = %d, pos = %d, idx= %d\n",cur.lettre,cur.dfg,pos,idx);
            }
            cur = (*arbre)[pos];
            //On est a une lettre
            printf("pos = %d,idx = %d\n",pos,idx);
            if(cur.dfg == 0 && cur.dfd==0){
                if(pos == 0){
                    //Il faut lire une lettre
                    idx = readLetter(descRead, descWrite, &arbre, nbNode, idx,c)-1;
                    printf("IDX = %d\n",idx);
                    if(idx == -2){
                        close(descRead);
                        close(descWrite);
                        return 0;
                    }else if(idx < -1){
                        return -1;
                    }
                }else{
                    reequilibre(*arbre,searchChar(*arbre, cur.lettre, *nbNode),*nbNode,0);
                    write(descWrite, &cur.lettre, 1);
                }
                pos = *nbNode-1;
                cur=(*arbre)[pos];
                createDotFile (new_file(), *arbre,*nbNode);
            }
        }
    }
}
