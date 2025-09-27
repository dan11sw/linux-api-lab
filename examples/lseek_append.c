#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int fd = -1;

    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s filename data\n", argv[0]);
    }

    int openFlags = O_CREAT | O_WRONLY | O_APPEND;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    // Open large file for read and write (if not exist - created file)
    fd = open(argv[1], openFlags, filePerms);
    if(fd == -1) {
        errExit("open");
    }

    // Execute lseek for file (to begin)
    off_t currentSeek = lseek(fd, 0, SEEK_SET);
    printf("Current offset (before write): %ld\n", currentSeek);

    if(currentSeek == -1) {
        errExit("lseek");
    }
    
    if(write(fd, argv[2], strlen(argv[2])) == -1) {
        errExit("write");
    }

    // It will always increase because O_APPEND flag for open file guarantees that
    // lseek(fd, 0, SEEK_END) executed before each operation
    currentSeek = lseek(fd, 0, SEEK_CUR);
    printf("Current offset (after write): %ld\n", currentSeek);

    close(fd);
    exit(EXIT_SUCCESS);
    return 0;
}