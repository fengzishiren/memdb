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

static struct command cmd;

struct command * parse_to_command(char *data) {
	log_debug("正在解析协议数据。。。");
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
	if (*p == '\0') {
		log_error("解析错误！提前结束");
		goto err;
	}
	cmd.argc = (int) strtol(data + 1, NULL, 10);
	if (cmd.argc < 1)
		goto err;
	cmd.argv = malloc(sizeof(char *) * cmd.argc);

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

		if (i >= cmd.argc) {
			log_error("解析错误！参数长度与实际不匹配");
			goto err;
		}

		cmd.argv[i] = tok;

		if (p)
			p = NULL;
	}

	if (i != cmd.argc) {
		log_error("解析错误！参数长度与实际不匹配");
		goto err;
	}

	return &cmd;

	err: free(cmd.argv);
	return NULL;
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
struct string *list_to_pro_string(struct list *ls, size_t start, size_t end) {
	char buf[20];

	struct string *s = string_new(PRO_MULTI);
	sprintf(buf, "%zu", (ls->size));
	s = string_concat(s, buf);
	s = string_concat(s, PRO_NEW_LINE);

	struct list_iter it;
	list_iter(ls, &it);
	struct list_entry *e;

	int index = 0;
	while ((e = list_next(&it)) != NULL) {
		if (index++ < start)
			continue;
		char *item = (char *) e->data;
		s = string_concat(s, PRO_BULK);
		sprintf(buf, "%zu", strlen(item));
		s = string_concat(s, buf);
		s = string_concat(s, PRO_NEW_LINE);
		s = string_concat(s, item);
		s = string_concat(s, PRO_NEW_LINE);
		if (end >= -1 && index >= end)
			break;
	}
	return s;
}

struct string *integer_to_pro_string(struct object *o) {
	char buf[20];
	sprintf(buf, "%lld", (long long int) o->val.num);
	return string_format(PRO_STRING_DATA, strlen(buf), buf);
}


struct string *string_to_pro_string(struct object *o) {
	return string_format(PRO_STRING_DATA, strlen((const char *) o->val.add),
					(const char *) o->val.add);
}
/*
struct string *object_to_pro_string(struct object *o) {

	struct string *str = NULL;
	char buf[20];

	switch (o->ot) {
	case INT:
		sprintf(buf, "%lld", (long long int) o->val.num);
		str = string_format(PRO_STRING_DATA, strlen(buf), buf);
		break;
	case STRING:
		$6\r\nfoobar\r\n"
		char *s = (const char *) o->val.add;
		str = string_format(PRO_STRING_DATA, strlen((const char *) o->val.add),
				(const char *) o->val.add);
		break;
	case LIST:
		str = list_to_pro_string((struct list *) o->val.add);
		break;
	default:
		break;
	}
	return str;
}*/
