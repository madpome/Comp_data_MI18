#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
//Huffman sur un fichier binaire ?
//Avons nous besoin de savoir sur combien de bit est codé une unité d'information
//Probablement, alors on va supposer qu'il est entré en parametre
//ALORS NON on lit 8 bits par 8 bits car c'est la norme

typedef struct node {
	int id;
	char *lettre;
	//Si il n'a pas de fils alors c'est une lettre, sinon un noeud normal
	struct node * left;
	struct node * right;
	int weight;
}node;

typedef struct montableau {
	// c = caractere
	unsigned char c;
	// v = nombre d'occurence
	int v;
}montableau;
typedef struct charCode{
	char c;
	char * code;
}charCode;

void affiche (node *tab) {
	int lid = -1;
	int rid = -1;
	// On recupere l'id du fils gauche
	if (tab->left != NULL)
		lid = tab->left->id;
	// On recupere l'id du fils droit
	if (tab->right != NULL)
		rid = tab->right->id;

	printf("%d\t%c\t%d\t%d\t%d\n", tab->id, *tab->lettre, tab->weight, lid, rid);

	// On affiche l'arbre droit si il existe;
	if (rid != -1) {
		affiche(tab[0].right);
	}
	// On affiche l'arbre gauche si il existe;
	if (lid != -1) {
		affiche(tab[0].left);
	}
}

//Utilisation d'un tableau comme un tas pour l'arbre ?
int montableau_cmp(const void *a, const void *b){
	const montableau * da = a, *db = b;
	return da->v - db->v;
}

// Tri le tableau tab en fonction des poids
void insert_sort(node * tab, int nb_elem){
	for (int i = 0; i<nb_elem; i++) {
		node key = tab[i];
		int j = i-1;
		while (j>=0 && tab[j].weight>key.weight) {
			tab[j+1] = tab[j];
			j--;
		}
		tab[j+1] = key;
	}
}

/*
   Passe de :  a b c d e

   a :          f c d e
              / \
             a   b

 */
node* fusionne_deux_premiers(node *tab, int * nb_elem, int *nb_noeud){
	node *t = malloc (sizeof(node)*(*nb_elem-1));
	t[0].lettre = NULL;
	t[0].left = &tab[0];
	t[0].right = &tab[1];
	t[0].weight = tab[0].weight + tab[1].weight;
	t[0].id = (++*nb_noeud);
	if (*nb_elem >= 2) {
		(*nb_elem)--;
	}
	if (*nb_elem >= 2) {
		for (int i = 1; i<*nb_elem; i++) {
			t[i] = tab[i+1];
		}
	}
	return t;
}


// Ajoute le caractere c dans le tableai tab
void add_tab(montableau ** tab, unsigned char c, int * size, int * nb_elem){
	for(int i = 0; i<*nb_elem; i++) {
		// On a trouve c dans tab
		if((*tab)[i].c == c) {
			(*tab)[i].v++;
			return;
		}
	}
	//Si il n'est pas dedans il faut l'ajouter;
	(*tab)[*nb_elem].c = c;
	(*tab)[*nb_elem].v = 1;
	(*nb_elem)++;
	if((*nb_elem) == (*size)) {
		(*size)*=2;
		*tab = realloc(*tab, (*size)*sizeof(montableau));
	}
}


char * aux(node src, char c, char * s){
	char * p = calloc((strlen(s)+2),sizeof(char));
	if(src.lettre != NULL && *src.lettre == c) {
		strcpy(p,s);
		return p;
	}
	p=strcpy(p,s);
	p=strcat(p,"0");
	char * k=NULL;
	if(src.left != NULL) {
		k = aux(*src.left, c, p);
	}
	if(k==NULL) {
		p[strlen(p)-1]='1';
		if(src.right != NULL) {
			k=aux(*src.right,c,p);
		}
	}
	free(p);
	return k;

}

char * getCode(node src, char c){
	//Il faut pouvoir renvoyer un truc qui me dit qu'il est pas dedans mais je sais pas quoi
	//Je pense qu'on va renvoyer NULL si il le trouve pas ?
	//penser à lui foutre un \0 pour connaitre la taille du string
	//Gauche = 0
	//Droite = 1
	//Ca devrait me retourner le code à lire de gauche à droite (little endian ?)
	return aux(src, c, "\0");


}




