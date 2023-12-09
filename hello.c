#include <sys/syscall.h>
#include <sys/types.h>

#define STDOUT_FILENO 1
#define _tdata_ __attribute__((section(".text#"), aligned(1)))

static inline ssize_t write(int fd, const void *buf, size_t count) {
    ssize_t ret;
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

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    static _tdata_ const char message[] = "Hello, world!\n";
    write(STDOUT_FILENO, message, sizeof message - 1);
}
