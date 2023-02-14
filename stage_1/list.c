#include "list.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *builtin_str[] = {
    "help",
    "exit",
};

int(*builtin_function[])(char**) = {
    &aa_help,
    &aa_exit,
};

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
    exit(1);
    return 0;
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

    printf("Cannot execute external commands yet\n");

    return 1;
}


#define token_buffer_size 64
#define token_delim " \t\r\n\a"

char **aa_split_line(char *line){
    int buffer_size = token_buffer_size;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    if(!token){
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

// char *aa_read_line(){ //IN CASE getline IS NOT ALLOWED, also doesnt work
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