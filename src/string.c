#include <string.h>

#include "string.h"

static inline struct string *string_init(const char *s, size_t len) {
	struct string *ss = string_expand(NULL, len + 1);
	ss->capacity = len + 1;
	ss->length = len;
	ss->hash = 0;

	if (len)
		memcpy(ss->value, s, len);
	ss->value[len] = '\0';
	return ss;
}

struct string string_stack(const char *s) {
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

	ss->length = 0;
	ss->value[0] = '\0';
	ss->hash = 0;
	return ss;
}

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
}

struct string *string_dup(struct string *ss) {
	return string_init(ss->value, ss->length);
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
	ss->hash = 0;
	return ss;
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

