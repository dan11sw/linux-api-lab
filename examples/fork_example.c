#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t pid;
    int status = 0;

    switch(pid = fork()) {
        case -1: {
            perror("fork");
            exit(1);
        }
        case 0: {
            // sleep(5);
            printf("\nChild process\n");
            printf("Child process pid: %d\n", getpid());
            printf("Child process ppid: %d\n", getppid());
            exit(0);
        }
        default: {
            printf("Parent process create prosecc with pid = %d\n", pid);
            printf("Parent process pid: %d\n", getpid());
            printf("Parent process ppid: %d\n", getppid());

            sleep(25);
            //wait(&status);
            // printf("Return code of child: %d\n", WEXITSTATUS(status));

            // exit(0);
        }
    }
    
    return 0;
}