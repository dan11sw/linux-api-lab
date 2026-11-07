#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <gnu/libc-version.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>

enum {
    BUF_SIZE = 100
};

int main(int argc, char* argv[]) {
    // Disabling buffering
    setbuf(stdout, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);

    int fd;
    ssize_t numRead, numWritten;

    char buf[BUF_SIZE];

    if((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s {-a} file\n", argv[0]);
    } else if((argc < 3) && (strcmp(argv[1], "-a") == 0)) {
        usageErr("%s -a file\n", argv[0]);
    }

    char* filename = NULL;
    bool append_to_end = false;

    if((argc >= 3) && (strcmp(argv[1], "-a") == 0)) {
        append_to_end = true;
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    int flags = O_WRONLY | O_CREAT | (append_to_end ? O_APPEND : O_TRUNC);

    fd = open(filename, flags,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | // rw-rw-rw-
        S_IROTH | S_IWOTH
    );

    if(fd == -1) {
        errExit("open");
    }

    int counter_error_read = 0;
    while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) != 0) {
        if(numRead == -1) {
            if((errno == EINTR) && (counter_error_read < 5)) {
                counter_error_read++;
                continue;
            }

            close(fd);
            errExit("read STDIN_FILENO");
        } else if((numRead == 1 && buf[0] == '\n')) {
            // End write (only experiments)
            break;
        }

        numWritten = write(STDOUT_FILENO, buf, numRead);
        if((numWritten != numRead) || (numWritten == -1)) {
            close(fd);
            errExit("write STDOUT_FILENO");
        }

        numWritten = write(fd, buf, numRead);
        if((numWritten != numRead) || (numWritten == -1)) {
            close(fd);
            errExit("write fd");
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}