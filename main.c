#define _GNU_SOURCE
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
    BUF_SIZE = 1024
};

void write_and_hole(int fd, const char* title, const char* text, off_t offset_hole) {
    if((fd == -1) || (title == NULL) || (text == NULL)) {
        errExit("write_and_hole: invalid arguments\n");
    }

    size_t len = strlen(text);
    if(write(fd, text, len) != len) {
        errExit("couldn't write %s to file\n", title);
    } else {
        printf("Write in file %s: %s\n", title, text);
    }

    if(offset_hole) {
        off_t offset = lseek(fd, offset_hole, SEEK_SET);

        if(offset == -1) {
            errExit("couldn't create hole in file\n");
        }
    }
}

void create_hole_file(const char* filepath) {
    int openFlags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    int fd = open(filepath, openFlags, filePerms);
    if(fd == -1) {
        errExit("opening file %s\n", filepath);
    }

    write_and_hole(fd, "start", "start", 1024 * 10);
    write_and_hole(fd, "middle", "middle", 1024 * 20);
    write_and_hole(fd, "end", "end", 0);

    close(fd);
    printf("The file with hole created\n");
}

int main(int argc, char* argv[]) {
    int inputFd, outputFd;
    int openFlags, filePerms;
    ssize_t numRead, numWritten;

    char buf[BUF_SIZE];

    if((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s old-file new-file | -h file-hole\n", argv[0]);
    } else if((argc == 3) && (strcmp(argv[1], "-h") == 0)) {
        create_hole_file(argv[2]);
        exit(EXIT_SUCCESS);
    } else if(argc != 3) {
        usageErr("%s old-file new-file | -h file-hole\n", argv[0]);
    }

    inputFd = open(argv[1], O_RDONLY);
    if(inputFd == -1){
        errExit("opening file: %s\n", argv[1]);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    outputFd = open(argv[2], openFlags, filePerms);
    if(outputFd == -1) {
        errExit("opening file %s\n", argv[2]);
    }

    size_t len = 0;
    int i = 0;

    off_t offset = 0;
    off_t hole = 0;
    off_t data = 0;
    off_t current_offset = 0;

    struct stat st;
    fstat(inputFd, &st);

    hole = lseek(inputFd, offset, SEEK_HOLE);
    if(hole == st.st_size) {
        // hole is not
        while((numRead = read(inputFd, buf, BUF_SIZE)) != 0) {
            if(write(outputFd, buf, numRead) == -1) {
                errExit("write file: %s", argv[2]);
            }
        }

        goto graceful_shutdown;
    }

    data = lseek(inputFd, offset, SEEK_DATA);
    if(data == -1) {
        if(errno == ENXIO) {
            // All file is hole
            if(lseek(outputFd, st.st_size, SEEK_SET) == -1) {
                errExit("lseek");
            }

            goto graceful_shutdown;
        } else {
            errExit("SEEK_DATA");
        }
    }

    while(offset < st.st_size) {
        data = lseek(inputFd, offset, SEEK_DATA);
        if(data == -1) {
            current_offset = lseek(outputFd, 0, SEEK_CUR);
            if(current_offset == -1) {
                errExit("lseek");
            }

            if(lseek(outputFd, (st.st_size - current_offset), SEEK_CUR) == -1) {
                errExit("lseek");
            }

            break;
        }

        hole = lseek(inputFd, data, SEEK_HOLE);
        if(hole == -1) {
            hole = st.st_size;
        }

        if(hole == data) {
            break;
        }

        if(lseek(inputFd, data, SEEK_SET) == -1){
            errExit("lseek");
        }

        len = (hole - data);
        if(len > BUF_SIZE) {
            int current_size = BUF_SIZE;
            while((numRead = read(inputFd, buf, current_size)) != 0) {
                if(numRead == -1) {
                    errExit("read");
                }

                if(write(outputFd, buf, numRead) == -1) {
                    errExit("write");
                }

                len -= numRead;
                if(len < BUF_SIZE) {
                    current_size = len;
                }
            }
        } else {
            numRead = read(inputFd, buf, len);
            if(numRead != len) {
                errExit("read");
            }

            if(write(outputFd, buf, numRead) == -1) {
                errExit("write");
            }
        }

        offset = hole;
    }

    graceful_shutdown:
    if(close(inputFd) == -1) {
        errExit("close input\n");
    }

    if(close(outputFd) == -1) {
        errExit("close output\n");
    }

    printf("Data from file %s copied to file %s\n", argv[1], argv[2]);
    exit(EXIT_SUCCESS);
}