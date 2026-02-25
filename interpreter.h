#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "command.h"

void check_background_jobs();

int execute_command(Command cmd);

#endif // INTERPRETER_H