/*
 * command.c
 *
 *  Created on: 2014年4月11日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objcol.h"
#include "protocol.h"
#include "transport.h"
#include "utils.h"
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

struct memdb memdb;
struct memdb *db = &memdb;

typedef void *cmd_func(int argc, char **argv);

static void *get_cmd(int argc, char **argv);
static void *set_cmd(int argc, char **argv);
static void *del_cmd(int argc, char **argv);

static struct cmd_table {
	char *cmd;
	cmd_func *func;
	int argc;
} cmdtbl[] = { { "GET", get_cmd, 2 }, { "SET", set_cmd, 3 },
		{ "DEL", del_cmd, 0 }, { "INCR", incr_cmd, 2 },
		{ "INCRBY", incr_cmd, 3 }, { "DECR", decr_cmd, 2 }, { "DECRBY",
				incr_cmd, 3 } };

static void *get_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);
	if (o == NULL) {
		return string_new(PRO_NULL);
	} else if (o->ot == INT || o->ot == STRING)
		return obj2prostr(o, STRING);
	else
		return string_format(PRO_TYPE_ERROR, argv[1]);
}

static void *set_cmd(int argc, char **argv) {
	struct object *o = str2object(strdup(argv[2]), STRING);
	memdb_set(db, argv[1], o);
	return string_new(PRO_OK);
}
/*
 *
 * 一次可以删除多个key
 *
 */
static void *del_cmd(int argc, char **argv) {
	int cnt = 0;
	int i;
	for (i = 1; i < argc; ++i) {
		struct object *o = memdb_del(db, argv[i]);
		if (o) {
			cnt++;
			del_obj(o);
		}
	}
	return string_format(PRO_INTEGER_DATA, cnt);
}

static void *incr_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);
	if (o->ot == INT) {
		o->val.num++;
	} else if (o->ot == STRING) {
		int stat = 0;
		int_t n = (int_t) str2int((const char *) o->val.add, stat);
		if (stat == 0) {
			free(o->val.add);
			o->ot = INT;
			o->val.num = n + 1;
			return string_format(PRO_INTEGER_DATA, o->val.num);
		}
	}
	return string_format(PRO_TYPE_ERROR, argv[1]);

}

static void *decr_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);
	if (o->ot == INT) {
		o->val.num++;
	} else if (o->ot == STRING) {
		int stat = 0;
		int_t n = (int_t) str2int((const char *) o->val.add, stat);
		if (stat == 0) {
			free(o->val.add);
			o->ot = INT;
			o->val.num = n - 1;
			return string_format(PRO_INTEGER_DATA, o->val.num);
		}
	}
	return string_format(PRO_TYPE_ERROR, argv[1]);

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
 * argc == 0时 忽略参数数目比较
 *
 */
static int find_func_pos(const char *cmd, int argc) {
	int count = sizeof(cmdtbl) / sizeof(struct cmd_table);
	int i = 0;
	for (i = 0; i < count; ++i) {
		if (strcasecmp(cmdtbl[i].cmd, cmd) == 0) {
			if (argc == 0 || cmdtbl[i].argc == argc)
				return i;
			else
				return -2;
		}
	}
	return -1;
}
/*
 *
 * argc: param count
 * argv: cmd key arg1 arg2 ....
 *
 * 返回一个新的string类型的数据
 * 需要free释放
 *
 */
struct string *execute(int argc, char **argv) {
	int pos = find_func_pos(argv[0], argc);
	if (pos >= 0)
		return cmdtbl[pos].func(argc, argv);
	else if (pos == -1)
		return string_format("-ERROR unknown command '%s'\r\n", argv[0]);
	else
		return string_new("-ERROR illegal arguments\r\n");
}

