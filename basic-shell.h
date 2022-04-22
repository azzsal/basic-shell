#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

void quit(FILE *stream, char *line, char **path_list, int status);

int tokenize_cmds_string(char *line, char ***path_list);

int execute_cmd(char *full_cmd, char ***path_list);

char *trim(char *str);

char **construct_path(char *new_path_list);

void free_path(char **path);

char *get_path(char *cmd, char **path_list);

char *get_output_redirection_stream(char **cmd);

char **get_argv(char *cmd_path, char *full_cmd);
