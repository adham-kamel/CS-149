#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct task {
    struct task *next;
    char **argv;
    pid_t pid;
    time_t start_time;
    time_t end_time;
};

void add_to_list(struct task **head_ptr, char **argv){
    struct task *node = malloc(sizeof(*node));
    node->argv = argv;
    node->next = *head_ptr;
    *head_ptr = node;
}

void print_list(struct task *head, int count) {
    while(head != NULL){
        for (int i = 0; i < count; i++){
            printf("%s\n", head->argv[i]); // causes seg fault
            head = head->next;
        }
    }
}

int main(int argc, char **argv){
    struct task *head = NULL;
    char **all_arguments = malloc(sizeof(**all_arguments));
    int starting_index = 0;
    int ending_index = 0;
    int arg_count = 0;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], ".") != 0){
            //printf("%s\n", argv[i]);
            all_arguments[ending_index] = argv[i];
            ending_index++;
        }
        else{
            char **add_arguments = malloc(sizeof(**add_arguments));
            for (int j = 0; j < ending_index - starting_index; j++){
                add_arguments[j] = all_arguments[starting_index + j];
                printf("%s\n", add_arguments[j]);
            }
            starting_index = ending_index;
            add_to_list(&head, add_arguments);
            arg_count++;
        }
    }
    print_list(head, arg_count);
    return 0;
}