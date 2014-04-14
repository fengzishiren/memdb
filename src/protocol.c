/*
 * protocol.c
 *
 *  Created on: 2014年4月11日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "protocol.h"

/*
 *
 * *3 \r\n $3 \r\n SET \r\n $5 \r\n mykey\r\n $7 \r\n myvalue \r\n
 *
 */

struct command * parse_to_command(char *data) {
	log_debug("正在解析协议数据。。。");
	struct command *args = malloc(sizeof(struct command));
	char *p = data;

	if (*p != '*') {
		log_error("解析错误！未期待的'*'");
		goto err;
	}

	while (*p) {
		if (*p == '\r' && *(p + 1) == '\n')
			break;
		p++;
	}
	if (*p == '\0') {
		log_error("解析错误！提前结束");
		goto err;
	}
	*p = '\0';
	p += 2;

	args->argc = (int) strtol(data + 1, NULL, 10);
	if (args->argc < 1)
		goto err;
	args->argv = malloc(sizeof(char *) * args->argc);

	char *tok;
	int i;
	for (i = 0; (tok = strtok(p, "\r\n")); ++i) {
		if (*tok != '$') {
			log_error("解析错误！必须以$开头");
			goto err;
		}

		int len = (int) strtol(tok + 1, NULL, 10);
		tok = strtok(NULL, "\r\n");
		if (tok == NULL || strlen(tok) != len)
			goto err;

		if (i >= args->argc) {
			log_error("解析错误！参数长度与实际不匹配");
			goto err;
		}

		args->argv[i] = tok;

		if (p)
			p = NULL;
	}

	if (i != args->argc) {
		log_error("解析错误！参数长度与实际不匹配");
		goto err;
	}

	return args;

	err: return NULL;
}

/*
 *
 *
 * *5\r\n
 * :1\r\n
 * :2\r\n
 * :3\r\n
 * :4\r\n
 * $6\r\n
 * foobar\r\n
 *
 */
struct string *list_to_pro_string(struct list *ls) {
	char buf[20];

	struct string *s = string_new(PRO_MULTI);
	sprintf(buf, "%lu", (ls->size));
	s = string_concat(s, buf);
	s = string_concat(s, PRO_NEW_LINE);

	struct list_iter it;
	list_iter(ls, &it);
	struct list_entry *e;
	while ((e = list_next(&it)) != NULL) {
		char *item = (char *) e->data;
		s = string_concat(s, PRO_BULK);
		sprintf(buf, "%lu", strlen(item));
		s = string_concat(s, buf);
		s = string_concat(s, PRO_NEW_LINE);
		s = string_concat(s, item);
		s = string_concat(s, PRO_NEW_LINE);
	}
	return s;
}

struct string *object_to_pro_string(struct object *o) {

	struct string *str = NULL;
	switch (o->ot) {
	case INT:
		str = string_format(PRO_INTEGER_DATA, (long long int) o->val.num);
		break;
	case STRING:
		/*$6\r\nfoobar\r\n"*/
		/*char *s = (const char *) o->val.add;*/
		str = string_format(PRO_STRING_DATA,
				(int) strlen((const char *) o->val.add),
				(const char *) o->val.add);
		break;
	case LIST:
		str = list_to_pro_string((struct list *) o->val.add);
		break;
	default:
		break;
	}
	return str;
}
