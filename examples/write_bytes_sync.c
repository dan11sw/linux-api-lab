#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"
#include <stdbool.h>

char generate_byte() {
    return (char)(rand() % 256 - 128);
}

int main(int argc, char* argv[]) {
    int fd = -1;
    bool x = false;

    if(argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s filename num-bytes\n", argv[0]);
    }

    int numBytes = getInt(argv[2], GN_ANY_BASE, "num-bytes");

    if(argc >= 4 && strcmp(argv[3], "x") == 0) {
        x = true;
    }

    int openFlags;
    if(x) {
        openFlags = O_CREAT | O_WRONLY;
    } else {
        openFlags = O_CREAT | O_WRONLY | O_APPEND;
    }

    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    // Open file
    fd = open(argv[1], openFlags, filePerms);
    if(fd == -1) {
        errExit("open");
    }

    for(int i = 0; i < numBytes; i++) {
        char byte = generate_byte();

        if(x) {
            // Always before write one byte make offset in file on end file
            if(lseek(fd, 0, SEEK_END) == -1) {
                errExit("lseek");
            }
        }

        // Write one byte
        if(write(fd, &byte, 1) == -1) {
            errExit("write");
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
    return 0;
}