/* eval.c - tree evaluation and value printing */

#include "ntlang.h"

void eval_error(char *err) {
    printf("eval_error: %s\n", err);
    exit(-1);
}

uint32_t eval(struct parse_node_st *pt) {
    uint32_t v1, v2;

    if (pt->type == EX_INTVAL) {
        v1 = pt->intval.value;
    } else if (pt->type == EX_OPER1) {
        v1 = eval(pt->oper1.operand);
        if (pt->oper1.oper == OP_MINUS) {
            v1 = -v1;
        } else if(pt->oper1.oper == OP_NOT){
        	v1 = ~v1;
        } else {
            eval_error("Bad operator");
        }
    } else if (pt->type == EX_OPER2) {
        v1 = eval(pt->oper2.left);
        v2 = eval(pt->oper2.right);
        if (pt->oper2.oper == OP_PLUS) {
            v1 = v1 + v2;
        } else if (pt->oper2.oper == OP_MINUS) {
            v1 = v1 - v2;
        } else if (pt->oper2.oper == OP_MULT) {
            v1 = v1 * v2;
        } else if (pt->oper2.oper == OP_DIV) {
            v1 = v1 / v2;
        } else if (pt->oper2.oper == OP_LSR) {
        	v1 = v1 >> v2;
    	} else if (pt->oper2.oper == OP_LSL) {
			v1 = v1 << v2;
		} else if (pt->oper2.oper == OP_AND) {
			v1 = v1 & v2;
		} else if (pt->oper2.oper == OP_OR) {
			v1 = v1 | v2;
		} else if (pt->oper2.oper == OP_XOR) {
			v1 = v1 ^ v2;
		} else if(pt->oper2.oper == OP_ASR){
			if(v1 >= 2147483648){
			for(int i = 0; i < (int)v2 && i < 32; i ++){
					v1 = v1 >> 1;
					v1 = v1 | 2147483648;
				}
			} else{
				v1 = v1 >> v2;
			}
		} else {
            eval_error("Bad operator");            
        }
    }

    return v1;
}

void eval_print(struct config_st *cp, uint32_t value) {
    /*
     * Handle -b -w -u
     *
     * Use your own conversion functions for uint32_t to string.
     */

	char tmp[OUTPUT_LEN];
	int i = 0;
	int digit;
	int base = cp->base;
	int width = cp->width;
	uint32_t mask;
	uint32_t org_value = value;
	bool negative = false;
	
	if(width == 4 || width == 8 || width == 16){
		mask = (1 << width) - 1;
	} else if(width == 32){
		mask = 4294967295;
	} else{
		eval_error("Invalid width");
	}
	value = mask & value;

	if(base == 10 && !cp->unsign  && (value & (1 << (width - 1)))){
		negative = true;
		value = (-org_value) & mask;
	}

	while(value > 0){
		digit = value % base;
		if(digit >= 10){
			tmp[i] = digit - 10 + 'A';
		} else{
			tmp[i] = digit + '0';
		}
		value /= base;
		i++;
	}

	if(base == 2){
		for(i = i; i < width; i++){
			tmp[i] = '0';
		}
		
		tmp[i] = '\0';
	
		for(int j = 0; j < i; j++){
			cp->output[j] = tmp[i - j - 1];
		}
	
		cp->output[i] = '\0';
		printf("0b%s", cp->output);
	} else if(base == 16){
		for(i = i; i < width / 4; i++){
			tmp[i] = '0';
		}
		
		tmp[i] = '\0';
	
		for(int j = 0; j < i; j++){
			cp->output[j] = tmp[i - j - 1];
		}
	
		cp->output[i] = '\0';
		printf("0x%s", cp->output);
	} else if(base == 10){
		for(int j = 0; j < i; j++){
			cp->output[j] = tmp[i - j - 1];
		}

		if(i == 0){
			cp->output[i] = '0';
			i++;
		}
		cp->output[i] = '\0';

		if(negative){
			printf("-%s", cp->output);
		} else{
			printf("%s", cp->output);
		}

	} else{
		eval_error("Invalid base");
	}
    printf("\n");

	
	
}
