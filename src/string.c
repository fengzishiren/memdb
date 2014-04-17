#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "string.h"

static inline struct string *string_init(const char *s, size_t len) {
	struct string *ss = string_expand(NULL, len + 1);
	ss->capacity = len + 1;
	ss->length = len;
	if (len)
		memcpy(ss->value, s, len);
	ss->value[len] = '\0';
	return ss;
}

struct string string_stack_new(const char *s) {
	struct string ss;
	int len = strlen(s);

	ss.capacity = STRING_STACK_SIZE;
	ss.hash = 0;
	ss.length = len < STRING_STACK_SIZE ? len : STRING_STACK_SIZE - 1;

	if (ss.length)
		memcpy(ss.value, s, ss.length);
	ss.value[ss.length] = '\0';
	return ss;
}

struct string *string_new(const char *s) {
	return string_init(s, strlen(s));
}

struct string *string_alloc(size_t nbytes) {
	struct string *ss = string_expand(NULL,
			nbytes ? nbytes : STRING_DEFAULT_CAPACITY);
	return string_clear(ss);
}

struct string *string_format(const char *msg, ...) {
	int ret = 0;
	struct string *ss = NULL;
	int length = 1024;

	va_list va;
	va_list args;
	va_start(va, msg);

	ss = string_expand(ss, length);
	va_copy(args, va);
	ret = vsnprintf(ss->value, length, msg, args);
	va_end(args);
	if (ret < 0) {
		free(ss);
		return NULL;
	} else if (ret >= length)
		for (;;) {
			ss = string_expand(ss, length);
			va_copy(args, va);
			ret = vsnprintf(ss->value, length, msg, args);
			va_end(args);
			if (ret < 0) {
				free(ss);
				return NULL;
			} else if (ret >= length) {
				length = ret + 1;
				continue;
			}
			break;
		}
	ss->length = ret;
	return ss;
}

struct string *string_dup(struct string *ss) {
	return string_init(ss->value, ss->length);
}


struct string *string_clear(struct string *ss) {
	memset(ss->value, 0, ss->capacity);
	ss->length = 0;
	ss->hash = 0;
	return ss;
}

int string_hashcode(struct string *ss) {
	int h = ss->hash;
	size_t len = ss->length;
	char *s = ss->value;

	if (h == 0 && len > 0) {

		while (*s) {
			h = 31 * h + *s++;
		}
		ss->hash = h;
	}
	return h;
}

static struct string *string_cat(struct string *ss, const char *src, size_t len) {
	int cap = ss->capacity;

	if (len == 0)
		return ss;

	while (cap - ss->length <= len) {
		cap = cap * 3 / 2;
	}
	if (cap > ss->capacity)
		ss = string_expand(ss, cap);

	/*	strncat(ss->value, src, len);*/
	memcpy(ss->value + ss->length, src, len);

	ss->length += len;
	ss->value[ss->length] = '\0';
	return ss;
}
/*

 ASCII码值（十进制）
 \a

 响铃(BEL)

 007
 \b

 退格(BS) ，将当前位置移到前一列

 008
 \f

 换页(FF)，将当前位置移到下页开头

 012
 \n

 换行(LF) ，将当前位置移到下一行开头

 010
 \r

 回车(CR) ，将当前位置移到本行开头

 013
 \t

 水平制表(HT) （跳到下一个TAB位置）

 009
 \v

 垂直制表(VT)

 011
 代表一个反斜线字符''\'

 092
 \'

 代表一个单引号（撇号）字符

 039
 \"

 代表一个双引号字符

 034
 \0

 空字符(NULL)

 000
 \ddd

 1到3位八进制数所代表的任意字符

 三位八进制
 \xhh

 1到2位十六进制所代表的任意字符

 二位十六进制
 */
/*
struct string *string_escape(struct string *ss) {
	struct string *nss = string_alloc(ss->length * 2 + 1);
	int i, j = 0;
	char ec;
	for (i = 0; i < ss->length; ++i, ++j) {
		ec = ss->value[i];
		switch (ec) {
		case '\r':
			nss->value[j++] = '\\';
			nss->value[j] = 'r';
			break;
		case '\n':
			nss->value[j++] = '\\';
			nss->value[j] = 'n';
			break;
		default:
			nss->value[j] = ec;
		}
	}
	nss->value[j] = '\0';
	return nss;


}*/
struct string string_stack_escape(struct string *ss) {
	struct string stack = string_stack_new("");
	int i, j = 0;
	char ec;
	for (i = 0; j < STRING_STACK_SIZE - 1 && i < ss->length; ++i, ++j) {
		ec = ss->value[i];
		switch (ec) {
		case '\r':
			stack.value[j++] = '\\';
			stack.value[j] = 'r';
			break;
		case '\n':
			stack.value[j++] = '\\';
			stack.value[j] = 'n';
			break;
		default:
			stack.value[j] = ec;
		}
	}
	stack.value[j] = '\0';
	stack.length = j;
	return stack;
}

struct string *string_concat(struct string *ss, const char *src) {
	if (src) {
		ss = string_cat(ss, src, strlen(src));
	}
	return ss;
}

struct string *string_append(struct string *ss, struct string *so) {
	if (so) {
		ss = string_cat(ss, so->value, so->length);
	}
	return ss;
}

