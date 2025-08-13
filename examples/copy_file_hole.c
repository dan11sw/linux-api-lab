#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif // BUF_SIZE 

void create_hole_file(char* filename) {
    int openFlags = O_RDWR | O_CREAT;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    int fd = open(filename, openFlags, filePerms);
    if(fd == -1) {
        errExit("opening file %s", filename);
    }

    const char* start_text = "start\0";
    size_t st_len = strlen(start_text);

    if(write(fd, start_text, st_len) != st_len) {
        close(fd);
        errExit("couldn't write data to a file");
    } else {
        printf("Write text in hole file: %s\n", start_text);
    }

    // Create hole in file
    off_t offset = lseek(fd, 1024 * 10, SEEK_SET);
    if(offset == -1) {
        close(fd);
        errExit("couldn't create hole in file");
    }

    const char* middle_text = "middle\0";
    size_t mt_end = strlen(middle_text);

    if(write(fd, middle_text, mt_end) != mt_end) {
        close(fd);
        errExit("couldn't write data to a file");
    } else {
        printf("Write text in hole file: %s\n", middle_text);
    }

    offset = lseek(fd, 1024 * 20, SEEK_SET);
    if(offset == -1) {
        close(fd);
        errExit("couldn't create hole in file");
    }

    const char* end_text = "end\0";
    size_t et_len = strlen(end_text);

    if(write(fd, end_text, et_len) != et_len) {
        close(fd);
        errExit("couldn't write data to a file");
    }else {
        printf("Write text in hole file: %s\n", end_text);
    }

    close(fd);
    printf("The file with hole created\n");
}

int main(int argc, char* argv[]) {
    if((argc != 3) || (strcmp(argv[1], "--help") == 0)) {
        usageErr("%s old-file new-file or -h new-hole-file\n", argv[0]);
    }

    // Created hole file
    if((argc == 3) && (strcmp(argv[1], "-h") == 0)) {
        create_hole_file(argv[2]);
        exit(EXIT_SUCCESS);
    }

    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // Open file for input
    inputFd = open(argv[1], O_RDONLY);
    if(inputFd == -1) {
        errExit("opening file %s", argv[1]);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    // Open file for output
    outputFd = open(argv[2], openFlags, filePerms);
    if(outputFd == -1) {
        errExit("opening file %s", argv[2]);
    }

    // Transfer data while not go to end file (working with hole file)
    while((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if(write(outputFd, buf, numRead) != numRead) {
            fatal("couldn't write whole buffer");
        }
    }

    if(numRead == -1) {
        errExit("read");
    }

    if(close(inputFd) == -1) {
        errExit("close input");
    }

    if(close(outputFd) == -1) {
        errExit("close output");
    }

    printf("Data from file %s copied to file %s\n", argv[1], argv[2]);

    return 0;
}