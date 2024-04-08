/* eval.c - tree evaluation and value printing */

#include "ntlang.h"
/*prints error messages*/
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
			if(v1 >> 31){
				v1 = (v1 >> v2) | ~(uint32_t)0 << (32 - v2);
			} else{
				v1 = v1 >> v2;
			}
		} else {
            eval_error("Bad operator");            
        }
    }

    return v1;
}

void eval_to_string(struct config_st *cp, uint32_t value){

	char tmp[OUTPUT_LEN];
	int i = 0;
	int digit;
	int base = cp->base;
	int width = cp->width;
	uint32_t mask;
	uint32_t org_value = value;
	bool negative = false;

	// masking for different width
	if(width == 4 || width == 8 || width == 16){
		mask = (1 << width) - 1;
	// 32-bit mask overflows so it needs to be handled seperately
	} else if(width == 32){
		mask = 4294967295;
	} else{
		eval_error("Invalid width");
	}
	value = mask & value;

	// handle signed value with base of 10
	if(base == 10 && !cp->unsign  && (value & (1 << (width - 1)))){
		negative = true;
		value = (-org_value) & mask;
	}

	// converts the uint32_t value into a string representation
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

	// handle the printing format for different bases
	if(base == 2){
		for(i = i; i < width; i++){
			tmp[i] = '0';
		}
		
		tmp[i] = '\0';
	
		cp->output[0] = '0';
		cp->output[1] = 'b';
		for(int j = 2; j < i + 2; j++){
			cp->output[j] = tmp[i - j + 1];
		}
	
		cp->output[i + 2] = '\0';
	} else if(base == 16){
		for(i = i; i < width / 4; i++){
			tmp[i] = '0';
		}
		
		tmp[i] = '\0';

		cp->output[0] = '0';
		cp->output[1] = 'x';
		for(int j = 2; j < i + 2; j++){
			cp->output[j] = tmp[i - j + 1];
		}
	
		cp->output[i + 2] = '\0';
	} else if(base == 10){
		if(i == 0){
			cp->output[0] = '0';
			i++;
		} else if(negative){
			cp->output[0] = '-';
			for(int j = 1; j < i + 1; j++){
				cp->output[j] = tmp[i - j];
			}
			i++;
		} else{
			for(int j = 0; j < i; j++){
				cp->output[j] = tmp[i - j - 1];
			}
		}

		cp->output[i] = '\0';

	} else{
		eval_error("Invalid base");
	}
}

void eval_print(struct config_st *cp, uint32_t value) {
    /*
     * Handle -b -w -u
     *
     * Use your own conversion functions for uint32_t to string.
     */

	eval_to_string(cp, value);
	printf("%s\n", cp->output);

}
