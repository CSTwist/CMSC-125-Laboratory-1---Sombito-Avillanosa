#include "parser.h"
#include "command.h"
#include <string.h>
#include <stdio.h>

Command parse_command(char *input) {
    char *command = strtok(input, " \t\n");
    Command cmd = {0};
    cmd.command = command;
    int i = 0;

    while (command != NULL && i < 255) {
        if (strcmp(command, "<") == 0) {
            cmd.input_file = strtok(NULL, " ");
        } else if (strcmp(command, ">") == 0) {
            cmd.output_file = strtok(NULL, " ");
            cmd.append = false;
        } else if (strcmp(command, ">>") == 0) {
            cmd.output_file = strtok(NULL, " ");
            cmd.append = true;
        } else if (strcmp(command, "&") == 0) {
            cmd.background = true;
        }
        else{
            cmd.args[i++] = command;
        }
        command = strtok(NULL, " ");
    }
    cmd.args[i] = NULL; //Null-terminate

    return cmd;
}