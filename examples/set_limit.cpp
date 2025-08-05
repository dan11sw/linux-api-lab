#include <iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <signal.h>

void handle_sigxfsz(int sig) {
    std::cout << "Handler file size exceeded work" << std::endl;
}

// Function for set limit on used any resources
void set_limit(int resource, rlim_t soft, rlim_t hard, const char* name) {
    struct rlimit limit;

    if(getrlimit(resource, &limit) == -1) {
        std::cout << "Get rlimit failed: " << strerror(errno) << std::endl;
        return;
    }

    //std::cout << "Current limit: " << resource << " soft: " << (unsigned long)limit.rlim_cur 
       // << " hard: " << (unsigned long)limit.rlim_max << std::endl;

    limit.rlim_cur = soft;
    limit.rlim_max = hard;

    if(setrlimit(resource, &limit) == -1) {
        std::cout << "Set rlimit failed: " << strerror(errno) << std::endl;
        return;
    }

    //std::cout << "New limit: " << resource << " soft: " << (unsigned long)limit.rlim_cur 
        //<< " hard: " << (unsigned long)limit.rlim_max << std::endl;
}

int main() {
    // Limit on size file (in bytes)
    set_limit(RLIMIT_FSIZE, 1024, 2 * 1024, "file size");

    // Limit on segment of data process (heap)
    set_limit(RLIMIT_DATA, 256 * 1024 * 1024, 512 * 1024 * 1024, "data segment");

    // Limit on CPU (in seconds)
    set_limit(RLIMIT_CPU, 60, 120, "CPU time");

    // Limit on count opened files
    set_limit(RLIMIT_NOFILE, 1024, 2048, "open files");

    // Limit on stack size
    set_limit(RLIMIT_STACK, 8 * 1024 * 1024, 16 * 1024 * 1024, "stack size");

    signal(SIGXFSZ, handle_sigxfsz);

    // Check work limit with print to file
    FILE* f = fopen("text.txt", "w");
    if(f) {
        for(int i = 0; i < 100000; i++) {
            if(fprintf(f, "This is line %d\n", i) < 0) {
                std::cout << "Error limit: " << strerror(errno) << std::endl;
                break;
            }
        }

        fclose(f);
    }

    std::cout << "Program end" << std::endl;

    return 0;
}
