#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

#include "elf_parser.hpp"

void help(){
    printf("-H, Show help info\n");
    printf("-f elfpath, elf file path\n");
    printf("-h, Display the ELF file header， 显示ELF文件头\n");
    printf("-S, Display the section's header，显示节头\n");
    printf("-s, Display the symbol table，显示符号表（动态符号表）\n");
    printf("-g, Display all string，显示所有字符串表\n");
    printf("-F symbol, Dump Function code，显示指定函数的机器码\n");
    printf("-G, Display hash table，显示指定函数的机器码\n");
    printf("-V，绘制ELF文件布局视图\n");
}

int main(int argc, const char* argv[])
{
    const char* optstring = "Hhf:SsgF:GLV";
    int option_index = 0;
       static struct option long_options[] = {  
       {"reqarg", required_argument, NULL, 'r'},  
       {"noarg",  no_argument,       NULL, 'n'},  
       {"optarg", optional_argument, NULL, 'o'},  
       {0, 0, 0, 0}  
   };

    int opt;
    while((opt = getopt_long(argc,  (char* const *)argv, optstring, long_options, &option_index)) != -1)
    {
        switch(opt)
        {
            case '?':
            case 'H':
                help();
                break;
            case 'f':
                //printf("input file name %s\n", optarg);
                if(load_file(optarg) == 0)
                    ;//printf("load elf %s ok\n", optarg);
                else
                    printf("load elf %s failed\n", optarg );
                break;
            case 'S':
                parse_scetions_header();
                break;
            case 's':
                parse_symbol_table();
                break;
            case 'g':
                parse_all_string_table();
                break;
            case 'F':
                break;
            case 'G':
                parse_hash_table();
                break;
            case 'V':
                draw_load_view();
                break;
            case 'h':
                break;
            default:
                break;
        }
    }
    
    return 0;
}
