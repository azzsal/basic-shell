#include "basic-shell.h"

int main(int argc, char *argv[])
{
  printf("\033[0;31m");
  FILE *stream;
  // interactive mode
  if(argc == 1) {
    stream = stdin;
  }
  // batch mode
  else if(argc == 2) {
    stream = fopen(argv[1], "r");
    if(stream == NULL) {
      printf("%s: Not Found\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  else {
    printf("Too many arguments provided\n");
    exit(EXIT_FAILURE);
  }

  char **path_list = init_path();

  char *line = NULL;  
  size_t line_len = 0;

  while(1) {
    if(argc == 1) 
      printf("basic-shell> ");

    if(getline(&line, &line_len, stream) > 0) {
      if(tokenize_cmds_string(line, &path_list))
        continue;
      quit(stream, line, path_list, EXIT_SUCCESS);
    }
    else if(feof(stream)) {
      quit(stream, line, path_list, EXIT_SUCCESS);
    }
  }
}

char **init_path(void) {
    char **path = malloc(32 * sizeof(char *));
    path[0] = malloc((strlen("/bin") + 1) * sizeof(char));
    strcpy(path[0], "/bin");
    return path;
}

void quit(FILE *stream, char *line, char **path_list, int status) {
      fclose(stream);
      free(line);
      for(int i = 0; i < 32; i++)
        free(path_list[i]);
      free(path_list);
      exit(status);
}

int tokenize_cmds_string(char *line, char ***path_list) {
  char *full_cmd = strsep(&line, "&");
  // base case (only one command is present)
  if(line == NULL)
    return execute_cmd(full_cmd, path_list);
  // TODO (the recursive part)
  return 0;
}

int execute_cmd(char *full_cmd, char ***path_list) {
  full_cmd = trim(full_cmd);
  if(*full_cmd == '\0')
    return 1;
  char *cmd = strsep(&full_cmd, " \n\r\t");
  char *cmd_args = full_cmd;
  
  if(strcmp(cmd, "exit") == 0) {
    return 0;
  } else if(strcmp(cmd, "cd") == 0) {
    if(chdir(cmd_args) == -1) {
      printf("Unable to change directory\n");
    }
    return 1;
  } else if(strcmp(cmd, "path") == 0) {
      // TODO 
  } else {
      // TODO: executing non-built-in commands
  }

}

char *trim(char *str) {
    size_t len = strlen(str);
    while(isspace(str[len - 1])) --len;
    while(*str && isspace(*str)) ++str, --len;
    return strndup(str, len);
}


