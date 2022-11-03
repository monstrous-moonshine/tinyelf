#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>

#define _noreturn_ __attribute__((noreturn))
#define _section_(x) __attribute__((section(x)))
#define _aligned_(x) __attribute__((aligned(x)))

static inline ssize_t my_write(int fd, const void *buf, size_t size) {
    ssize_t ret;
    asm volatile (
        "syscall"
        : "=a"(ret)
        : "a"(__NR_write), "D"(fd), "S"(buf), "d"(size)
        : "rcx", "r11", "memory"
    );
    return ret;
}

_noreturn_ static inline void my_exit(int status) {
    asm volatile (
        "syscall"
        :
        : "a"(__NR_exit), "D"(status)
        :
    );
    __builtin_unreachable();
}

_noreturn_ void _start() {
    _section_(".text#") _aligned_(1)
    static const char message[] = "Hello, world!\n";

    my_write(STDOUT_FILENO, message, sizeof message - 1);
    my_exit(0);
}
