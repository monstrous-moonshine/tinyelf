#include "util.h"

#define STDOUT_FILENO 1
#define _tdata_ __attribute__((section(".text#"), aligned(1)))


int main(int argc, char *argv[]) {
    // We keep the @argc and the @argv arguments to remind ourselves
    // that our startup routine calls main with these arguments and
    // we could use them if we wanted. Here, we don't.
    (void)argc;
    (void)argv;
    // Keep it in the .text section along with the code. Look at the
    // generated assembly (make hello.s) to understand the # at the
    // end.
    static _tdata_ const char message[] = "Hello, world!\n";
    // We don't need to write the terminating NULL, so subtract 1.
    write(STDOUT_FILENO, message, sizeof message - 1);
}
