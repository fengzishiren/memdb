/*
 * command.c
 *
 *  Created on: 2014年4月11日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transport.h"
/**
 *
 *
 *
 String
 List
 Set
 SortedSet

 INT
 STRING
 LIST
 SET
 SSET

 *
 *
 *
 *
 */
typedef void *cmd_func(int argc, char **argv);

static void *get_cmd(int argc, char **key);
static void *set_cmd(int argc, char **key);

static struct cmd_table {
	char *cmd;
	cmd_func *func;
	int argc;
} cmdtbl[] = { { "GET", get_cmd, 2 }, { "SET", set_cmd, 3 } };

static void *get_cmd(int argc, char **key) {
	return get_obj(key[0], STRING);
}

static void *set_cmd(int argc, char **key) {
	set_obj(key[0], strdup(key[1]), STRING);
	return NULL;
}
/*

 static cmd_func *find_func(const char *cmdname) {
 int count = sizeof(cmdtbl) / sizeof(struct cmd_table);
 int i = 0;
 for (i = 0; i < count; ++i) {
 if (strcasecmp(cmdtbl[i].cmd, cmdname) == 0)
 return cmdtbl[i].func;
 }
 return NULL;
 }
 */
/*
 *
 * -1 未找到 -2 参数不匹配
 *
 */
static int find_func_pos(const char *cmd, int argc) {
	int count = sizeof(cmdtbl) / sizeof(struct cmd_table);
	int i = 0;
	for (i = 0; i < count; ++i) {
		if (strcasecmp(cmdtbl[i].cmd, cmd) == 0) {
			if (cmdtbl[i].argc == argc)
				return i;
			else
				return -2;
		}
	}
	return -1;
}
void execute(int argc, char **argv) {

	int pos = find_func_pos(argv[0], argc);
	if (pos >= 0)
		cmdtbl[pos].func(argc - 1, argv + 1);
}