// dir indique si le noeud est un noeud droit ou gauche (0 ou 1)
char *auxGetCode2 (node root, char c, char dir) {
	if (root.lettre!=NULL && *root.lettre == c) {
		char *q = calloc (2, sizeof(char));
		q[0] = dir;
		return q;
	}
	if (root.left == NULL) {
		// Si la lettre de root n'est pas c, et que root est une feuille
		// On a directement que root.right == NULL aussi car l'arbre est complet
		char *a = calloc (2, sizeof(char));
		a[0] = '2';
		return a;
	} else {
		// On lit la gauche
		char *s = auxGetCode2(*(root.left), c, '0');
		if (s[strlen(s)-1] == '2') {
			// Si on rencontre un '2', alors c n'est pas dans la gauche, donc on va a droite
			s = auxGetCode2(*(root.right), c, '1');
		}
		char *p = calloc (1+strlen(s), sizeof(char));
		p[0] = dir;
		strcat(p, s);
		return p;
	}
}
void auxDOT(FILE * f, node root){
	char buff[100]; //Je pense pas que ca fera plus de 100 char par ligne
	memset(buff,0,100);
	if(root.left != NULL) {
		if(root.lettre ==NULL && root.left->lettre ==NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,,%d\" [label = \"0\"];\n",root.id,root.weight,root.left->id,root.left->weight);
		}else if(root.lettre == NULL && root.left->lettre !=NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,%c,%d\" [label = \"0\"];\n",root.id,root.weight,root.left->id,*root.left->lettre,root.left->weight);
		}else if(root.lettre != NULL && root.left->lettre ==NULL) {
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,,%d\" [label = \"0\"];\n",root.id,*root.lettre,root.weight,root.left->id,root.left->weight);
		}else{
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,%c,%d\" [label = \"0\"];\n",root.id,*root.lettre,root.weight,root.left->id,*root.left->lettre,root.left->weight);
		}
		fwrite(buff,sizeof(char), strlen(buff), f);
		memset(buff,0,100);
		auxDOT(f,*root.left);
	}
	if(root.right != NULL) {
		if(root.lettre ==NULL && root.right->lettre ==NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,,%d\" [label = \"1\"];\n",root.id,root.weight,root.right->id,root.right->weight);
		}else if(root.lettre == NULL && root.right->lettre !=NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,%c,%d\" [label = \"1\"];\n",root.id,root.weight,root.right->id,*root.right->lettre,root.right->weight);
		}else if(root.lettre != NULL && root.right->lettre ==NULL) {
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,,%d\" [label = \"1\"];\n",root.id,*root.lettre,root.weight,root.right->id,root.right->weight);
		}else{
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,%c,%d\" [label = \"1\"];\n",root.id,*root.lettre,root.weight,root.right->id,*root.right->lettre,root.right->weight);
		}
		fwrite(buff,sizeof(char), strlen(buff), f);
		memset(buff,0,100);
		auxDOT(f,*root.right);
	}
}
//Créer le fichier dot ? peut etre le fichier en png aussi si ca marche bien
//f ouvert en w, début de fichier;
void createDOT(FILE * f, node root){
	char buff[100]; //Je pense pas que ca fera plus de 100 char par ligne
	sprintf(buff,"%s","digraph graphname {\n\"FORME id,char,poids\"\n");
	fwrite(buff,sizeof(char), strlen(buff), f);
	memset(buff,0,100);
	if(root.left != NULL) {
		if(root.lettre ==NULL && root.left->lettre ==NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,,%d\" [label = \"0\"];\n",root.id,root.weight,root.left->id,root.left->weight);
		}else if(root.lettre == NULL && root.left->lettre !=NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,%c,%d\" [label = \"0\"];\n",root.id,root.weight,root.left->id,*root.left->lettre,root.left->weight);
		}else if(root.lettre != NULL && root.left->lettre ==NULL) {
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,,%d\" [label = \"0\"];\n",root.id,*root.lettre,root.weight,root.left->id,root.left->weight);
		}else{
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,%c,%d\" [label = \"0\"];\n",root.id,*root.lettre,root.weight,root.left->id,*root.left->lettre,root.left->weight);
		}
		fwrite(buff,sizeof(char), strlen(buff), f);
		memset(buff,0,100);
		auxDOT(f,*root.left);
	}
	if(root.right != NULL) {
		if(root.lettre ==NULL && root.right->lettre ==NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,,%d\" [label = \"1\"];\n",root.id,root.weight,root.right->id,root.right->weight);
		}else if(root.lettre == NULL && root.right->lettre !=NULL) {
			sprintf(buff,"\t\"%d,,%d\"->\"%d,%c,%d\" [label = \"1\"];\n",root.id,root.weight,root.right->id,*root.right->lettre,root.right->weight);
		}else if(root.lettre != NULL && root.right->lettre ==NULL) {
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,,%d\" [label = \"1\"];\n",root.id,*root.lettre,root.weight,root.right->id,root.right->weight);
		}else{
			sprintf(buff,"\t\"%d,%c,%d\"->\"%d,%c,%d\" [label = \"1\"];\n",root.id,*root.lettre,root.weight,root.right->id,*root.right->lettre,root.right->weight);
		}
		fwrite(buff,sizeof(char), strlen(buff), f);
		memset(buff,0,100);
		auxDOT(f,*root.right);
	}
	sprintf(buff,"}");
	fwrite(buff,sizeof(char),strlen(buff),f);
}


