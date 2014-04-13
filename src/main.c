/*
 ============================================================================
 Name        : main.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "epoll.h"
#include "log.h"
#include "string.h"

#define SERVER_LOGO ""\
		"古之立大事者 不惟有超世之才 亦必有坚忍不拔之志\n"\
		"                              ————苏轼\n"

int main(void) {
	log_init(DEBUG, NULL, 0);
	puts(SERVER_LOGO);
	log_debug("启动。。。。");
	struct epoll *ep = epoll_open(8014);
	epoll_loop(ep);
	epoll_close(ep);
	puts("END");
	return EXIT_SUCCESS;
}
