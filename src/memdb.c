/*
 * memdb.c
 *
 *  Created on: 2014年4月10日
 *      Author: epay111
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "memdb.h"
#include "dict.h"

struct memdb *memdb_new(size_t dbcnt) {
	struct memdb *db = malloc(sizeof(struct memdb));
	db->count = dbcnt == 0 ? DB_COUNT : dbcnt;
	db->idx = 0;

	db->dict = malloc(sizeof(struct dict *) * db->count);
	memset(db->dict, 0, sizeof(struct dict *) * db->count);

	db->dict[db->idx] = malloc(sizeof(struct dict));

	dict_init(db->dict[db->idx], NULL, NULL);

	return db;
}

/*
 *
 *  dict中存储k,V对应的地址
 *
 *
 *
 *
 */

void *memdb_set(struct memdb *db, char *key, struct object *val) {
	return dict_put(db->dict[db->idx], key, (void *) val);
}

struct object *memdb_get(struct memdb *db, char *key) {
	return dict_get(db->dict[db->idx], key);
}
int memdb_contain(struct memdb *db, char *key, struct object *val) {
	return NULL == dict_get(db->dict[db->idx], key) ? 0 : 1;

}
struct object *memdb_del(struct memdb *db, char *key) {
	return dict_remove(db->dict[db->idx], key);
}

void memdb_free(struct memdb *db) {
	int i;
	for (i = 0; i < db->idx; ++i) {
		if (db->dict[i])
			dict_free(db->dict[i]);
	}
	free(db->dict);
	free(db);
}

