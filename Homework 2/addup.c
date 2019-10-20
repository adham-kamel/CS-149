#include <stdio.h>
#include <unistd.h>

double addup(char* filein);

int main(int argc, char **argv) {
    double total = 0;
    double total_sum = 0;
    pid_t pid;
    int sum_pipe[2];
    pipe(sum_pipe);
    for (int i = 1; i < argc; i++){
        pid = fork();
        if (pid == 0){
            total_sum += addup(argv[i]);
            write(sum_pipe[1], &total_sum, sizeof(total_sum));
            return 0;
        }
    }
    // Parent process
    for (int i = 1; i < argc; i++){
        read(sum_pipe[0], &total_sum, sizeof(total_sum));
        total += total_sum;
    }
    printf("%f\n", total);
    return 0;
}

double addup(char* filein){
    double a;
    double sum = 0;
    char *filename = filein;
    FILE *file;
    file = fopen(filename, "r");
    if(!file){
        return 0;
    }
    while(fscanf(file, "%le", &a) == 1){
        sum += a;
    }
    fclose(file);
    return sum;
}
