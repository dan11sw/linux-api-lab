#include <sys/stat.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <gnu/libc-version.h>
#include <dirent.h>
#include <ctype.h>

void print_flags(unsigned int flags) {
    printf("Decode flags (octal %o): \n", flags);

    int accmode = flags & O_ACCMODE;
    if(accmode == O_WRONLY) printf("WRONLY ");
    else if(accmode == O_RDONLY) printf("RDONLY ");
    else if(accmode == O_RDWR) printf("RDWR ");

    if(flags & O_APPEND) printf("APPEND ");
    if(flags & O_NONBLOCK) printf("NONBLOCK ");
    if(flags & O_CREAT) printf("CREAT ");
    if(flags & O_TRUNC) printf("TRUNC ");
}

void print_decode_fdinfo(const char* path) {
    FILE* f = fopen(path, "r");
    if(!f) {
        return;
    }

    char line[256];
    
    unsigned long pos = 0;
    unsigned int flags = 0;
    unsigned int mnt_id = 0;
    unsigned long ino = 0;

    while(fgets(line, sizeof(line), f)) {
        if(sscanf(line, "pos: %lu", &pos) == 1) continue;
        if(sscanf(line, "flags: %o", &flags) == 1) continue;
        if(sscanf(line, "mnt_id: %u", &mnt_id) == 1) continue;
        if(sscanf(line, "ino: %lu", &ino) == 1) continue;
    }

    printf("Print fdinfo: ");
    printf(" pos    = %lu\n", pos);
    printf(" mnt_id = %u\n", mnt_id);
    printf(" ino    = %lu\n", ino);

    print_flags(flags);
}

pid_t find_pid_by_name(const char* target_name) {
    DIR* dir = opendir("/proc");
    if(!dir) {
        printf("Error: opendir /proc\n");
        return - 1;
    }

    struct dirent* entry;
    pid_t found_pid = -1;

    while((entry = readdir(dir)) != NULL) {
        int is_pid = 1;
        for(char* p = entry->d_name; *p; p++) {
            if(!isdigit((unsigned char)*p)) {
                is_pid = 0;
                break;
            }
        }

        if(!is_pid) {
            continue;
        }

        // proc/PID/comm
        char comm_path[256];
        snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);

        FILE* f = fopen(comm_path, "r");
        if(!f) {
            continue;
        }

        char comm_name[256];
        if(fgets(comm_name, sizeof(comm_name), f)) {
            comm_name[strcspn(comm_name, "\n")] = 0;
            if(strcmp(comm_name, target_name) == 0) {
                found_pid = (pid_t)atoi(entry->d_name);
                fclose(f);
                break;
            }
        }

        fclose(f);
    }

    closedir(dir);
    return found_pid;
}

char* get_fd_path(pid_t pid, int fd) {
    char link_path[64];
    snprintf(link_path, sizeof(link_path), "/proc/%d/fd/%d", (int)pid, fd);

    char* buf = malloc(PATH_MAX);
    if(!buf) {
        return NULL;
    }

    ssize_t len = readlink(link_path, buf, PATH_MAX - 1);
    if(len == -1) {
        free(buf);
        return NULL;
    }

    buf[len] = '\0';
    return buf;
}

int* get_fd_list(pid_t pid, size_t* count) {
    *count = 0;

    char dir_path[64];
    snprintf(dir_path, sizeof(dir_path), "/proc/%d/fd", (int)pid);

    DIR* dir = opendir(dir_path);
    if(!dir) {
        return NULL;
    }

    size_t capacity = 64;
    size_t iter = 0;

    int* fds = malloc(capacity * sizeof(int));
    if(!fds) {
        closedir(dir);
        return NULL;
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
        char is_all_digit = 1;
        for(char* p = entry->d_name; *p; p++) {
            if(!isdigit((unsigned char)*p)) {
                is_all_digit = 0;
                break;
            }
        }

        if(!is_all_digit) {
            continue;
        }

        if(iter == capacity) {
            capacity *= 2;
            int* tmp = realloc(fds, capacity * sizeof(int));
            if(!tmp) {
                free(fds);
                closedir(dir);
                *count = 0;

                return NULL;
            }

            fds = tmp;
        }

        fds[iter] = atoi(entry->d_name);
        iter++;
    }

    closedir(dir);
    *count = iter;

    if(iter == 0) {
        free(fds);
        return NULL;
    }

    int* shrink = realloc(fds, iter * sizeof(int));
    return shrink ? shrink : fds;
}

int main(int argc, char* argv[]) {
    const char* target_name = "main_call";
    pid_t target_pid = find_pid_by_name("main-call");
    printf("PID %s = %d\n", target_name, target_pid);

    if(target_pid == -1) {
        return 1;
    }

    size_t count = 0;
    int* fds = get_fd_list(target_pid, &count);

    if(!fds) {
        return 1;
    }

    printf("Count fds: %zu\n", count);

    for(int i = 0; i < count; i++) {
        int fd_item = fds[i];

        char* fd_path = get_fd_path(target_pid, fd_item);
        if(fd_path) {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%d/fdinfo/%d", (int)target_pid, fd_item);

            printf("fd path by descriptor %d = %s\n", fd_item, fd_path);
            print_decode_fdinfo(path);
            printf("\n\n");

            free(fd_path);
        } else {
            printf("fd path by descriptor %d is not defined\n", fd_item);
        }
    }

    free(fds);
    printf("\n");
    return 0;
}