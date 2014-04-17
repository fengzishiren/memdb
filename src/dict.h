/*
 * dict.h
 *
 * 对于dict的实现 链接法
 *
 *  Created on: 2013年10月7日
 *      Author: Lunatic Zheng
 */

#ifndef MAP_H_
#define MAP_H_

#define MAX_LEN 100
#include <stddef.h>

struct pair {
	int hashcode;
	char *key;
	void *val;
	struct pair *next;
};

typedef int hash(char *key);
typedef int cmp(const void *, const void *);

struct dict {
	size_t size;
	size_t capacity;
	hash *hash_fuc;
	cmp *cmp_fuc;
	struct pair **table;
};

extern struct dict *dict_init(struct dict *dict,
		hash hash_fuc, cmp cmp_fuc);

extern void *dict_put(struct dict *dict, char *key, void *val);

extern void *dict_get(struct dict *dict, char *key);

extern void *dict_remove(struct dict *dict, char *key);

extern struct dict *dict_clear(struct dict *dict);

extern void dict_free(struct dict *dict);

#endif /* MAP_H_ */
