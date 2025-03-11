#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Forward declaration to avoid infinite recursion warning
void primes(int left_pipe) __attribute__((noreturn));

void primes(int left_pipe) {
    int num;
    int p[2]; // Pipe for the next stage

    // Read the first number from the left pipe
    if (read(left_pipe, &num, sizeof(int)) == 0) {
        // Pipe closed, no more numbers
        close(left_pipe);
        exit(0);
    }

    // This number is a prime
    printf("prime %d\n", num);

    // Create a pipe for the next stage
    if (pipe(p) < 0) {
        printf("pipe failed\n");
        close(left_pipe);
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        close(left_pipe);
        close(p[0]);
        close(p[1]);
        exit(1);
    }

    if (pid == 0) {
        // Child: Close write end of new pipe, recurse with read end
        close(p[1]);
        close(left_pipe); // Done with left pipe in child
        primes(p[0]);     // Recursive call to handle next prime
    } else {
        // Parent: Filter numbers and write to the new pipe
        close(p[0]); // Close read end, we only write

        int next_num;
        while (read(left_pipe, &next_num, sizeof(int)) > 0) {
            if (next_num % num != 0) {
                // Not a multiple of the current prime, pass it along
                if (write(p[1], &next_num, sizeof(int)) != sizeof(int)) {
                    printf("write failed\n");
                    break;
                }
            }
        }

        // Done reading, close pipes and wait for child
        close(left_pipe);
        close(p[1]); // Close write end to signal end to child
        wait(0);     // Wait for the child process to finish
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int p[2];

    // Create the first pipe
    if (pipe(p) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        close(p[0]);
        close(p[1]);
        exit(1);
    }

    if (pid == 0) {
        // Child: Close write end and start sieving
        close(p[1]);
        primes(p[0]);
    } else {
        // Parent: Write numbers 2 to 280 into the pipe
        close(p[0]); // Close read end, we only write

        for (int i = 2; i <= 280; i++) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                printf("write failed\n");
                break;
            }
        }

        // Close write end to signal end of input
        close(p[1]);
        wait(0); // Wait for all children to finish
        exit(0);
    }

    return 0;
}