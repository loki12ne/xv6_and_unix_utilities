#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: xargs <command> [args...]\n");
        exit(1);
    }

    char buf[512];       // Buffer for reading a line
    char *xargv[MAXARG]; // Argument vector for exec
    int i;

    // Copy initial command and arguments into xargv
    for (i = 1; i < argc; i++) {
        xargv[i - 1] = argv[i];
    }
    int base_argc = argc - 1; // Number of fixed arguments

    // Read input lines from stdin
    char *p = buf;
    int n;
    while ((n = read(0, p, 1)) > 0) {
        if (*p == '\n') {
            *p = 0; // Null-terminate the line

            // Fork a child to execute the command
            int pid = fork();
            if (pid < 0) {
                printf("xargs: fork failed\n");
                exit(1);
            }

            if (pid == 0) {
                // Child: Append the line as the last argument
                xargv[base_argc] = buf;
                xargv[base_argc + 1] = 0; // Null-terminate argv array

                // Execute the command
                exec(argv[1], xargv);
                printf("xargs: exec %s failed\n", argv[1]);
                exit(1);
            } else {
                // Parent: Wait for child to finish
                wait(0);
            }

            // Reset buffer for next line
            p = buf;
        } else {
            p++;
            if (p - buf >= sizeof(buf)) {
                printf("xargs: line too long\n");
                exit(1);
            }
        }
    }

    if (n < 0) {
        printf("xargs: read failed\n");
        exit(1);
    }

    exit(0);
}