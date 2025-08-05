#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <limits.h>

int main() {
    char buffer[PATH_MAX];

    if(getcwd(buffer, sizeof(buffer))) {
        std::cout << "Current working directory: " << buffer << std::endl;
    } else {
        std::cout << "Error: " << strerror(errno) << std::endl;
    }

    return 0;
}
