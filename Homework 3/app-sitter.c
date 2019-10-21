#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

struct task {
    struct task *next;
    char **argv;
    int count;
    pid_t pid;
    time_t start_time;
    time_t end_time;
};

void add_to_list(struct task **head_ptr, char **argv, int count, int starting_index, int ending_index){
    char **add_arguments = malloc((count + 1) * sizeof(*add_arguments));
    for (int j = 0; j < count; j++){
        add_arguments[j] = argv[starting_index + j];
    }
    struct task *node = malloc(sizeof(*node));
    node->argv = add_arguments;
    node->count = count;
    for(int i = 0; i < ending_index - starting_index; i++){
        printf("%s\n",node->argv[i]);
    }
    printf("\n");
    node->next = *head_ptr;
    *head_ptr = node;
}

void print_list(struct task *head) {
    while(head != NULL){
        printf("Arg count: %d\n", head-> count);
        for (int i = 0; i < head->count; i++){
            printf("%s\n", head->argv[i]);
        }
        printf("\n");
        head = head->next;
    }
}

void execute(struct task *head, int total_count){
    while(head != NULL){
        if ((head->pid = fork()) == 0){
            printf("Process %s:\n", head->argv[0]);
            // for (int i = 1; i < head->count; i++){
            //     printf("%s ", head->argv[i]);
            // }
            // printf("\n");
            execvp(head->argv[0], head->argv);
        }
        else if(head->pid > 0){
            printf("Parent process %s: \n", head->argv[0]);
        }
        head = head->next;
    }
    for (int i = 0; i < total_count; i++){
        wait(NULL);
    }
    // while (head != NULL){
    //     printf("Process %s: \nArguments: ", head->argv[0]);
    //     for (int i = 1; i < head->count; i++){
    //         printf("%s\n", head->argv[i]);
    //     }
    //     printf("\n");
    //     head = head->next;
    // }
}

int main(int argc, char **argv){
    struct task *head = NULL;
    char **all_arguments = malloc((argc + 1) * sizeof(*all_arguments));
    int starting_index = 0;
    int ending_index = 0;
    int arg_count = 0;
    int total_count = 0;
    for (int i = 1; i <= argc; i++){
        if (i != argc && strcmp(argv[i], ".") != 0){
            all_arguments[ending_index] = argv[i];
            ending_index++;
            arg_count++;
        }
        else{
            if(arg_count != 0){
                add_to_list(&head, all_arguments, arg_count, starting_index, ending_index);
                total_count++;
                starting_index = ending_index;
                arg_count = 0;
            }
        }
    }
    printf("-----------------------------PRINT LIST-----------------------------\n");
    print_list(head);
    printf("------------------------------EXECUTE--------------------------------\n");
    execute(head, total_count);
    return 0;
}