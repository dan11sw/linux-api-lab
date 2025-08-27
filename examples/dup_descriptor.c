#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

int main(int argc, char* argv[]) {
    if(argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file\n", argv[0]);
    }

    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    int filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    int fd = open(argv[1], openFlags, filePerms);

    if(fd == -1) {
        errExit("opening file %s", argv[1]);
    }

    int fd_dup = dup(fd);
    int fd_dup2 = dup2(fd, 100);

    printf("File descriptor #1: %i\n", fd);
    printf("File descriptor #2: %i\n", fd_dup);
    printf("File descriptor #3: %i\n", fd_dup2);

    int fd_dup3 = fcntl(fd_dup, F_DUPFD, 102);
    if(fd_dup3 == -1) {
        errExit("fcntl F_DUPFD failed");
    }

    printf("File descriptor #4: %i", fd_dup3);

    close(fd);
    close(fd_dup);
    close(fd_dup2);
    close(fd_dup3);

    printf("\n");
    exit(EXIT_SUCCESS);
    return 0;
}