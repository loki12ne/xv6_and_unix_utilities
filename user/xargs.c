#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h" // MAXARG: maximum number of arguments

int main(int argc, char *argv[]) 
{
    char *cmd[MAXARG]; 
    char buffer[512];   
    int buffer_pos = 0;
    int pid;

    // Copy argv into cmd
    for (int i = 1; i < argc && i < MAXARG - 1; i++) {
        cmd[i - 1] = argv[i];
    }

    while (1) 
    {
        int r = read(0, &buffer[buffer_pos], 1); 
        if (r <= 0) break;  

        if (buffer[buffer_pos] == '\n')
        {
            buffer[buffer_pos] = '\0'; 
            cmd[argc - 1] = buffer;     
            cmd[argc] = 0;              

            // Create a new process to execute the command
            if ((pid = fork()) == 0) 
            {
                exec(cmd[0], cmd); // Execute the command
                fprintf(2, "exec failed\n");
                exit(1);
            } 
            else if (pid > 0) 
            { 
                wait(0);  // Wait for child to terminate
            } 
            else 
            {
                fprintf(2, "Fork Error!\n");
                exit(1);
            }

            buffer_pos = 0; // Reset buffer for next line
        } 
        else 
        {
            buffer_pos++; // Continue reading the line
        }
    }
    exit(0);
}
