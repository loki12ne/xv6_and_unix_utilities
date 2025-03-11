#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void close_pipes(int p1[2], int p2[2]) {
    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);
}

int main(int argc, char *argv[]) {
    int p1[2];  // p1[0] read, p1[1] write
    int p2[2];  // p2[0] read, p2[1] write
    char buf[1];

    // Create pipes
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("pipe creation failed\n");
        exit(1);  
    }

    int pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
        close_pipes(p1, p2);
        exit(1);
    }

    if (pid == 0) { // Child process
        close(p1[1]); // Close write end of p1
        close(p2[0]); // Close read end of p2

        if (read(p1[0], buf, 1) != 1) {
            printf("child: read failed\n");
            close(p1[0]);
            close(p2[1]);
            exit(1);
        }

        // Định dạng chính xác để pass test
        printf("%d: received ping\n", getpid());

        if (write(p2[1], buf, 1) != 1) {
            printf("child: write failed\n");
            close(p1[0]);
            close(p2[1]);
            exit(1);
        }

        // Close remaining ends
        close(p1[0]);
        close(p2[1]);
        exit(0);
    } else { // Parent process
        close(p1[0]); // Close read end of p1
        close(p2[1]); // Close write end of p2

        buf[0] = 'A';
        if (write(p1[1], buf, 1) != 1) {
            printf("parent: write failed\n");
            close(p1[1]);
            close(p2[0]);
            exit(1);
        }

        if (read(p2[0], buf, 1) != 1) {
            printf("parent: read failed\n");
            close(p1[1]);
            close(p2[0]);
            exit(1);
        }

        printf("%d: received pong\n", getpid());

        // Close remaining ends
        close(p1[1]);
        close(p2[0]);

        wait(0); // Wait for child to finish
        exit(0);
    }
}
