#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {

    //Amount of children
    pid_t childPids[15];
    int status;

    printf("Parent Process ID: %d\n", getpid());

    //Commands for the children
    char * commands[15][3] = {
        {"ls", NULL, NULL},
        {"date", NULL, NULL},
        {"whoami", NULL, NULL},
        {"pwd", NULL, NULL},
        {"uname", NULL, NULL},
        {"echo", "Hello Jerry", NULL},
        {"ps", NULL, NULL},
        {"hostname", NULL, NULL},
        {"invalidcmd1", NULL, NULL},
        {"invalidcmd2", NULL, NULL},
        {"df", "-h", NULL},
        {"free", "-h", NULL},
        {"id", NULL, NULL},
        {"ABORT", NULL, NULL},
        {"ABORT", NULL, NULL}
    };

    //Create children and assigning commands
    for (int i = 0; i < 15; i++) {

        pid_t pid = fork();
        

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {

            // Child process
            printf("Child Process ID: %d, PID: %d, Command: %s\n", i, getpid(), commands[i][0]);
            
            //Check for abort command to abort
            if(strcmp(commands[i][0], "ABORT") == 0) {
                abort();
            }

            //Execute command
            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            childPids[i] = pid;
        }
    }

    int normalZero = 0;
    int normalNonZero = 0;
    int signaled = 0;

    printf("Waiting for children to finish...\n");
    for (int i = 0; i < 15; i++) {
        waitpid(childPids[i], &status, 0);
        
        //Reporting status
        if(WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            printf("Child PID %d exited normally with exit code %d\n", childPids[i], exitCode);
            
            //Counter for exit codes
            if(exitCode == 0) {
                normalZero++;
            } else {
                normalNonZero++;
            }
        
        //Check if terminated by signal
        } else if(WIFSIGNALED(status)) {
            int termSig = WTERMSIG(status);
            printf("Child PID %d was terminated by signal %d\n", childPids[i], termSig);
            signaled++;
        }
    }

    //Printing summary
    printf("Normal exits with zero: %d\n", normalZero);
    printf("Normal exits with non-zero: %d\n", normalNonZero);
    printf("Signaled exits: %d\n", signaled);

    return 0;
}