#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "interpreter.h"
#include "command.h"

#define MAX_JOBS 100

// Background job tracking
int next_job_id = 1;
pid_t bg_pids[MAX_JOBS];
int bg_job_ids[MAX_JOBS];
int bg_count = 0;

static void remove_bg_job(int index) {
    // swap with last to remove (replaced shifting down loop)
    bg_pids[index] = bg_pids[bg_count - 1];
    bg_job_ids[index] = bg_job_ids[bg_count - 1];
    bg_count--;
}

void check_background_jobs() {
    while (1) {
        int status;
        pid_t done = waitpid(-1, &status, WNOHANG);

        if (done > 0) {
            // find job in list
            for (int i = 0; i < bg_count; i++) {
                if (bg_pids[i] == done) {
                    printf("\n[Job %d] %d completed\n", bg_job_ids[i], (int)done);
                    remove_bg_job(i);
                    break;
                }
            }
            continue; // keep looping to reap more finished children
        }

        if (done == 0) {
            break; // none finished
        }

        // done == -1
        if (errno == ECHILD) {
            break; // no child processes
        }
        perror("waitpid");
        break;
    }
}

int execute_command(Command cmd) {
    // Built-in commands
    if (strcmp(cmd.command, "exit") == 0) {
        return 0;
    }

    if (strcmp(cmd.command, "cd") == 0) {
        if (cmd.args[1] != NULL) {
            if (chdir(cmd.args[1]) != 0) {
                perror("cd");
            }
        } else {
            fprintf(stderr, "cd: missing argument\n");
        }
        return 1;
    }

    if (strcmp(cmd.command, "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }

    // External commands (fork + execvp + waitpid)
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child Process
        if (cmd.input_file != NULL) {
            int fd = open(cmd.input_file, O_RDONLY);
            if (fd < 0) {
                perror("open input file");
                exit(1);
            }
            if (dup2(fd, STDIN_FILENO) < 0) {
                perror("dup2 stdin");
                close(fd);
                exit(1);
            }
            close(fd);
        }

        if (cmd.output_file != NULL) {
            int flags = O_WRONLY | O_CREAT;

            if (cmd.append) {
                flags |= O_APPEND;   // Append mode (>>)
            } else {
                flags |= O_TRUNC;    // Truncate mode (>)
            }

            int fd = open(cmd.output_file, flags, 0644);

            if (fd < 0) {
                perror("open output file");
                exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2 stdout");
                close(fd);
                exit(1);
            }
            close(fd);
        }

        // Run the command
        execvp(cmd.command, cmd.args);

        // If execvp returns, it failed
        if (errno == ENOENT) {
            fprintf(stderr, "mysh: command not found: %s\n", cmd.command);
            exit(127);
        } else {
            perror("execvp");
            exit(1);
        }
    } else {
        // Parent process
        int status;

        if (cmd.background) {
            if (bg_count < MAX_JOBS) {
                bg_pids[bg_count] = pid;
                bg_job_ids[bg_count] = next_job_id++;
                printf("[Job %d] %d started in background\n", bg_job_ids[bg_count], (int)pid);
                bg_count++;
            } else {
                fprintf(stderr, "mysh: maximum background jobs reached\n");
            }
        } else {
            // Wait for foreground job to finish
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            } else {
                if (WIFEXITED(status)) {
                    int code = WEXITSTATUS(status);
                    if (code != 0) {
                        printf("Command exited with code %d\n", code);
                    }
                }
            }
        }
    }

    return 1; // Tell main() to continue the loop
}