#define _LARGEFILE64_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int fd = -1;
    off64_t off = -1;

    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pathname offset\n", argv[0]);
    }

    // Open large file for read and write (if not exist - created file)
    fd = open64(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1) {
        errExit("open64");
    }

    // Execute lseek for large file
    off = atoll(argv[2]);
    if(lseek64(fd, off, SEEK_SET) == -1) {
        errExit("lseek64");
    }
    
    if(write(fd, "test", 4) == -1) {
        errExit(write);
    }

    exit(EXIT_SUCCESS);
    return 0;
}