// Since abreviating simple shell can be problamatic, I will call this this Almost-everything-else-is-worse-than Alacritty, or aa for short
#include "list.h"
#include "list.c"
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){

    char* path_env = getenv("PATH");
    char* home_env = getenv("HOME");
    // char* root_env = getenv("ROOT");

    chdir(home_env);

    aa_loop();

    chdir(path_env);


    return EXIT_SUCCESS;

}