#include "ntlang.h"

void conversion_error(char* err){
	printf("conversion error: %s", err);
}

uint32_t toValue(char* str, int base){
	int size = strlen(str);
	uint32_t value = 0;
	uint32_t prev;
	int digit;
	for(int i = 0; i < size; i++){
		if(str[i] >= '0' && str[i] <= '9'){
			digit = str[i] - '0';
		} else if(str[i] >= 'a' && str[i] <= 'f'){
			digit = str[i] - 'a' + 10;
		} else if(str[i] >= 'A' && str[i] <= 'F'){
			digit = str[i] - 'A' + 10;
		} else{
			conversion_error("invalid digit");
			break;
		}
		prev = value;
		value = value * base + digit;
		if(prev > value){
			printf("overflows uint32_t: %s\n", str);
			exit(-1);
		}
		
	}
	return value;
}
