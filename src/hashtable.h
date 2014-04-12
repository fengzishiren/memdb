/*
 * hashtable.h
 *
 * 对于hashtable的实现 链接法
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
	void *key;
	void *val;
	struct pair *next;
};

typedef int hash(void *key);
typedef int cmp(const void *, const void *);

struct hashtable {
	size_t size;
	size_t capacity;
	hash *hash_fuc;
	cmp *cmp_fuc;
	struct pair **table;
};

extern struct hashtable *hashtable_init(struct hashtable *hashtable,
		hash hash_fuc, cmp cmp_fuc);

extern void hashtable_put(struct hashtable *hashtable, void *key, void *val);

extern void *hashtable_get(struct hashtable *hashtable, void *key);

extern void *hashtable_remove(struct hashtable *hashtable, void *key);

extern struct hashtable *hashtable_clear(struct hashtable *hashtable);

extern void hashtable_free(struct hashtable *hashtable);

#endif /* MAP_H_ */
