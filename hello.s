    .text
    .globl _start
_start:
    mov     $1, %eax         # __NR_write
    mov     $1, %edi         # STDOUT_FILENO
    lea     .LC0(%rip), %rsi # message
    mov     $14, %edx        # message length
    syscall
    mov     $60, %eax        # __NR_exit
    mov     $0, %edi         # status
    syscall
.LC0:
    .string "Hello, world!\n"