// Permet d'avoir le code de c
/* Principe de l'algo :
   Regarde si c est dans l'arbre gauche (la chaine contient 2 a la fin si il n'y est pas.
   Si c est dans la gauche, on renvoit la chaine de gauche, sinon on renvois la chaine de droite.

 */
char *getCode2 (node root, char c) {
	if (root.lettre != NULL && *root.lettre == c) {
		// Impossible qu'on arrive ici a moins de compresser un fichier a un caractere
		char *a = calloc (2, sizeof(char));
		a[0] = '0';
		return a;
	}
	if (root.left != NULL) {
		char *leftStr = auxGetCode2(*(root.left), c, '0');
		if (leftStr[strlen(leftStr)-1] == '2') {
			return auxGetCode2(*(root.right), c, '1');
		} else {
			return leftStr;
		}
	} else {
		char *res = calloc (2, sizeof(char));
		res[0] = '2';
		return res;
	}

	// Impossible qu'on arrive ici : pour eviter le warning
	return NULL;
}
void write_node(FILE * output, node x){
	uint16_t i = htons(x.id);
	char c = 0;
	fwrite(&i,2,1,output);
	if(x.lettre == NULL){
		fwrite(&c,1,1,output);
	}else{
		fwrite(x.lettre,1,1,output);
	}
	uint16_t * id = malloc(sizeof *id);
	if(x.left != NULL){
		*id=htons((*x.left).id);
		fwrite(id,2,1,output);
	}else{
		*id=0;
		fwrite(id,2,1,output);
	}
	if(x.right != NULL){
		*id=htons((*x.right).id);
		fwrite(id,2,1,output);
	}else{
		*id=0;
		fwrite(id,2,1,output);
	}
	free(id);
	if(x.left!=NULL){
		write_node(output,*x.left);
	}
	if(x.right!=NULL){
		write_node(output,*x.right);
	}
}

