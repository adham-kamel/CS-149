#include <stdio.h>

int main(int argc, char **argv) {
    double a;
    double sum = 0;
    char  c;
    char *filename = argv[1];
    FILE *file;
    file = fopen(filename, "r");
    if(!file){
        return 0;
    }
    while(fscanf(file, "%le", &a) == 1){
        sum += a;
    }
    printf("Sum: %f\n", sum);
    fclose(file);

    return 0;

}