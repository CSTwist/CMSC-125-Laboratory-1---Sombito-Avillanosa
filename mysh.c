#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    
    while (1) {
        char command[256];
        printf("mysh> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Exit on EOF
        }
        // Remove newline character from the command
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            break; // Exit the shell
        }

        // Execute the command
        int ret = system(command);
        if (ret == -1) {
            perror("system");
        }
    }
    return 0;
}