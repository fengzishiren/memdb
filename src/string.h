#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>

/*use func's declaration avoid include '<string.h>'*/
extern int strcmp(const char *__s1, const char *__s2);

#define STRING_STACK_SIZE 1024
#define STRING_DEFAULT_CAPACITY 1024

#define string_size (sizeof(struct string) - sizeof(char) * STRING_STACK_SIZE)
#define byte_size(n, type) (n * sizeof(type))

struct string {
	size_t capacity;
	size_t length;
	int hash;
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
	ss = realloc(ss, string_size + byte_size(new_cap, char));
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
