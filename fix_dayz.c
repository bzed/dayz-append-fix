#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h> // For AT_FDCWD and related constants


// Function pointer types for the original functions.
typedef int (*original_open64_f)(const char *pathname, int flags, ...);
typedef int (*original_open_f)(const char *pathname, int flags, ...);
typedef int (*original_openat_f)(int dirfd, const char *pathname, int flags, ...);
typedef int (*original_openat64_f)(int dirfd, const char *pathname, int flags, ...);
typedef int (*original_creat_f)(const char *pathname, mode_t mode);
typedef int (*original_creat64_f)(const char *pathname, mode_t mode);
typedef int (*original_dup_f)(int oldfd);
typedef int (*original_dup2_f)(int oldfd, int newfd);
typedef int (*original_dup3_f)(int oldfd, int newfd, int flags);


// Keep track if the original functions have already been resolved
static original_open64_f real_open64 = NULL;
static original_open_f real_open = NULL;
static original_openat_f real_openat = NULL;
static original_openat64_f real_openat64 = NULL;
static original_creat_f real_creat = NULL;
static original_creat64_f real_creat64 = NULL;
static original_dup_f real_dup = NULL;
static original_dup2_f real_dup2 = NULL;
static original_dup3_f real_dup3 = NULL;



// Helper function to modify flags (avoids code duplication)
static inline int modify_flags(int flags) {
    if ((flags & O_APPEND) && !(flags & (O_WRONLY | O_RDWR))) {
        fprintf(stderr, "Flags modified: 0x%x -> 0x%x\n", flags, flags | O_RDWR);
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

    flags = modify_flags(flags);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_open64(pathname, flags, mode);
    } else {
        fd = real_open64(pathname, flags);
    }
    va_end(args);

    if (fd == -1) {
        fprintf(stderr, "open64 failed: %s\n", strerror(errno));
    }
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

    flags = modify_flags(flags);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_open(pathname, flags, mode);
    } else {
        fd = real_open(pathname, flags);
    }
    va_end(args);

    if (fd == -1) {
        fprintf(stderr, "open failed: %s\n", strerror(errno));
    }
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
    flags = modify_flags(flags);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_openat(dirfd, pathname, flags, mode);
    } else {
        fd = real_openat(dirfd, pathname, flags);
    }
    va_end(args);

    if (fd == -1) {
        fprintf(stderr, "openat failed: %s\n", strerror(errno));
    }
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

    flags = modify_flags(flags);

    va_list args;
    va_start(args, flags);
    int fd;
    if (flags & O_CREAT) {
        mode_t mode = va_arg(args, mode_t);
        fd = real_openat64(dirfd, pathname, flags, mode);
    } else {
        fd = real_openat64(dirfd, pathname, flags);
    }
    va_end(args);

    if (fd == -1) {
        fprintf(stderr, "openat64 failed: %s\n", strerror(errno));
    }
    return fd;
}

// --- creat ---
int creat(const char *pathname, mode_t mode) {
    if (!real_creat) {
        real_creat = (original_creat_f)dlsym(RTLD_NEXT, "creat");
        if (!real_creat) {
            fprintf(stderr, "Error resolving creat: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }

    // creat is equivalent to open with O_CREAT | O_WRONLY | O_TRUNC
    // Therefore, no need for modify_flags, as O_WRONLY is always set.
    int fd = real_creat(pathname, mode);
    if (fd == -1) {
        fprintf(stderr, "creat failed: %s\n", strerror(errno));
    }
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
    // creat64 is equivalent to open64 with O_CREAT | O_WRONLY | O_TRUNC
    // Therefore, no need for modify_flags, as O_WRONLY is always set.
    int fd = real_creat64(pathname, mode);
    if (fd == -1) {
        fprintf(stderr, "creat64 failed: %s\n", strerror(errno));
    }
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
    if (newfd == -1) {
        fprintf(stderr, "dup failed: %s\n", strerror(errno));
    }
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
    if (result == -1) {
        fprintf(stderr, "dup2 failed: %s\n", strerror(errno));
    }
    return result;
}

// --- dup3 ---
int dup3(int oldfd, int newfd, int flags){
      if (!real_dup3) {
        real_dup3 = (original_dup3_f)dlsym(RTLD_NEXT, "dup3");
        if (!real_dup3) {
            fprintf(stderr, "Error resolving dup3: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }
    }
    int result = real_dup3(oldfd, newfd, flags);
    if (result == -1) {
        fprintf(stderr, "dup3 failed: %s\n", strerror(errno));
    }
    return result;
}
