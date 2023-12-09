CFLAGS = -ffreestanding -Wall -Wextra -Os

all: patch hello

patch: patch.c

hello: hello.o start.o
	$(CC) -static -nostdlib -o $@ $^

hello.s: hello.c
	$(CC) $(CFLAGS) -S -o $@ $<

clean:
	$(RM) patch hello hello.o start.o

.PHONY: all clean
