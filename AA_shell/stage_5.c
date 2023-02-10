// Since abreviating simple shell can be problamatic, I will call this this Almost-everything-else-is-worse-than Alacritty, or aa for short

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv);
void aa_loop(void);
char *aa_read_line();
char **aa_split_line(char *line);
int aa_launch(char **args);
int aa_execute(char **args);
int aa_cd(char** args);
int aa_help(char **args);
int aa_exit(char** args);
int aa_getpath(char** args);
int aa_setpath(char** args);
int aa_history(char **args);
char *history[19];


char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "getpath",
    "setpath",
    "history",
};


int(*builtin_function[])(char**) = {
    &aa_cd,
    &aa_help,
    &aa_exit,
    &aa_getpath,
    &aa_setpath,
    &aa_history,
};


int aa_load_history(void){
    FILE *save_file;
    char* individual_line = NULL;
    size_t len = 0;
    ssize_t read;

    save_file = fopen(".hist_list", "r");
    if(save_file == NULL){
        printf("aa: No file found, starting with new history\n");
        return 1;
    }

    while ((read = getline(&individual_line, &len, save_file)) != -1){
        printf("%s", individual_line);
    }

    fclose(save_file);
    if(individual_line){
        free(individual_line);
    }

    return 1;
}

int aa_save_history(void){
    //
}

int aa_history(char** args){
    if(args[1] != NULL){
        fprintf(stderr, "aa: too many arguments parsed\n");
    } else {
        int i = 0;

        do{
            printf("%d. %s\n", i+1, history[i]);
            i++;
        } while(history[i]);
    }

    return 1;
}

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
        } else{
            chdir(*args);
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

int aa_execute(char **args) {

    if(args[0] == NULL){
       return 1;
    }

    int history_size = 20;
    int history_position = 0;
    for (int i = 0; args[i]; i++) {
        history_position++;
    }

    char* command = calloc(history_position + 1, sizeof(char*));
    if (!command) {
        perror("aa: Error allocating memory for command\n");
        return 1;
    }

    strcpy(command, args[0]);
    for (int i = 1; args[i]; i++) {
        strcat(command, " ");
        strcat(command, args[i]);
    } //combines the comand and the args into one string 


    //start of invoke prev

    // Fun thing, args[i][j] the value as the ascii value so heres a nice key:
    // ! = 33
    // 0 = 48
    // 1 = 49
    // ...
    // 9 = 57
 

    if (args[0][0] == 33){ 

        int is_negative_number = 0; // So, as we are dealing with ascii, if it is neg then args[0][1] is 45 with args [0][2] 
                                // and args[0][3] are the numbers, which is perfectly one too much

        if(args[1] != NULL){
            fprintf(stderr, "aa: Too many arguments parsed\n");
            return 1;
        }

        if(args[0][1] == 45){
            is_negative_number = 1;
        }

        if(args[0][1] == 33){
            int increment = 0;

            while(history[increment]){
                increment++;
            }

            int last_element =  increment-1; 

            if((last_element >= 0) && (last_element <= 19)) { //catch errors    
                char** args = aa_split_line(history[last_element]);
                return aa_execute(args); //probs memory leak but yay it worked
            } else{
                fprintf(stderr, "aa: Error, no history provided\n");
                return 1;
            }


        } else {
            int first_digit_ascii = args[0][1 + is_negative_number];

            int first_digit = first_digit_ascii - 48;

            if((first_digit >= 0) && (first_digit <=9)){

                int second_digit_ascii = args[0][2 + is_negative_number];
                int second_digit = second_digit_ascii - 48;

                if((second_digit >= 0) && (second_digit <=9)){
                    
                    char num1[20];
                    char num2[20];

                    sprintf(num1, "%d", first_digit);
                    sprintf(num2, "%d", second_digit);

                    strcat(num1, num2);

                    int combined = atoi(num1);

                    int element_position = combined - 1;

                    if((element_position >= 0) && (element_position <= 19)) { 
                        if(history[element_position]){
                            char** args = aa_split_line(history[element_position]);
                            return aa_execute(args);
                        } else {
                            fprintf(stderr, "aa: Error, something isn't working\n");
                        }
                    } else {
                        fprintf(stderr, "aa: Out of range value\n");
                        return 1;
                    }

                } else{
                    int element_position = first_digit - 1;

                    if(history[element_position]){
                        char** args = aa_split_line(history[element_position]);
                        return aa_execute(args);
                    } else {
                        fprintf(stderr, "aa: Error, something isn't working\n");
                        return 1;
                    }
                }
            } else{
                fprintf(stderr, "aa: No arguments parsed\n");
                return 1;
            }
        }
    } else{ // end of invoke prev
        for (int j = 0; j < history_size; j++) {
            if (!history[j]) {
                history[j] = command;
                break;
            }

            if (j == history_size - 1) {
                char* temp = history[0];
                for (int k = 0; k < history_size - 1; k++) {
                    history[k] = history[k + 1];
                }
                history[history_size - 1] = command;
                free(temp);
            }
        }
    }

    for (int i = 0; i < aa_num_builtin(); i++) {
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