/*
 ============================================================================
 Name        : ctest.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

enum ss {FF};
struct bank {
	void *key;
	union {
		void *val;
		int v;
		char bu[323];
	} val;
} bank;

int main(void) {
	bank.val.v = 10;
	printf("%d\n", sizeof(sizeof(enum ss)));
	return EXIT_SUCCESS;
}
