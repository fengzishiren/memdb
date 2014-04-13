/*
 * string.h
 *
 *  Created on: 2014年4月10日
 *      Author: Lunatic Poet
 */
#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>

/*use func's declaration avoid include '<string.h>'*/
extern int strcmp(const char *__s1, const char *__s2);

#define STRING_STACK_SIZE 1024
#define STRING_DEFAULT_CAPACITY 1024

#define string_size (sizeof(struct string) - sizeof(char) * STRING_STACK_SIZE)
#define byte_size(n, type) (n * sizeof(type))
/*
 * 相对于c_str
 * 这里将可用的字符空间长度capacity 以及实际的已使用空间length独立出来构成string结构体
 *
 * 采用C99 的flexible array member 参考了redis的sds
 *
 *
 * 注意：
 *  虽然length记录了字符串的实际长度
 *  但为了方便与c_str转换 string结构体的value字段总是以'\0'结尾
 *  length的长度不包括结尾的'\0'
 *  hash成员主要为了进行快速匹配 作用不太重要
 *  因此这里采用惰性hash技术 只有到使用时发现hash未设置 才去计算hash值
 *
 *  这里惰性hash的设计参考了java语言jdk java.lang.String的设计
 *
 * 另外要注意：
 * 		STRING_STACK_SIZE长度只有在stack上非配字段时生效 即使用sting_stack()方法时
 *  其他创建string时均令STRING_STACK_SIZE = 0
 *
 *  这里在堆栈上创建string的设计参考了云风大侠的string实现
 *
 *
 */
struct string {
	size_t capacity;
	size_t length;
	int hash;
	/* STRING_STACK_SIZE长度只有在stack上非配字段时生效
	 * 即使用sting_stack()方法时
	 * 其他情形均被是为0
	 */
	char value[STRING_STACK_SIZE];
};

struct string string_stack(const char *s);

struct string *string_new(const char *s);

struct string *string_alloc(size_t nbytes);

struct string *string_format(const char *msg, ...);

struct string *string_concat(struct string *ss, const char *src);

struct string *string_append(struct string *ss, struct string *so);

struct string *string_dup(struct string *ss);

static inline struct string *string_expand(struct string *ss, size_t new_cap) {
	ss = realloc(ss, (string_size + byte_size(new_cap, char)));
	ss->capacity = new_cap;
	return ss;
}

static inline int string_empty(struct string *ss) {
	return ss->length == 0;
}

static inline char *string_cstr(struct string *ss) {
	return ss->value;
}

static inline struct string *string_clear(struct string *ss) {
	ss->value[0] = '\0';
	ss->length = 0;
	ss->hash = 0;
	return ss;
}

int string_hashcode(struct string *ss);

static inline int string_equals(struct string *ss, struct string *so) {
	return string_hashcode(ss) == string_hashcode(so)
			&& strcmp(ss->value, so->value) == 0;
}

static inline void string_free(struct string *ss) {
	free(ss);
}

#endif
