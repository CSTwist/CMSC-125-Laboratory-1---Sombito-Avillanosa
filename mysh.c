#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "parser.h"
#include "command.h"

int main() {
    while (1) {
        char line[256];

        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            //EOF
            printf("\n");
            break;
        }

        //Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        //Ignore empty input
        if (line[0] == '\0') {
            continue;
        }

        char *cmd_input = strdup(line); //Pass copy
        if (cmd_input == NULL) {
            perror("strdup");
            continue;
        }
        Command cmd = parse_command(cmd_input); //Parser
                                            //Interpreter

        //If typed only spaces or special tokens, cmd.command may be NULL
        if (cmd.command == NULL) {
            free(cmd_input); //Free memory if command is invalid
            continue;
        }

        //Built-in commands
        if (strcmp(cmd.command, "exit") == 0) {
            free(cmd_input); //Cleanup 
            break;
        }

        if (strcmp(cmd.command, "cd") == 0) {
            if (cmd.args[1] != NULL) {
                if (chdir(cmd.args[1]) != 0) {
                    perror("cd");
                }
            } else {
                fprintf(stderr, "cd: missing argument\n");
            }
            free(cmd_input); //Cleanup
            continue;
        }

        if (strcmp(cmd.command, "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("pwd");
            }
            free(cmd_input); //Cleanup
            continue;
        }

        //External commands
        //fork + execvp + waitpid
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            free(cmd_input); //Cleanup
            continue;
        }

        if (pid == 0) {
            //Child process: run the command
            execvp(cmd.command, cmd.args);

            //If execvp returns, it failed
            if (errno == ENOENT) {
                fprintf(stderr, "mysh: command not found: %s\n", cmd.command);
                exit(127);
            } else {
                perror("execvp");
                exit(1);
            }
        } else {
            //Parent process: wait for child
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            } else {
                if (WIFEXITED(status)) {
                    int code = WEXITSTATUS(status);
                    if (code != 0) {
                    }
                }
            }
        }

        free(cmd_input); //Note: Remove this line at the implementation of background jobs.
    }

    return 0;
}