/*
 * engine.c
 *
 *  Created on: 2014年4月11日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "engine.h"
#include "list.h"
#include "memdb.h"
#include "objs.h"
#include "protocol.h"
#include "env.h"

#define db env.db
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

static void *ping_cmd(int argc, char **argv);
static void *get_cmd(int argc, char **argv);
static void *set_cmd(int argc, char **argv);
static void *del_cmd(int argc, char **argv);
static void *incr_cmd(int argc, char **argv);
static void *decr_cmd(int argc, char **argv);
static void *lpush_cmd(int argc, char **argv);
static void *lindex_cmd(int argc, char **argv);

static struct cmd_func_table {
	char *cmd;
	cmd_func *func;
	int argc; /*用argc校验参数个数是否匹配*/
} cmdtbl[] = { { "PING", ping_cmd, 1 }, { "GET", get_cmd, 2 }, { "SET", set_cmd,
		3 }, { "DEL", del_cmd, 0 }, { "INCR", incr_cmd, 2 }, { "INCRBY",
		incr_cmd, 3 }, { "DECR", decr_cmd, 2 }, { "DECRBY", incr_cmd, 3 }, {
		"LPUSH", lpush_cmd, 0 }, { "LINDEX", lindex_cmd, 3 } };

static void *ping_cmd(int argc, char **argv) {
	return string_new(PRO_PONG);
}

static void *get_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);
	if (o == NULL) {
		return string_new(PRO_NULL_DATA);
	} else if (o->ot == INT || o->ot == STRING)
		return object_to_pro_string(o);
	else
		return string_format(PRO_TYPE_ERROR, argv[1]);
}

static void *set_cmd(int argc, char **argv) {
	struct object *o;
	char *str = str_to_str(argv[2]);
	o = create_object(str, STRING);
	o = memdb_set(db, argv[1], o);
	if (o) {
		delete_object(o);
	}
	return string_new(PRO_OK);
}
/*
 *
 * 一次可以删除多个key
 *ss
 */
static void *del_cmd(int argc, char **argv) {
	int cnt = 0;
	int i;
	for (i = 1; i < argc; ++i) {
		struct object *o = memdb_del(db, argv[i]);
		if (o) {
			cnt++;
			delete_object(o);
		}
	}
	return string_format(PRO_INTEGER_DATA, cnt);
}

static void *incr_cmd(int argc, char **argv) {
	char *stat = 0;
	struct object *o = memdb_get(db, argv[1]);
	if (o == NULL) {
		int_t n = (int_t) 1;
		o = create_object(&n, INT);
		memdb_set(db, argv[1], o);
		return string_format(PRO_INTEGER_DATA, o->val.num);
	} else if (o->ot == INT) {
		o->val.num++;
		return string_format(PRO_INTEGER_DATA, o->val.num);
	} else if (o->ot == STRING) {
		int_t n = (int_t) strtol((const char *) o->val.add, &stat, 10);
		if (*stat != '\0') {
			return string_format(PRO_ERROR_DATA, "无法将字符串转换为整形");
		}
		free(o->val.add);
		o->ot = INT;
		o->val.num = n + 1;
		return string_format(PRO_INTEGER_DATA, o->val.num);
	} else
		return string_format(PRO_TYPE_ERROR, argv[1]);

}

static void *decr_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);
	if (o == NULL) {
		int_t n = (int_t) -1;
		o = create_object(&n, INT);
		memdb_set(db, argv[1], o);
		return string_format(PRO_INTEGER_DATA, o->val.num);
	} else if (o->ot == INT) {
		o->val.num--;
		return string_format(PRO_INTEGER_DATA, o->val.num);
	} else if (o->ot == STRING) {
		char *stat = 0;
		int_t n = (int_t) strtol((const char *) o->val.add, &stat, 10);
		if (*stat != '\0') {
			return string_format(PRO_ERROR_DATA, "无法将字符串转换为整形");
		}
		free(o->val.add);
		o->ot = INT;
		o->val.num = n - 1;
		return string_format(PRO_INTEGER_DATA, o->val.num);
	}
	return string_format(PRO_TYPE_ERROR, argv[1]);

}
/*
 *
 *
 * 如果链表不存在 创建新链表并将元素加入 返回当前链表的元素数目
 */
static void *lpush_cmd(int argc, char **argv) {
	struct list *ls;
	struct object *o = memdb_get(db, argv[1]);

	if (o == NULL) {
		/*create list*/
		ls = strs_to_list(NULL, argc - 1, (argv + 1));
		/*strs2list(NULL, argc) ;*/
		o = create_object(ls, LIST);
		memdb_set(db, argv[1], o);
		return string_format(PRO_INTEGER_DATA, (long long int) ls->size);
	} else if (o->ot == LIST) {
		ls = (struct list *) o->val.add;
		strs_to_list(ls, argc - 1, argv + 1);
		return string_format(PRO_INTEGER_DATA, (long long int) ls->size);
	}
	return string_format(PRO_TYPE_ERROR, argv[1]);
}

static void *lindex_cmd(int argc, char **argv) {
	struct object *o = memdb_get(db, argv[1]);

	if (o == NULL) {
		return string_new(PRO_NULL_DATA);
	} else if (o->ot != LIST) {
		return string_format(PRO_TYPE_ERROR, argv[1]);
	} else {
		struct list *ls = (struct list *) o->val.add;
		char *stat = NULL;
		long int idx = strtol(argv[2], &stat, 10);
		if (stat || idx < 0)
			return string_new(PRO_ARG_ILLEGAL);
		size_t pos = (size_t) idx;
		char *e = (char *) list_get(ls, pos);
		return (e == NULL) ?
				string_new(PRO_NULL_DATA) :
				string_format(PRO_STRING_DATA, (int) strlen(e), e);

	}
}
/*
 *
 * -1 未找到 -2 参数不匹配
 * argc == 0时 忽略参数数目比较
 *
 */
static int find_func_pos(const char *cmd, int argc) {
	int count = sizeof(cmdtbl) / sizeof(struct cmd_func_table);
	int i = 0;
	for (i = 0; i < count; ++i) {
		if (strcasecmp(cmdtbl[i].cmd, cmd) == 0) {
			return (cmdtbl[i].argc == 0 || cmdtbl[i].argc == argc) ? i : -2;
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
struct string *execute(struct command *cmd) {
	int pos = find_func_pos(cmd->argv[0], cmd->argc);
	if (pos >= 0)
		return cmdtbl[pos].func(cmd->argc, cmd->argv);
	else if (pos == -1)
		return string_format("-ERROR 不支持的命令 '%s'\r\n", cmd->argv[0]);
	else
		return string_new(PRO_ARG_ILLEGAL);
}

