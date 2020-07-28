#include "elf_parser.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>

int elf_fd = -1;
void* elf_mem = NULL;
int file_size = 0;

int load_file(const char* file){
    int ret = -1;
    struct stat buf;
    
    do{
                elf_fd = open(file, O_RDONLY);
                    if(elf_fd == -1)
                        break;
    if(fstat(elf_fd,  &buf) == -1)
        break;
    
    file_size = buf.st_size;
    
    elf_mem = mmap(NULL, buf.st_size, PROT_READ,  MAP_SHARED, elf_fd, 0);
    
    }while(false);
        
    if(elf_mem == NULL){
        close(elf_fd);
    }else
        ret = 0;
    
    return ret;
}

Elf32_Ehdr* get_ehdr(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    return e;
}

Elf32_Shdr* get_shdr(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    return s;
}

Elf32_Phdr* get_phdr(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Phdr* p = (Elf32_Phdr*)((char*)e + e->e_phoff);
    return p;
}

const char* get_dym_str(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    int i=0;
    for(;i<e->e_shnum;++i)
    {
        if(s[i].sh_type == SHT_DYNSYM)
            break;
    }
    return (const char*)elf_mem + s[i].sh_offset;
}

const char* get_sh_str(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    return (const char*)elf_mem + s[e->e_shstrndx].sh_offset;
}

void parse_elf_header(){
    Elf32_Ehdr* p = get_ehdr();
}

void parse_scetions_header(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    const char* str_table;
    
    /*//错误代码，第一个strtab，是动态符号的
    for(int i=0; i<e->e_shnum;++i){
        if(s[i].sh_type == SHT_STRTAB){
            str_table = (const char*)e + s[i].sh_offset;
            break;
        }
    }*/
    str_table = (const char*)e + s[e->e_shstrndx].sh_offset;
    
    printf("[Nr] Name        Type        Addr        Off        Size        ES        Flg        Lk        Inf        Al");
    for(int i=0; i<e->e_shnum;++i){
        printf("[%d] %s 0x%x 0x%x 0x%x 0x%x\n", i, str_table + s[i].sh_name, s[i].sh_type, s[i].sh_addr, s[i].sh_offset, s[i].sh_size);
    }
}

void parse_symbol_table(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    const char* dyn_str_table;
    Elf32_Sym* dyn_sym;
    int dyn_sym_index=0;
    
    for(int i=0; i<e->e_shnum;++i){
        if(s[i].sh_type == SHT_STRTAB){
            dyn_str_table = (const char*)e + s[i].sh_offset;
            break;
        }
    }
    
    for(dyn_sym_index=0; dyn_sym_index<e->e_shnum;++dyn_sym_index){
        if(s[dyn_sym_index].sh_type == SHT_DYNSYM){
            dyn_sym = (Elf32_Sym*)( (const char*)e + s[dyn_sym_index].sh_offset);
            break;
        }
    }
    
    int num = s[dyn_sym_index].sh_size / s[dyn_sym_index].sh_entsize;
    printf("[num]     name    value    size    type    bind    other    shndx\n");
    for(int i=0; i<num;++i){
        printf("[%d] %s 0x%x 0x%x 0x%x 0x%x 0x%x %d\n", i, dyn_str_table + dyn_sym[i].st_name,  dyn_sym[i].st_value, dyn_sym[i].st_size, dyn_sym[i].st_info, dyn_sym[i].st_info, dyn_sym[i].st_other, dyn_sym[i].st_shndx);
    }
}

void parse_all_string_table(){
    Elf32_Ehdr* e = (Elf32_Ehdr*)elf_mem; 
    Elf32_Shdr* s = (Elf32_Shdr*)((char*)e + e->e_shoff);
    const char* str_table;
    
    printf("print all string. 打印所有字符串表\n");
    for(int i=0; i<e->e_shnum;++i){
        if(s[i].sh_type == SHT_STRTAB){
            printf("string table index=%d\n", i);
             int num = 0;
            str_table = (const char*)e + s[i].sh_offset;
            const char* str_end = str_table + s[i].sh_size;
            while(str_table < str_end){
                printf("[%d] %s\n", num++, str_table);
                str_table = str_table + strlen(str_table) + 1;
            }
        }
    }
}

static unsigned long elf_hash(const unsigned char *_name)
{
    const unsigned char *name = (const unsigned char *) _name;
    unsigned h = 0, g;
    
    while(*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}


void parse_hash_table(){
}













/////////////////////////////////////////////////////动态部分
void draw_load_view()
{
    Elf32_Ehdr* e = get_ehdr();
    Elf32_Phdr* p = get_phdr();
    printf("file 0x%x 0x%x\n", 0, file_size);
    
    printf("elf_header 0x%x 0x%x\n", 0, e->e_ehsize);
    
    printf("program_table 0x%x 0x%x\n", e->e_phoff, e->e_phoff + e->e_phnum * e->e_phentsize);
    
    printf("segment_table 0x%x 0x%x\n", e->e_shoff, e->e_shoff + e->e_shnum * e->e_shentsize);
    
    Elf32_Shdr* s = get_shdr();
    const char* sh_str = get_sh_str();
    for(int i=0; i<e->e_shnum; ++i)
    {
        if(s[i].sh_name != 0)
          printf("%s 0x%x 0x%x\n", sh_str + s[i].sh_name, s[i].sh_offset, s[i].sh_offset + s[i].sh_size);
    }
    
        for(int i=0; i<e->e_phnum; ++i)
    {
        if(p[i].p_type == PT_LOAD)
        {
            printf("load_off%d 0x%x 0x%x\n", i, p[i].p_offset, p[i].p_offset + p[i].p_filesz);
            printf("load_vir%d 0x%x 0x%x\n", i, p[i].p_vaddr, p[i].p_vaddr + p[i].p_memsz);
        }
        
        if(p[i].p_type == PT_DYNAMIC)
        {
            printf("dynamic_Segment_off 0x%x 0x%x\n", p[i].p_offset, p[i].p_offset + p[i].p_filesz);
            printf("dynamic_Segment_vir 0x%x 0x%x\n", p[i].p_vaddr, p[i].p_vaddr + p[i].p_memsz);
        }
    }
}
