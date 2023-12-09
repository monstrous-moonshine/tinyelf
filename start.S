	.text
	.globl	_start
	.type	_start, @function
_start:
	xor	%ebp, %ebp		  # clear BP to satisfy ABI
	pop	%rdi			  # argc
	mov	%rsp, %rsi		  # argv
	and	$0xfffffffffffffff0, %rsp # align SP before call
	call	main			  # main(argc, argv)
	mov	$60, %eax		  # __NR_exit
	mov	$0, %edi		  # status
	syscall				  # _exit(status)
	.size	_start, .-_start
