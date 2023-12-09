CFLAGS = -Wall -Wextra -Os

all: patch hello

patch: patch.c

hello: hello.o start.o
	$(CC) -static -nostdlib -o $@ $^

hello.o: CFLAGS += -ffreestanding

hello.s: hello.c
	$(CC) $(CFLAGS) -ffreestanding -S -o $@ $<

clean:
	$(RM) patch hello hello.o start.o hello.s

.PHONY: all clean
