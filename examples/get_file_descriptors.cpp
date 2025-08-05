#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <limits.h>
#include <dirent.h>

int main() {
    const char* fd_dir = "/proc/self/fd";
    DIR* dir = opendir(fd_dir);

    if(!dir) {
        std::cout << "Dir not opened: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    dirent* entry;
    char link_target[PATH_MAX];

    std::cout << "Open file descriptors: " << std::endl;
    while((entry = readdir(dir)) != nullptr) {
        if(entry->d_type == DT_LNK) {
            snprintf(link_target, sizeof(link_target), "%s/%s", fd_dir, entry->d_name);

            char fd_path[PATH_MAX];
            ssize_t len = readlink(link_target, fd_path, sizeof(fd_path) - 1);

            if(len != -1) {
                fd_path[len] = '\0';
                std::cout << "FD " << entry->d_name << " -> " << fd_path << std::endl;
            }
        }
    }

    closedir(dir);

    return 0;
}
