#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int p_read) __attribute__((noreturn)); //function prototype

void primes(int p_read) {
    int p[2], prime, num;
    
    if (read(p_read, &prime, sizeof(prime)) == 0) {
        close(p_read);
        exit(0);
    }
    
    printf("prime %d\n", prime);
    pipe(p);
    
    if (fork() == 0) { // Child process
        close(p[1]);
        close(p_read);
        primes(p[0]);
    } else { // Parent process
        close(p[0]);
        
        while (read(p_read, &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(p[1], &num, sizeof(num));
            }
        }
        
        close(p[1]);
        close(p_read);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);
    
    if (fork() == 0) { // Child process
        close(p[1]);
        primes(p[0]);
    } else { // Parent process
        close(p[0]);
        
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }
        
        close(p[1]);
        wait(0);
        exit(0);
    }
}