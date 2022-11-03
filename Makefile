CFLAGS = -Wall -Wextra

all: patch hello

patch: patch.c

hello: hello.c
	$(CC) -static -nostdlib -Os $^ -o $@

clean:
	$(RM) patch hello

.PHONY: all clean
