/*
 * hashtable.c
 * 动态散列
 *
 *  Created on: 2013年10月7日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define DEFAULT_CAPACITY 	16
#define LOAD_FACTOR 		0.75

static int str_hash(void *key) {
	int result = 0;
	const char *k = (const char *) key;
	while (*k) {
		result = result * 31 + *k++;
	}
	return result;
}
static inline size_t index_for(size_t hash, size_t length) {
	return hash & (length - 1);
}

struct hashtable *hashtable_init(struct hashtable *hashtable, hash hash_fuc,
		cmp cmp_fuc) {
	hashtable->capacity = DEFAULT_CAPACITY;
	hashtable->size = 0;
	hashtable->hash_fuc = hash_fuc == NULL ? str_hash : hash_fuc;
	hashtable->cmp_fuc = cmp_fuc == NULL ? (cmp *) strcmp : cmp_fuc;
	hashtable->table = (struct pair **) malloc(
			sizeof(struct pair *) * hashtable->capacity);
	memset(hashtable->table, 0, sizeof(struct pair *) * hashtable->capacity);
	return hashtable;
}

static void transfer(struct hashtable *hashtable, size_t new_capacity) {
	struct pair **new_table = (struct pair **) malloc(
			sizeof(struct pair *) * new_capacity);
	memset(new_table, 0, sizeof(struct pair *) * new_capacity);

	struct pair **old_table = hashtable->table;
	int i, old_capacity = hashtable->capacity;
	for (i = 0; i < old_capacity; ++i) {
		struct pair *p = old_table[i];
		while (p != NULL) {
			struct pair *next = p->next;
			int index = index_for(p->hashcode, new_capacity);
			p->next = new_table[index];
			new_table[index] = p;
			p = next;
		}
	}
	free(old_table);
	hashtable->table = new_table;
	hashtable->capacity = new_capacity;
}

static inline void add_pair(struct hashtable *hashtable, int hashcode,
		void *key, void *val, size_t pos) {
	struct pair *p = (struct pair *) malloc(sizeof(struct pair));
	p->key = key;
	p->val = val;
	p->hashcode = hashcode;
	p->next = hashtable->table[pos];
	hashtable->table[pos] = p;

	int threshold = (int) (hashtable->capacity * LOAD_FACTOR);

	if (hashtable->size++ >= threshold) {
		transfer(hashtable, hashtable->capacity << 1);
	}
}

void hashtable_put(struct hashtable *hashtable, void *key, void *val) {

	int hashcode = hashtable->hash_fuc(key);
	size_t pos = index_for(hashcode, hashtable->capacity);

	struct pair *p;
	for (p = hashtable->table[pos]; p != NULL; p = p->next) {
		if (p->hashcode == hashcode && hashtable->cmp_fuc(p->key, key) == 0) {
			p->val = val;
			return;
		}
	}
	add_pair(hashtable, hashcode, key, val, pos);
}

void *hashtable_get(struct hashtable *hashtable, void *key) {
	if (hashtable->size == 0)
		return NULL;
	int hashcode = hashtable->hash_fuc(key);
	size_t pos = index_for(hashcode, hashtable->capacity);

	struct pair *p;
	for (p = hashtable->table[pos]; p != NULL; p = p->next) {
		if (p->hashcode == hashcode && hashtable->cmp_fuc(p->key, key) == 0) {
			return p->val;
		}
	}
	return NULL;
}

void *hashtable_remove(struct hashtable *hashtable, void *key) {
	void *rm = NULL;
	int hashcode = key == NULL ? 0 : hashtable->hash_fuc(key);
	size_t pos = index_for(hashcode, hashtable->capacity);

	struct pair *p, *prev = NULL;
	for (p = hashtable->table[pos]; p != NULL; prev = p, p = p->next) {
		if (p->hashcode == hashcode && hashtable->cmp_fuc(p->key, key) == 0) {
			rm = p->val;
			if (prev == NULL)
				hashtable->table[pos] = p->next;
			else
				prev->next = p->next;
			free(p);
			--hashtable->size;
			break;
		}
	}
	return rm;
}

void hashtable_each(struct hashtable *hashtable,
		void (*handle_kv)(void *key, void *val)) {
	struct pair **old_table = hashtable->table;
	int i, old_capacity = hashtable->capacity;

	for (i = 0; i < old_capacity; ++i) {
		struct pair *p = old_table[i];
		while (p != NULL) {
			struct pair *next = p->next;
			handle_kv(p->key, p->val);
			p = next;
		}
	}
}

struct hashtable *hashtable_clear(struct hashtable *hashtable) {
	int i;
	for (i = 0; i < hashtable->capacity; ++i) {
		struct pair *p, *del;
		for (p = hashtable->table[i]; p != NULL;) {
			del = p;
			p = p->next;
			free(del);
		}
		hashtable->table[i] = NULL;
	}
	hashtable->size = 0;
	return hashtable;
}

void hashtable_free(struct hashtable *hashtable) {
	hashtable_clear(hashtable);
	free(hashtable->table);
}
