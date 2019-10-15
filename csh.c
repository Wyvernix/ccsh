#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Will update the CWD and replace the home directory with tilde
void updateCWD(char *buf, int size) {
  getcwd(buf, size);
  char* home = getenv("HOME");
  char* match = strstr(buf, home);
  if (match) {
    sprintf(buf, "~%s", buf + strlen(home));
  }
}

int main(int argc, char* argv[]) {
  // Variables
  char cmd[64];
  char params[512];
  char cwd[1024];
  char linein[576];

  // Main loop
  do {
    updateCWD(cwd, sizeof(cwd));
    // Print prompt
    printf("[%s]$ ", cwd);
    // Scan input
    fgets(linein, 512, stdin);
    sscanf(linein, "%63s %511[^\n]", cmd, params);

    if (argc > 1 && strcmp(argv[1], "debug") == 0) {
      printf("[%s]\n[%s]\n", cmd, params); //debug
    }

    // Shell command overrides
    if (strcmp("cd", cmd) == 0) {
      chdir(params);
    } else {
      system(linein);
    }
  } while (strcmp("exit", cmd) != 0);

  return 0;
}
