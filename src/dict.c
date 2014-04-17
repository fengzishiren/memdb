/*
 * dict.c
 * 动态散列
 *
 *  Created on: 2013年10月7日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "memdb.h"
#include "dict.h"
#include "log.h"

#define DEFAULT_CAPACITY 	16
#define LOAD_FACTOR 		0.75

static int str_hash(char *key) {
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

struct dict *dict_init(struct dict *dict, hash hash_fuc,
		cmp cmp_fuc) {
	dict->capacity = DEFAULT_CAPACITY;
	dict->size = 0;
	dict->hash_fuc = hash_fuc == NULL ? str_hash : hash_fuc;
	dict->cmp_fuc = cmp_fuc == NULL ? (cmp *) strcmp : cmp_fuc;
	dict->table = (struct pair **) malloc(
			sizeof(struct pair *) * dict->capacity);
	memset(dict->table, 0, sizeof(struct pair *) * dict->capacity);
	return dict;
}

static void transfer(struct dict *dict, size_t new_capacity) {
	struct pair **new_table = (struct pair **) malloc(
			sizeof(struct pair *) * new_capacity);
	memset(new_table, 0, sizeof(struct pair *) * new_capacity);

	struct pair **old_table = dict->table;
	int i, old_capacity = dict->capacity;
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
	dict->table = new_table;
	dict->capacity = new_capacity;
}

static inline void add_pair(struct dict *dict, int hashcode,
		char *key, void *val, size_t pos) {
	struct pair *p = (struct pair *) malloc(sizeof(struct pair));
	p->key = key;
	p->val = val;
	p->hashcode = hashcode;
	p->next = dict->table[pos];
	dict->table[pos] = p;

	int threshold = (int) (dict->capacity * LOAD_FACTOR);

	if (dict->size++ >= threshold) {
		transfer(dict, dict->capacity << 1);
	}
}

void dict_each(struct dict *dict,
		void (*handle_kv)(char *key, void *val)) {
	struct pair **old_table = dict->table;
	int i, old_capacity = dict->capacity;

	for (i = 0; i < old_capacity; ++i) {
		struct pair *p = old_table[i];
		while (p != NULL) {
			struct pair *next = p->next;
			handle_kv(p->key, p->val);
			p = next;
		}
	}
}

void *dict_put(struct dict *dict, char *key, void *val) {

	void *ret = NULL;
	int hashcode = dict->hash_fuc(key);
	size_t pos = index_for(hashcode, dict->capacity);

	log_debug("dict_put idx: %zu key: %s", pos, key);

	struct pair *p;
	for (p = dict->table[pos]; p != NULL; p = p->next) {
		if (p->hashcode == hashcode && dict->cmp_fuc(p->key, key) == 0) {
			ret = p->val;
			p->val = val;
			return ret;
		}
	}
	add_pair(dict, hashcode, key, val, pos);
	return ret;

}

void *dict_get(struct dict *dict, char *key) {
	if (dict->size == 0)
		return NULL;
	int hashcode = dict->hash_fuc(key);
	size_t pos = index_for(hashcode, dict->capacity);

	log_debug("dict_get idx: %zu key: %s", pos, key);

	struct pair *p;
	for (p = dict->table[pos]; p != NULL; p = p->next) {
		log_debug("dict_get p->key: %s", p->key);
		if (p->hashcode == hashcode && dict->cmp_fuc(p->key, key) == 0) {
			return p->val;
		}
	}
	return NULL;
}

void *dict_remove(struct dict *dict, char *key) {
	void *rm = NULL;
	int hashcode = key == NULL ? 0 : dict->hash_fuc(key);
	size_t pos = index_for(hashcode, dict->capacity);

	struct pair *p, *prev = NULL;
	for (p = dict->table[pos]; p != NULL; prev = p, p = p->next) {
		if (p->hashcode == hashcode && dict->cmp_fuc(p->key, key) == 0) {
			rm = p->val;
			if (prev == NULL)
				dict->table[pos] = p->next;
			else
				prev->next = p->next;
			free(p);
			--dict->size;
			break;
		}
	}
	return rm;
}

struct dict *dict_clear(struct dict *dict) {
	int i;
	for (i = 0; i < dict->capacity; ++i) {
		struct pair *p, *del;
		for (p = dict->table[i]; p != NULL;) {
			del = p;
			p = p->next;
			free(del);
		}
		dict->table[i] = NULL;
	}
	dict->size = 0;
	return dict;
}

void dict_free(struct dict *dict) {
	dict_clear(dict);
	free(dict->table);
}
