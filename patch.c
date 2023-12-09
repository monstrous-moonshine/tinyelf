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
        fprintf(stderr, "Error: No executable segment found\n");
        return -1;
    }
    Elf64_Phdr *p_text = &phdr[text_idx];
    if (ehdr->e_entry < p_text->p_vaddr || ehdr->e_entry >= p_text->p_vaddr + p_text->p_memsz) {
        fprintf(stderr, "Error: Entry point not in executable segment\n");
        return -1;
    }
    if (text_idx != 0) {
        memcpy(&phdr[0], p_text, ehdr->e_phentsize);
        p_text = &phdr[0];
    }
    ehdr->e_phnum = 1;

    /* /proc/sys/vm/mmap_min_addr = 0x10000 */
    const Elf64_Off base_off     = 0x10000;
    const Elf64_Off text_off_new = sizeof *ehdr + sizeof *phdr;
    const Elf64_Off text_off_old = p_text->p_offset;
    const Elf64_Off entry_off    = ehdr->e_entry - p_text->p_vaddr;
    p_text->p_align = 1;
    p_text->p_offset = text_off_new;
    p_text->p_vaddr = base_off + text_off_new;
    p_text->p_paddr = base_off + text_off_new;
    memmove(&elf[text_off_new], &elf[text_off_old], p_text->p_filesz);

    ehdr->e_entry = p_text->p_vaddr + entry_off;
    ehdr->e_shentsize = 0;
    ehdr->e_shoff = 0;
    ehdr->e_shnum = 0;
    ehdr->e_shstrndx = SHN_UNDEF;

    return text_off_new + p_text->p_filesz;
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
