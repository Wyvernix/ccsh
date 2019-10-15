#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void updateCWD(char *buf, int size) {
  getcwd(buf, size);
  char* home = getenv("HOME");
  char* match = strstr(buf, home);
  if (match) {
    sprintf(buf, "~%s", buf + strlen(home));
  }
}

int main() {
  // Variables
  char cmd[64];
  char params[512];
  char cwd[1024];
  char out[576];
  updateCWD(cwd, sizeof(cwd));

  // Main loop
  do {
    printf("[%s]$ ", cwd);
    scanf("%63s", cmd);
    fgets(params, 511, stdin);
    sprintf(out, "%s %s", cmd, params);
    //debug
    printf("[%s]\n[%s]\n", cmd, params);
    //end
    if (strcmp("cd", cmd) == 0) {
      chdir(params);
    } else {
      system(out);
    }
  } while (strcmp("exit", cmd) != 0);

  return 0;
}
