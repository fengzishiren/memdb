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
#include "memdb.h"
#include "string.h"
#include "log.h"
#include "env.h"

#define SERVER_LOGO ""\
		"古之立大事者 不惟有超世之才 亦必有坚忍不拔之志\n"\
		"                              ————苏轼\n"
struct global_environ env = { .log_closed = 0, .lv = DEBUG, .dbcnt = 10, .db =
		NULL, .version = "1.0.0 beta", .name = "还没想好呢" };

static void init_environ() {
	/*configure env from config file*/
	printf("NoSQL %s \nversion %s\n", env.name, env.version);
	if (env.log_closed || env.lv == CLOSE) {
		log_init(CLOSE, NULL, 0);
		puts("警告！未开启日志功能");
	} else {
		log_init(env.lv, NULL, 0);
		printf("开启日志功能 级别 %s\n", level2str(env.lv));
	}
	env.db = memdb_new(env.dbcnt);
	printf("使用数据库数目:%zu\n", env.dbcnt);
	/**/

	printf("环境初始化完毕！\n\n");
}

int main(void) {

	puts(SERVER_LOGO);
	init_environ();

	log_debug("启动。。。");

	struct epoll *ep = open_epoll(1234);
	start_epoll(ep);
	close_epoll(ep);

	puts("END");
	return EXIT_SUCCESS;
}
