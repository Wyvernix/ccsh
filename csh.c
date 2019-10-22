#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char* home;
char cwd[1024];
char prompt[512];

// Will update the CWD and replace the home directory with tilde
void updateCWD() {
  getcwd(cwd, sizeof(cwd));
  // If cwd is a subdirectory of home, replace home with ~ in the prompt
  home = getenv("HOME");
  char *match = strstr(cwd, home);
  if (match) {
    sprintf(prompt, "~%s", cwd + strlen(home));
  }
}

int cp(char *params) {
  if (strlen(params) == 0) {
    printf("cp: missing file operand\n");
    return 1;
  }

  char *source, *dest;

  source = strtok(params, " ");
  dest = strtok(NULL, " ");
  if (dest == NULL) {
    printf("cp: missing destination file operand after '%s'\n", source);
    return 1;
  }

  printf("copy %s to %s!\n", source, dest);
  FILE *s_file = fopen(source, "r");
  if (s_file == NULL) {
    printf("cp: cannot read '%s'\n", source);
    return 1;
  }

  FILE *d_file = fopen(dest, "w");
  if (d_file == NULL) {
    printf("cp: cannot write '%s'\n", dest);
    fclose(s_file);
    return 1;
  }

  int bits = fgetc(s_file);
  while(bits != EOF) {
    fputc(bits, d_file);
    bits = fgetc(s_file);
  }

  fclose(d_file);
  fclose(s_file);
  return 0;
}

// Returns 1 if no matching commands
int try_bash_commands(char *cmd, char *params) {
  if (strcmp("cd", cmd) == 0) {
    if (strlen(params) == 0) {
      chdir(home);
    } else {
      chdir(params);
    }
    updateCWD();

  } else if (strcmp("pwd", cmd) == 0) {
    printf("%s\n", cwd);

  } else if (strcmp("ls", cmd) == 0) {
    struct dirent **namelist;
    int n;
    int i = 0;

    n = scandir(".", &namelist, NULL, alphasort);
    while (i < n) {
      printf("%s\n", namelist[i]->d_name);
      i++;
    }
    free(namelist);

  } else if (strcmp("cp", cmd) == 0) {
    cp(params);
  } else {
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  // Variables
  char cmd[64];
  char params[512];
  char linein[576];

  char* user = getenv("USER");
  char hostname[64];
  gethostname(hostname, sizeof(hostname));
  updateCWD();

  // Main loop
  do {
    // Print prompt
    printf("%s@%s:%s$ ", user, hostname, prompt);
    // Scan input
    fgets(linein, 512, stdin);
    int matches = sscanf(linein, "%63s %511[^\n]", cmd, params);
    if (matches < 2) {
      params[0] = '\0';
    }

    if (argc > 1 && strcmp(argv[1], "debug") == 0) {
      printf("[%s]\n[%s]\n", cmd, params); //debug
    }

    // Shell command overrides
    if (try_bash_commands(cmd, params)) {
      printf("Run: %s\n", linein);
      //system(linein);
    }
  } while (strcmp("exit", cmd) != 0);

  return 0;
}
