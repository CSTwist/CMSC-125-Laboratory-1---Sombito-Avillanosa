# -CMSC-125-Laboratory-1---Sombito-Avillanosa

This repo contains implementation of Lab 1 for CMSC 125 Operating Systems, where we focus on process management and I/O redirection using the POSIX API in C.

The goal of this lab activity is to build a simplified Unix shell, called "mysh" to demonstrate how OS create and manage process, execute programs, handle input/output redirection, background execution.

The shell aims to demonstrate:
- Process creation using fork()
- Program execution using execvp()
- Parent-child process sync using wait() and waitpid()
- Input and output redirection using file descriptors
- Background job execution
- Cleanup of zombie processes

Features(Planned/In progress)
1. Interactive Shell
- Displaying a prompt (mysh >)
- Keep accepting and executing user commands
2. Built-in commands
- Implemented directly in the shell with no fork: (exit, cd <directory>, pwd)
3. External Command Execution
- Execute non-built in commands using: fork(), execvp(), wait()/waitpid()
4. I/O redirection
- Supports Output redirection: >
- Output append: >>
- Input redirection: <
- Combined redirection: < input.txt > output.txt
5. Background Execeution
- Supports commands ending with &
- Shell remains responsive
- Background jobs tracked to prevent zombie processes

Project Structure (In-progress)
- mysh.c
- mysh.h
- mysh.o
- README.md
- Makefile

This repo structure may evolve as implementation progress.

Command Representation (suggested template)
1 typedef struct {
2   char * command ; // Command name
3   char * args [256]; // Arguments (NULL - terminated )
4   char * input_file ; // For < redirection ( NULL if none )
5   char * output_file ; // For > or >> redirection ( NULL if none )
6   bool append ; // true for >>, false for >
7   bool background ; // true if & present
8 } Command ;

Testing:
- Built-in commands
- External commands
- Invalid commands
- I/O redirection edge cases
- Background job execution
- Zombie process cleanup
- Behavior comparison with bash

