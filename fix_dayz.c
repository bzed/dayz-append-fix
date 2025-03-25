#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h> // For AT_FDCWD

// Function pointer types
typedef int (*original_open64_f)(const char *pathname, int flags, ...);
typedef int (*original_open_f)(const char *pathname, int flags, ...);
typedef int (*original_openat_f)(int dirfd, const char *pathname, int flags, ...);
typedef int (*original_openat64_f)(int dirfd, const char *pathname, int flags, ...);
/*
typedef int (*original_creat_f)(const char *pathname, mode_t mode);
typedef int (*original_creat64_f)(const char *pathname, mode_t mode);
typedef int (*original_dup_f)(int oldfd);
typedef int (*original_dup2_f)(int oldfd, int newfd);
typedef int (*original_dup3_f)(int oldfd, int newfd, int flags);
*/

// Static variables to hold the original function pointers
static original_open64_f real_open64 = NULL;
static original_open_f real_open = NULL;
static original_openat_f real_openat = NULL;
static original_openat64_f real_openat64 = NULL;
/*
static original_creat_f real_creat = NULL;
static original_creat64_f real_creat64 = NULL;
static original_dup_f real_dup = NULL;
static original_dup2_f real_dup2 = NULL;
static original_dup3_f real_dup3 = NULL;
*/

// Helper function to modify flags and print debug info
static inline int modify_flags(int flags, const char* function_name, const char* pathname) {
    if ((flags & O_APPEND) && !(flags & (O_WRONLY | O_RDWR))) {
        fprintf(stderr, "%s: Modifying flags for %s: 0x%x -> 0x%x\n", function_name, pathname, flags, flags | O_RDWR);
        return flags | O_RDWR;
    }
    return flags;
}

// --- open64 ---
int open64(const char *pathname, int flags, ...) {
    if (!real_open64) {
        real_open64 = (original_open64_f)dlsym(RTLD_NEXT, "open64");
        if (!real_open64) {
            fprintf(stderr, "Error resolving open64: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    int modified_flags = modify_flags(flags, "open64", pathname);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_open64(pathname, modified_flags, mode);
    } else {
        fd = real_open64(pathname, modified_flags);
    }
    va_end(args);

    return fd;
}

// --- open ---
int open(const char *pathname, int flags, ...) {
    if (!real_open) {
        real_open = (original_open_f)dlsym(RTLD_NEXT, "open");
        if (!real_open) {
            fprintf(stderr, "Error resolving open: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    int modified_flags = modify_flags(flags, "open", pathname);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_open(pathname, modified_flags, mode);
    } else {
        fd = real_open(pathname, modified_flags);
    }
    va_end(args);
    return fd;
}

// --- openat ---
int openat(int dirfd, const char *pathname, int flags, ...) {
    if (!real_openat) {
        real_openat = (original_openat_f)dlsym(RTLD_NEXT, "openat");
        if (!real_openat) {
            fprintf(stderr, "Error resolving openat: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    // Pass pathname directly.  It's the caller's responsibility
    // to interpret it correctly relative to dirfd.
    int modified_flags = modify_flags(flags, "openat", pathname);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_openat(dirfd, pathname, modified_flags, mode);
    } else {
        fd = real_openat(dirfd, pathname, modified_flags);
    }
    va_end(args);

    return fd;
}

// --- openat64 ---
int openat64(int dirfd, const char *pathname, int flags, ...) {
    if (!real_openat64) {
        real_openat64 = (original_openat64_f)dlsym(RTLD_NEXT, "openat64");
        if (!real_openat64) {
            fprintf(stderr, "Error resolving openat64: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    // Pass pathname directly.
    int modified_flags = modify_flags(flags, "openat64", pathname);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_openat64(dirfd, pathname, modified_flags, mode);
    } else {
        fd = real_openat64(dirfd, pathname, modified_flags);
    }
    va_end(args);

    return fd;
}

/*
// --- creat ---
int creat(const char *pathname, mode_t mode) {
    if (!real_creat) {
        real_creat = (original_creat_f)dlsym(RTLD_NEXT, "creat");
        if (!real_creat) {
            fprintf(stderr, "Error resolving creat: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int fd = real_creat(pathname, mode);
    return fd;
}

// --- creat64 ---
int creat64(const char *pathname, mode_t mode) {
    if (!real_creat64) {
        real_creat64 = (original_creat64_f)dlsym(RTLD_NEXT, "creat64");
        if (!real_creat64) {
            fprintf(stderr, "Error resolving creat64: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int fd = real_creat64(pathname, mode);
    return fd;
}

// --- dup ---
int dup(int oldfd) {
    if (!real_dup) {
        real_dup = (original_dup_f)dlsym(RTLD_NEXT, "dup");
        if (!real_dup) {
            fprintf(stderr, "Error resolving dup: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int newfd = real_dup(oldfd);
    return newfd;
}

// --- dup2 ---
int dup2(int oldfd, int newfd) {
    if (!real_dup2) {
        real_dup2 = (original_dup2_f)dlsym(RTLD_NEXT, "dup2");
        if (!real_dup2) {
            fprintf(stderr, "Error resolving dup2: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int result = real_dup2(oldfd, newfd);
    return result;
}

// --- dup3 ---
int dup3(int oldfd, int newfd, int flags) {
    if (!real_dup3) {
        real_dup3 = (original_dup3_f)dlsym(RTLD_NEXT, "dup3");
        if (!real_dup3) {
            fprintf(stderr, "Error resolving dup3: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int result = real_dup3(oldfd, newfd, flags);
    return result;
}
*/
