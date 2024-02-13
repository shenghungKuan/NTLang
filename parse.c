/* parse.c - parsing and parse tree construction */

#include "ntlang.h"

void parse_table_init(struct parse_table_st *pt) {
    pt->len = 0;
}

struct parse_node_st * parse_node_new(struct parse_table_st *pt) {
    struct parse_node_st *np;

    np = &(pt->table[pt->len]);
    pt->len += 1;

    return np;
}

void parse_error(char *err) {
    printf("parse_error: %s\n", err);
    exit(-1);
}

char *parse_oper_strings[] = {"PLUS", "MINUS", "MULT", "DIV", "LSR", "ASR", "LSL", "NOT", "AND", "OR", "XOR"};

struct parse_oper_pair_st parse_oper_map[] = {
    {TK_PLUS, OP_PLUS},
    {TK_MINUS, OP_MINUS},
    {TK_MULT, OP_MULT},
    {TK_DIV, OP_DIV},
    {TK_LSR, OP_LSR},
    {TK_ASR, OP_ASR},
    {TK_LSL, OP_LSL},
    {TK_NOT, OP_NOT},
    {TK_AND, OP_AND},
    {TK_OR, OP_OR},
    {TK_XOR, OP_XOR},
    {TK_NONE, OP_NONE}
};

int size = (int)(sizeof(parse_oper_map) / sizeof(parse_oper_map[0]));


/* We need to provide prototypes for the parsing functions because
 * we call them before they are defined.
 */
struct parse_node_st * parse_program(struct parse_table_st *pt, 
                                        struct scan_table_st *st);
struct parse_node_st * parse_expression(struct parse_table_st *pt, 
                                        struct scan_table_st *st);
struct parse_node_st * parse_operand(struct parse_table_st *pt, 
                                        struct scan_table_st *st);

/* We need a parsing function for each rule in the EBNF grammer */

struct parse_node_st * parse_program(struct parse_table_st *pt, 
                                        struct scan_table_st *st) {
    struct parse_node_st *np1;

    /* A program is a single expression followed by EOT */
    np1 = parse_expression(pt, st);

    if (!scan_table_accept(st, TK_EOT)) {
        parse_error("Expecting EOT");
    }

    return np1;
}

struct parse_node_st * parse_expression(struct parse_table_st *pt, 
                                        struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np1, *np2;

    /* An expression must start with an operand. */
    np1 = parse_operand(pt, st);

    while (true) {
        tp = scan_table_get(st, 0);
        /* Check for valid operator */
        int i = 0;
        for(i = 0; i < size; i++){
        	if(tp->id == parse_oper_map[i].tkid){
        		scan_table_accept(st, TK_ANY);
        		np2 = parse_node_new(pt);
        		np2->type = EX_OPER2;
        		np2->oper2.oper = parse_oper_map[i].opid;
        		np2->oper2.left = np1;
        		np2->oper2.right = parse_operand(pt, st);
        		np1 = np2;
        		break;
        	}
        }

        if(i == size){
        	break;
        }
    }

    return np1;
}

struct parse_node_st * parse_operand(struct parse_table_st *pt,
                                     struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np1;

    if (scan_table_accept(st, TK_INTLIT)) {
        tp = scan_table_get(st, -1);
        np1 = parse_node_new(pt);
        np1->type = EX_INTVAL;
        /* For Project02 you need to implement your own version of atoi() */
        np1->intval.value = toValue(tp->value, 10);
    } else if(scan_table_accept(st, TK_BINLIT)){
    	tp = scan_table_get(st, -1);
    	np1 = parse_node_new(pt);
    	np1->type = EX_INTVAL;
    	np1->intval.value = toValue(tp->value, 2);
    } else if(scan_table_accept(st, TK_HEXLIT)){
       	tp = scan_table_get(st, -1);
       	np1 = parse_node_new(pt);
       	np1->type = EX_INTVAL;
       	np1->intval.value = toValue(tp->value, 16);
    } else if(scan_table_accept(st, TK_MINUS)){
    	tp = scan_table_get(st, -1);
    	np1 = parse_node_new(pt);
    	np1->type = EX_OPER1;
    	np1->oper1.oper = OP_MINUS;
    	np1->oper1.operand = parse_operand(pt, st);
    } else if(scan_table_accept(st, TK_NOT)){
       	tp = scan_table_get(st, -1);
       	np1 = parse_node_new(pt);
       	np1->type = EX_OPER1;
       	np1->oper1.oper = OP_NOT;
       	np1->oper1.operand = parse_operand(pt, st);
    } else if(scan_table_accept(st, TK_LPAREN)){
    	tp = scan_table_get(st, -1);
    	np1 = parse_expression(pt, st);
    	if(!scan_table_accept(st, TK_RPAREN)){
    		parse_error("Missing right paren");
    	}
    } else {
    	parse_error("Bad operand");
    }

    return np1;
}

void parse_tree_print_indent(int level) {
    level *= 2;
    for (int i = 0; i < level; i++) {
        printf(".");
    }
}

void parse_tree_print_expr(struct parse_node_st *np, int level) {
    parse_tree_print_indent(level);
    printf("EXPR ");

    if (np->type == EX_INTVAL) {
        printf("INTVAL %d\n", np->intval.value);
    } else if (np->type == EX_OPER1) {
        printf("OPER1 %s\n", parse_oper_strings[np->oper1.oper]);
        parse_tree_print_expr(np->oper1.operand, level+1);
    } else if (np->type == EX_OPER2) {
        printf("OPER2 %s\n", parse_oper_strings[np->oper2.oper]);
        parse_tree_print_expr(np->oper2.left, level+1);
        parse_tree_print_expr(np->oper2.right, level+1);
    }
}

void parse_tree_print(struct parse_node_st *np) {
    parse_tree_print_expr(np, 0);    
}

void parse_args(struct config_st *cp, int argc, char **argv) {
    int i = 1;

    if (argc <= 2) {
        printf("Usage: ./project01 -e \"expression\" -u -b base(2/16/10) -w width(4/8/16/32)\n");
        exit(0);
    }

    while (i < argc) {
    	if(argv[i][0] == '-'){
	        if (argv[i][1] == 'e') {
	            i += 1;
	            strncpy(cp->input, argv[i], SCAN_INPUT_LEN);
	            cp->input[SCAN_INPUT_LEN] = '\0';
	        } else if(argv[i][1] == 'b'){
	        	i += 1;
	        	cp->base = toValue(argv[i], 10);
	        } else if(argv[i][1] == 'w'){
	        	i += 1;
	        	cp->width = toValue(argv[i], 10);
	        } else if(argv[i][1] == 'u' && cp->base == 10){
	        	cp->unsign = true;
	        }
	        i += 1;
        }
    }

    if (strnlen(cp->input, SCAN_INPUT_LEN) == 0) {
        parse_error("No expression given to evaluate");
    }
}
