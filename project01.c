/* project01.c - initial parsing implemenation */

#include "ntlang.h"

int main(int argc, char **argv) {
    struct config_st config;
    struct scan_table_st scan_table;
    struct parse_table_st parse_table;
    struct parse_node_st *parse_tree;
    uint32_t value;
    
    config.base = 10;
    config.unsign = false;
    config.width = 32;
    parse_args(&config, argc, argv);

    scan_table_init(&scan_table);
    scan_table_scan(&scan_table, config.input);
    
    parse_table_init(&parse_table);
    parse_tree = parse_program(&parse_table, &scan_table);
    
    value = eval(parse_tree);
    eval_print(&config, value);

    return 0;
}
