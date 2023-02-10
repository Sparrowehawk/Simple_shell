// Since abreviating simple shell can be problamatic, I will call this this Almost-everything-else-is-worse-than Alacritty, or aa for short

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int aa_cd(char** args);
int aa_help(char **args);
int aa_exit(char** args);
int aa_getpath(char** args);
int aa_setpath(char** args);



char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "getpath",
    "setpath"
};

int(*builtin_function[])(char**) = {
    &aa_cd,
    &aa_help,
    &aa_exit,
    &aa_getpath,
    &aa_setpath,
};

int aa_setpath(char** args){
    if(args[1] == NULL){
        fprintf(stderr, "aa: expected argument, non given\n");
    } else if (args[2] != NULL){
        fprintf(stderr, "aa: too many arguments parsed\n");
    } else { 
        char* previous_destination = getenv("PATH");
        char* destination = args[1];
        setenv("PATH", destination, 1);

        if(strcmp(previous_destination, getenv("PATH")) != 0){
            printf("aa: Path changed successfully\n");
        } else {
            printf("aa: Path didn't change due to error\n");
        }
    }

    return 1;
}


int aa_getpath(char** args){
    char* path = getenv("PATH");
    printf("Current path is: %s\n", path);

    return 1;
}

int aa_cd(char **args){
    if(args[1] == NULL){
        chdir(getenv("HOME"));
    } else{
        if(chdir(args[1]) != 0){
            perror("aa");
        }
    }
    return 1;
}

int aa_num_builtin(){
    return sizeof(builtin_str) / sizeof(char*);
}

int aa_help(char **args){
    printf("Hamid Jamal's simple shell, acronymed as aa\n");
    printf("Type program names followed by help for further info\n");
    printf("The following are said commands\n");

    for (int i = 0; i < aa_num_builtin  (); i++){
        printf("   %s\n", builtin_str[i]);
    }

    return 1;
}

int aa_exit(char **args){
    return 0;
    exit(1);
}

int aa_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0){
        if(execvp(args[0], args) == -1){
            // printf("Error with %s", args); who knows how this is supposed to work
            perror("aa");
        } 
        exit(EXIT_FAILURE);
    } else if(pid < 0 ){
        perror("aa");
    } else {
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int aa_execute(char **args){
    if(args[0] == NULL){
        return 1;
    }

    for(int i = 0; i < aa_num_builtin(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_function[i])(args);
        }
    }

    return aa_launch(args);
}


#define token_buffer_size 64
#define token_delim " \t\r\n\a"

char **aa_split_line(char *line){
    int buffer_size = token_buffer_size;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "aa: allocation error \n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, token_delim);
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= buffer_size){
            buffer_size += token_buffer_size;
            tokens = realloc(tokens, buffer_size * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "aa: allocation error \n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, token_delim);
    }
    tokens[position] = NULL;
    return tokens;
}


char *aa_read_line(){
    char *line = NULL;
    ssize_t buffer_size = 0; // getline does buffer instead
    
    if(getline(&line, &buffer_size, stdin) == -1){
        if(feof(stdin)){
            exit(EXIT_SUCCESS);
        } else{
            perror("readline");
            exit(EXIT_SUCCESS);
        }
    }

    return line;
}

// #define read_line_buffer 512 // if getline is not allowed

// char *aa_read_line(){ //IN CASE getline IS NOT ALLOWED
//     int buffer_size = read_line_buffer;
//     int position = 0;
//     char *buffer = malloc(sizeof(char) * buffer_size);
//     int character;
    
//     if(!buffer){
//         fprintf(stderr, "aa: allocation error\n");
//         exit(EXIT_FAILURE);
//     }

//     while(1){
//         character = getchar();

//         if(character == EOF || character == '\n'){
//             buffer[position] = '\0';
//             return buffer;
//         } else{
//             buffer[position] = character;
//         }

//         position++;

//         if(position >= buffer_size){
//             buffer_size += read_line_buffer;
//             buffer = realloc(buffer, buffer_size);
//             if(!buffer){
//                 fprintf(stderr, "aa: allocation error \n");
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }
// }

void aa_loop(){
    char *line;
    char **args;
    int status;

    do{
        printf("|>: ");
        line = aa_read_line();
        args = aa_split_line(line);
        status = aa_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char** argv){

    char* path_env = getenv("PATH");
    char* home_env = getenv("HOME");

    chdir(home_env);

    aa_loop();

    chdir(path_env);


    return EXIT_SUCCESS;

}