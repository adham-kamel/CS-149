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
    time_t time;
    int fd_out;
    int fd_err;
};

void add_to_list(struct task **head_ptr, char **argv, int count, int starting_index, int ending_index, int command_number){
    char **add_arguments = malloc((count + 1) * sizeof(*add_arguments));
    for (int j = 0; j < count; j++){
        add_arguments[j] = argv[starting_index + j];
    }
    add_arguments[count] = NULL;

    char out[10];
    int fd1 = command_number;
    sprintf(out, "%d.out", fd1);

    char err[10];
    int fd2 = command_number;
    sprintf(err, "%d.err", fd2);

    FILE* out_file = fopen(out, "w+");
    int fd_out = fileno(out_file);

    FILE* err_file = fopen(err, "w+");
    int fd_err = fileno(err_file);

    struct task *node = malloc(sizeof(*node));
    node->argv = add_arguments;
    node->count = count;
    node->next = *head_ptr;
    node->fd_out = fd_out;
    node->fd_err = fd_err;
    *head_ptr = node;
}

void execute(struct task *head){
    while(head != NULL){
        if ((head->pid = fork()) == 0){
            dup2(head->fd_out, 1);
            dup2(head->fd_err, 2);
            int rc = execvp(head->argv[0], head->argv);
            if (rc == -1){
                perror(head->argv[0]);
            }
        }
        head->start_time = time(NULL);
        head = head->next;
    }
}

void time_taken(struct task *head, int *count, pid_t cpid, time_t end_time){
    while(head != NULL){
        if (head->pid == cpid){
            head->time = (end_time - head->start_time);
            dup2(head->fd_out, 1);
            dup2(head->fd_err, 2);
            if (head->time > 2){
                (*count)++;
                if ((head->pid = fork()) == 0){
                    dup2(head->fd_out, 1);
                    dup2(head->fd_err, 2);
                    int rc = execvp(head->argv[0], head->argv);
                    if (rc < 0){
                        perror(head->argv[0]);
                    }
                }
                head->start_time = time(NULL);
            }
            else{
                free(head->argv);
                fprintf(stderr, "spawning too fast\n");
            }
            break;
        }
        head = head->next;
    }
}

int main(int argc, char **argv){
    struct task *head = NULL;
    char **all_arguments = malloc((argc + 1) * sizeof(*all_arguments));
    int starting_index = 0;
    int ending_index = 0;
    int arg_count = 0;
    int total_count = 0;
    int command_number = 0;
    for (int i = 1; i <= argc; i++){
        if (i != argc && strcmp(argv[i], ".") != 0){
            all_arguments[ending_index] = argv[i];
            ending_index++;
            arg_count++;
        }
        else{
            if(arg_count != 0){
                total_count++;
                if (total_count != 1){
                    command_number++;
                }
                add_to_list(&head, all_arguments, arg_count, starting_index, ending_index, command_number);
                starting_index = ending_index;
                arg_count = 0;
            }
        }
    }
    execute(head);
    for (int i = 0; i < total_count; i++){
        int stat;
        pid_t cpid = wait(&stat);
        time_t end_time = time(NULL);
        time_taken(head, &total_count, cpid, end_time);
        if (WIFEXITED(stat)){
            fprintf(stderr, "exited rc = %d\n", WEXITSTATUS(stat));
        }
        if (WIFSIGNALED(stat)){
            fprintf(stderr, "signal %d\n", WTERMSIG(stat));
        }
    }
    return 0;
}