// Since abreviating simple shell can be problamatic, I will call this this Almost-everything-else-is-worse-than Alacritty, or aa for short
#include "list.h"
#include "list.c"
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv){

    aa_loop();

    return EXIT_SUCCESS;

}