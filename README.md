This is a simple program to make a tiny elf executable from an input elf executable by keeping only the bare minimum necessary for it to run. That means the elf header, one segment header to mark the loadable and executable segment which will carry the code, and the code itself.

## Trying it out
The sample program `hello.s` is the classic hello world written in x64 assembly. Typing `make` will make both the `hello` test program, and the `patch` program which does the actual patching. Note that the `hello` program thus produced is significantly bigger (about 4 KB) than the 50 or so bytes of code it actually needs (you can find out the size with `wc -c hello`). Make it tiny by running `./patch hello`, and see it shrink to about 170 bytes.
