#include <stdio.h>
int main(int argc, char **argv){
    unsigned char x = 1 <<6;
    FILE * f = fopen("test","w");
    fwrite(&x,1,1,f);
    fclose(f);
}