void writeHeader(FILE * output,node src){
	fseek(output,0,SEEK_SET);
	fwrite("HSMI",1,4,output);
	printf("%d\n",src.id);
	fseek(output,1,SEEK_CUR);
	//et ici on écrit l'arbre
	uint16_t x = htons(src.id);
	printf("%d\n",x);
	fwrite(&x,2,1,output);
	fwrite(&x,2,1,output);
	fwrite(&x,2,1,output);
	char c = 0;
	if(src.lettre == NULL){
		fwrite(&c,1,1,output);
	}else{
		fwrite(src.lettre,1,1,output);
	}
	uint16_t *id = malloc(sizeof *id);
	if(src.left != NULL){
		*id=htons((*src.left).id);
		fwrite(id,2,1,output);
	}else{
		*id=0;
		fwrite(id,2,1,output);
	}
	if(src.right != NULL){
		*id=htons((*src.right).id);
		fwrite(id,2,1,output);
	}else{
		*id=0;
		fwrite(id,2,1,output);
	}
	if(src.left!=NULL){
		write_node(output,*src.left);
	}
	if(src.right!=NULL){
		write_node(output,*src.right);
	}
	free(id);
}
void encode(charCode * tab,int len, FILE * f,FILE * output){
	fseek(f,0,SEEK_SET);
	char c = 0;
	int cc;
	unsigned char u0 = 0 | (1<<0);
	unsigned char u1 = 0 | (1<<1);
	unsigned char u2 = 0 | (1<<2);
	unsigned char u3 = 0 | (1<<3);
	unsigned char u4 = 0 | (1<<4);
	unsigned char u5 = 0 | (1<<5);
	unsigned char u6 = 0 | (1<<6);
	unsigned char u7 = 0 | (1<<7);
	unsigned char bitC = 0;
	unsigned char idx = 0;
	int wrote = 0;
	char * code = calloc(1,sizeof(char));
	//mettre le code dans le fichier de sortie
	while((cc=fgetc(f))!=EOF){
		c=cc;
		for(int i = 0;i< len; i++){
			if(tab[i].c == c){
				code = tab[i].code;
				break;
			}
		}
		//code n'est forcement pas nulle
		for(int j = 0; j<strlen(code);j++){
			switch(idx){
				case 0:
					bitC |= ((code[j]=='1')?u0:bitC);
					break;
				case 1:
					bitC |= ((code[j]=='1')?u1:bitC);
					break;
				case 2:
					bitC |= ((code[j]=='1')?u2:bitC);
					break;
				case 3:
					bitC |= ((code[j]=='1')?u3:bitC);
					break;
				case 4:
					bitC |= ((code[j]=='1')?u4:bitC);
					break;
				case 5:
					bitC |= ((code[j]=='1')?u5:bitC);
					break;
				case 6:
					bitC |= ((code[j]=='1')?u6:bitC);
					break;
				case 7:
					bitC |= ((code[j]=='1')?u7:bitC);
					break;
			}
			if(idx == 7){
				idx = 0;
				fwrite(&bitC,1,1,output);
				bitC=0;
				wrote = 0;
			}else{
				wrote = 1;
				idx ++;
			}
		}
		//free(code);
	}

	//Si a la fin j'ai fini d'écrire mais que j'ai pas encore envoyé alors je le fais
	if(wrote!=0){
		fwrite(&bitC,1,1,output);
		fseek(output,0,SEEK_SET);
		fseek(output,4,SEEK_SET);
		fwrite(&idx,1,1,output);
		printf("aaag %d\n",idx);
	}else{
		fseek(output,4,SEEK_SET);
		idx=8;
		fwrite(&idx,1,1,output);
		printf("aaag %d\n",idx);
	}
}
int main(int argc, char ** argv){
	if(argc<3) {
		fprintf(stderr,"Besoin de 2 arguments in et out\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror ("Error :");
		return -1;
	}
	unsigned char * c = malloc(1*sizeof(char));
	int nb_elem = 0;
	int nb_noeud = 0;
	int size = 1;
	montableau * tab = malloc(1*sizeof(montableau));
	// On lit le fichier et on compte les occurences dans le tableau tab.
	while(read(fd, c, 1)>0) {
		add_tab(&tab, *c, &size, &nb_elem);
	}

	//Maintenant il faut le trier
	qsort(tab, nb_elem, sizeof(montableau), &montableau_cmp);
	int nb_elem2 = nb_elem;
	node * tab_node = malloc(nb_elem*sizeof(node));
	// On initialise le tableau de noeud avec chaque element du tableau de caractere.
	for(int i = 0; i<nb_elem; i++) {
		tab_node[i].id = i+1;
		tab_node[i].lettre = malloc(1);
		*(tab_node[i].lettre) = tab[i].c;
		printf("|%c|\n",*(tab_node[i].lettre));
		tab_node[i].weight = tab[i].v;
		tab_node[i].left = NULL;
		tab_node[i].right = NULL;
	}
	nb_noeud = nb_elem;
	while(nb_elem>1) {
		tab_node = fusionne_deux_premiers(tab_node, &nb_elem, &nb_noeud);
		insert_sort(tab_node, nb_elem);
	}
	//Il faudrait créer un tableau de correspondance charactere/code
	//Normalement apres ca on a l'arbre
	charCode * tabCode = malloc(nb_elem2*sizeof(charCode));
	for(int i = 0; i<nb_elem2; i++){
		tabCode[i].c = tab[nb_elem2-1-i].c;
		tabCode[i].code = getCode(*tab_node,tabCode[i].c);
		printf("[%c] : %s\n",tabCode[i].c,tabCode[i].code);
	}
	char *s;
	close(fd);
	FILE * f =fopen("truc.dot","w");
	createDOT(f,*tab_node);
	fclose(f);
	s = calloc(100,sizeof(char));
	sprintf(s,"dot -Tpng -o %s.png truc.dot",argv[2]);
	system(s);
	remove("truc.dot");
	f = fopen(argv[1],"r");
	if (f == NULL) {
		fprintf(stderr, "Error opening the file\n");
		exit(-1);

	}

	FILE * f2 = fopen(argv[2],"w");
	writeHeader(f2,tab_node[0]);
	encode(tabCode,nb_elem2,f,f2);
	fclose(f);
	fclose(f2);
	free(c);
	for(int i = 0;i<nb_elem2;i++){
		free(tabCode[i].code);
	}
	free(tab_node);
	free(tab);
	free(tabCode);
	free(s);
	return 0;
}
