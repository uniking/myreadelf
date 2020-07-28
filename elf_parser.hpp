#ifndef ELF_PARSER_HPP
#define ELF_PARSER_HPP
int load_file(const char* file);
void parse_scetions_header();
void parse_symbol_table();
void parse_all_string_table();
void parse_hash_table();
void draw_load_view();
#endif
