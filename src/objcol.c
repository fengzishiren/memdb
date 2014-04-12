/*
 * objcol.c
 *
 *  Created on: 2014年4月12日
 *      Author: epay111
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "string.h"
#include "list.h"
#include "memdb.h"
#include "protocol.h"

static char *int2str(long n, char *dest) {
	char temp[20]; /*long的最大长度刚好20位*/
	int m = 0, i = 0, j = 0, len = 0;

	len = sizeof(temp) / sizeof(char);

	if (n < 0) {
		n = -n;
		dest[j++] = '-';
	}
	for (i = len - 1; n != 0; i--) {
		m = n % 10;
		n /= 10;
		temp[i] = m + '0';
	}
	for (i++; i < len; j++, i++) {
		dest[j] = temp[i];
	}
	dest[j] = '\0';
	return dest;
}
/*
 *
 *
 *
 * 传入数据的地址 生成对应的object
 *
 * 注意：仅INT类型存储数据副本 其他类型均为地址
 *
 *
 */
struct object *str2object(void *val, enum object_type ot) {
	struct object *v = malloc(sizeof(struct object));
	v->ot = ot;
	switch (ot) {
	case INT:
		int_t n = *((int_t *) val);
		v->val.num = n;
		break;
	case STRING:
	case LIST:
	case SET:
	case SSET:
		v->val.add = val;
	}
	return v;
}

struct list *strs2list(char **strs, int n) {
	struct list *ls = list_new();
	int i;
	for (i = 0; i < n; ++i) {
		list_add(ls, strs[i]);
	}
	return ls;
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
struct string *list2prostr(struct list *ls) {
	char buf[20];

	struct string *s = string_new(PRO_MULTI);
	s = string_concat(s, int2str(ls->size, buf));
	s = string_concat(s, PRO_NEW_LINE);

	struct list_iter it;
	list_iter(ls, &it);
	struct list_entry *e;
	while ((e = list_next(&it)) != NULL) {
		char *item = (char *) e->data;
		s = string_concat(s, PRO_BULK);
		s = string_concat(s, int2str(strlen(item), buf));
		s = string_concat(s, PRO_NEW_LINE);
		s = string_concat(s, item);
		s = string_concat(s, PRO_NEW_LINE);
	}
	return s;
}
/*
 struct string *string_format(const char *msg, ...) {
 struct string *ss = NULL;
 int length = 1024;
 va_list va;
 va_list args;
 for (;;) {
 ss = string_expand(ss, length);
 va_copy(args, va);
 int ret = vsnprintf(ss->value, length, msg, args);
 if (ret < 0 || ret >= length) {
 length <<= 1;
 va_end(args);
 continue;
 }
 va_end(args);
 break;
 }
 return ss;
 }*/
/*
 struct string *int2prostr(long long int n) {

 * 整数回复就是一个以 ":" 开头， CRLF 结尾的字符串表示的整数。
 * 比如说， ":0\r\n" 和 ":1000\r\n" 都是整数回复

 char buf[20];
 int2str(n, buf);
 struct string *str = string_new(PRO_INTEGER);
 str = string_concat(str, PRO_NEW_LINE);
 str = string_concat(str, buf);
 str = string_concat(str, PRO_NEW_LINE);
 return str;
 }*/

struct string *obj2prostr(struct object *o) {

	struct string *str = NULL;
	switch (o->ot) {
	case INT:
		str = string_format(PRO_INTEGER_DATA, (long long int) o->val.num);
		break;
	case STRING:
		/*$6\r\nfoobar\r\n"*/
		char *s = (const char *) o->val.add;
		str = string_format(PRO_STRING_DATA, (int) strlen(s), s);
		break;
	case LIST:
		str = list2prostr((struct list *) o->val.add);
		break;
	default:
		break;
	}
	return str;
}

void del_obj(struct object *o) {
	switch (o->ot) {
	case INT:

		break;
	case STRING:
		free(o->val.add);
		break;
	case LIST:
		list_free((struct list *) o->val.add, 1);
	}
	free(o);
}
