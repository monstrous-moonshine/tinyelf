CFLAGS = -Wall -Wextra

all: patch hello

patch: patch.c

hello: hello.s
	$(CC) -static -nostdlib $^ -o $@

clean:
	$(RM) patch hello

.PHONY: all clean
