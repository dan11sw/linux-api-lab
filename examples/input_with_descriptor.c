#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif // BUF_SIZE 

int main(int argc, char* argv[]) {
    const int32_t MAX_READ = 100;
    char buf[MAX_READ + 1];
    ssize_t numRead;

    numRead = read(STDIN_FILENO, &buf, MAX_READ);
    if(numRead == -1) {
        errExit("read");
    }

    buf[numRead] = '\0';
    printf("Input data: %s\n", buf);

    return 0;
}