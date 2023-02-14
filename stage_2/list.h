#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int aa_launch(char **args);
int aa_execute(char** args);
int aa_help(char **args);
int aa_exit(char** args);
int aa_num_builtin();
int aa_execute(char **args);
char **aa_split_line(char *line);
char *aa_read_line();
void aa_loop();