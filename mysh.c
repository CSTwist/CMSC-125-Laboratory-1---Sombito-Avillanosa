#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "command.h"

int main() {
    
    while (1) {
        char command[256];
        printf("mysh> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Exit on EOF
        }
        // Remove newline character from the command
        command[strcspn(command, "\n")] = 0;

        Command parsed_command = parse_command(command);
        if (strcmp(parsed_command.command, "exit") == 0) {
            break; // Exit the shell
        }
        if (strcmp(parsed_command.command, "cd") == 0) {
            if (parsed_command.args[1] != NULL) {
                if (chdir(parsed_command.args[1]) != 0) {
                    perror("chdir");
                }
            } else {
                fprintf(stderr, "cd: missing argument\n");
            }
            continue;
        }
        if (strcmp(parsed_command.command, "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd");
            }
            continue;
        }

        // Execute the command
        int ret = system(parsed_command.command);
        if (ret == -1) {
            perror("system");
        }
    }
    return 0;
}