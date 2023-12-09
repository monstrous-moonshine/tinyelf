#ifndef UTIL_H
#define UTIL_H
#include <sys/syscall.h>
#include <sys/types.h>

static inline ssize_t write(int fd, const void *buf, size_t count) {
    ssize_t ret;
    // This says, execute the syscall instruction with the following
    // values:
    // - %eax: __NR_write, the linux syscall number for sys_write
    // - %edi: @fd
    // - %esi: @buf
    // - %edx: @count
    // It also says that %eax is written to by this assembly segment,
    // and that the registers %rcx and %r11 are clobbered. Linux puts
    // the return value from the syscall in %eax, and the other two
    // registers are clobbered immediately by the syscall instruction.
    // Memory is also put in the clobber list since @buf is read.
    asm volatile (
        "syscall"
        : "=a"(ret)
        : "a"(__NR_write), "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory"
    );
    return ret;
}

#if 0
static size_t strlen(const char *s) {
    size_t cnt = 0;
    while (*s++) cnt++;
    return cnt;
}
#endif
#endif
