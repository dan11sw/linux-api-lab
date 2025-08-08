#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "lib/tlpi_hdr.h"

const int BUF_SIZE = 100;

void customTee(int fd_out) {
    char buf[BUF_SIZE];
    ssize_t numRead;

    // Read data from standart input
    numRead = read(STDIN_FILENO, &buf, BUF_SIZE);

    if(numRead == -1) {
        errExit("read");
    }

    // Write to file
    if(write(fd_out, buf, numRead) != numRead) {
        fatal("couldn't write whole buffer");
    }

    // Write to standart output
    if(write(STDOUT_FILENO, buf, numRead) != numRead) {
        fatal("couldn't write whole buffer");
    }
}

int main(int argc, char* argv[]) {
    // Disabling buffering
    setbuf(stdout, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);

    size_t len;
    off_t offset;
    int fd;

    char* buf;
    ssize_t numRead, numWritten;

    // Check arguments
    if((argc == 2 && (strcmp(argv[1], "-a") == 0))
        || (argc >= 3 && (strcmp(argv[1], "-a") != 0))
        || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s file | %s -a file\n", argv[0], argv[0]); 
    }

    int indexFile = (argc == 2) ? 1 : 2;

    // Open file descriptor
    fd = open(argv[indexFile], O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);

    if(fd == -1) {
        errExit("open");
    }

    if(lseek(fd, 0, SEEK_CUR) == -1) {
        errExit("lseek");
    }

    if(indexFile == 2) {
        printf("Append to file");

        // To end file
        /*if(lseek(fd, 0, SEEK_END) == -1) {
            errExit("lseek");
        }*/
    }

    printf("\nHello, world!");
    
    customTee(fd);

    close(fd);

    exit(EXIT_SUCCESS);
}