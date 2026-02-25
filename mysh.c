#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "command.h"
#include "interpreter.h"

int main() {
    while (1) {
        // Clean up zombies
        check_background_jobs();

        char line[256];

        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        // Ignore empty input
        if (line[0] == '\0') {
            continue;
        }

        char *cmd_input = strdup(line); // Pass copy
        if (cmd_input == NULL) {
            perror("strdup");
            continue;
        }
        
        Command cmd = parse_command(cmd_input);

        if (cmd.command != NULL) {
            int should_continue = execute_command(cmd);
            
            if (should_continue == 0) {
                free(cmd_input); // Cleanup before breaking
                break;           // Exit the shell
            }
        }

        free(cmd_input);
    }

    return 0;
}