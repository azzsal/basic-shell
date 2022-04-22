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
      printf("%s: file not found\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  else {
    printf("Too many arguments provided\n");
    exit(EXIT_FAILURE);
  }

  char **path_list = construct_path("/bin");

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

void quit(FILE *stream, char *line, char **path_list, int status) {
      fclose(stream);
      free(line);
      free_path(path_list);
      exit(status);
}

int tokenize_cmds_string(char *line, char ***path_list) {
  char *full_cmd = strsep(&line, "&");
  // base case (only one command is present)
  if(line == NULL)
    return execute_cmd(full_cmd, path_list);
  int status = 1;
  int rc = fork();
  if(rc < 0) {
    return 0;
  } else if(rc == 0) {
    execute_cmd(full_cmd, path_list);
    return 0;
  } else {
    line = trim(line);
    status = tokenize_cmds_string(line, path_list);
    wait(NULL);
    return status;
  }
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
      free_path(*path_list);
      *path_list = construct_path(cmd_args);
      return 1;
  } else {
      // executing non-built-in commands
      char *cmd_path = get_path(cmd, *path_list);
      if(strlen(cmd_path) > 0) {
        char *output_stream = get_output_redirection_stream(&full_cmd);
        char **argv = get_argv(cmd_path, full_cmd);
        if(argv == NULL)
          return 0;

        int rc = fork();
        if(rc < 0) {
          free(argv[0]);
          free(argv);
        } else if(rc == 0) {
          if(output_stream != NULL) {
            if(output_stream[0] == '\0') {
              printf("specify a file for output redirection\n");
              return 1;
            }
            close(STDOUT_FILENO);
            open(output_stream, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
          }
          execv(argv[0], argv);
          exit(EXIT_SUCCESS); 
        } else {
          wait(NULL);
        }
        return 1;

      } else {
        printf("%s: command not found\n", cmd);
      }
  }

}

char **get_argv(char *cmd_path, char *full_cmd) {
    char **argv = calloc(32 + 1, sizeof(char *));
    if (argv == NULL) {
        return NULL;
    }

    argv[0] = cmd_path;

    if (full_cmd == NULL || *full_cmd == '\0') {
        return argv;
    }

    full_cmd = trim(full_cmd);
    
    for (int i = 1; i < 32; i++) {
        argv[i] = strsep(&full_cmd, " \t\n\r");
        if (argv[i] == NULL) {
            break;
        }
    }
    return argv;
}

char *get_output_redirection_stream(char **cmd) {
  char *temp = *cmd;
  *cmd = strsep(&temp, ">");
  if(temp != NULL)
    temp = trim(temp);
  char *output_stream = strsep(&temp, "\n\r\t");
  return output_stream;
}

char *trim(char *str) {
    size_t len = strlen(str);
    while(isspace(str[len - 1])) --len;
    while(*str && isspace(*str)) ++str, --len;
    return strndup(str, len);
}

char **construct_path(char *args) {
    char **new_path_list = calloc(32, sizeof(char *)); 
    char *token;                 
    int i = 0;
    while(1) {
      token = strsep(&args, " \n\r\t"); 
      if(token == NULL)
        break;
      new_path_list[i] = malloc(33 * sizeof(char));
      strncpy(new_path_list[i], token, 31);
      i++;  
    }
    return new_path_list;
}

void free_path(char **path) {
    for (size_t i = 0; i < 32; i++) {
        free(path[i]);
    }
    free(path);
}

char *get_path(char *cmd, char **path_list) {
  char *cmd_path = calloc(1, strlen(cmd) + 33);
  for(size_t i = 0; path_list[i] != NULL; i++) {
    strncpy(cmd_path, path_list[i], 32);
    strcat(cmd_path, "/");
    strcat(cmd_path, cmd);
    if(access(cmd_path, X_OK) == 0)
      return cmd_path;
  }
  return "";
}
