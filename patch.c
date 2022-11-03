#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long read_file(const char *name, void **out) {
    FILE *fp = fopen(name, "r");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void *buf = malloc(size);
    if (!buf) {
        fclose(fp);
        return -1;
    }
    long nread = fread(buf, 1, size, fp);
    if (nread != size) {
        fclose(fp);
        free(buf);
        return -1;
    }
    *out = buf;
    fclose(fp);
    return size;
}

static long write_file(const char *name, void *buf, long size) {
    FILE *fp = fopen(name, "w");
    if (!fp) return -1;
    long nwrite = fwrite(buf, 1, size, fp);
    fclose(fp);
    return nwrite;
}

static long patch_segment(unsigned char *elf) {
    Elf64_Ehdr *const ehdr = (Elf64_Ehdr *)elf;
    Elf64_Phdr *const phdr = (Elf64_Phdr *)&elf[ehdr->e_phoff];
    int text_idx = -1;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
            text_idx = i;
            break;
        }
    }
    if (text_idx < 0) {
        fprintf(stderr, "Warning: No executable segment found\n");
        return -1;
    }
    if (text_idx != 0) {
        memcpy(&phdr[0], &phdr[text_idx], ehdr->e_phentsize);
    }
    ehdr->e_phnum = 1;

    const Elf64_Off base_off     = 0x10000;
    const Elf64_Off text_off_new = 0x78;
    const Elf64_Off text_off_old = phdr[0].p_offset;
    phdr[0].p_align = 8;
    phdr[0].p_offset = text_off_new;
    phdr[0].p_vaddr = base_off + text_off_new;
    phdr[0].p_paddr = base_off + text_off_new;
    memcpy(&elf[text_off_new], &elf[text_off_old], phdr[0].p_filesz);

    ehdr->e_entry = base_off + text_off_new;

    ehdr->e_shentsize = 0;
    ehdr->e_shoff = 0;
    ehdr->e_shnum = 0;
    ehdr->e_shstrndx = SHN_UNDEF;

    return text_off_new + phdr[0].p_filesz;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s elf_file\n", argv[0]);
        return 1;
    }
    void *in = NULL;
    long size = read_file(argv[1], &in);
    if (size < 0) {
        fprintf(stderr, "Error reading '%s'\n", argv[1]);
        return 1;
    }
    long patched_size  = patch_segment(in);
    if (patched_size < 0)
        return 1;
    long nwr = write_file(argv[1], in, patched_size);
    if (nwr != patched_size) {
        fprintf(stderr, "Error writing '%s'\n", argv[1]);
        return 1;
    }
    free(in);
    return 0;
}
