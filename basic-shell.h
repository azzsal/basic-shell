#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

char **init_path(void);

void quit(FILE *stream, char *line, char **path_list, int status);

int tokenize_cmds_string(char *line, char ***path_list);

int execute_cmd(char *full_cmd, char ***path_list);

char *trim(char *str);