/*
 * memdb.c
 *
 *  Created on: 2014年4月10日
 *      Author: epay111
 */
#include <stdlib.h>
#include <stdint.h>

#include "memdb.h"
#include "hashtable.h"

struct memdb *memdb_new(size_t dbcnt) {
	struct memdb *db = malloc(sizeof(struct memdb));
	db->count = dbcnt == 0 ? DB_COUNT : dbcnt;
	db->idx = 0;

	db->dict = malloc(sizeof(struct hashtable *) * db->count);
	memset(db->dict, 0, sizeof(struct hashtable *) * db->count);

	db->dict[db->idx] = malloc(sizeof(struct hashtable));

	hashtable_init(db->dict[db->idx], NULL, NULL);

	return db;
}

/*
 *
 * 对于字符串类型 dict中存储k,V对应的地址
 *
 * 对于整形（包括int16, int32, int64：
 * 先放入对应的inset中 然后返回地址，再将地址作为V存储到dict
 *
 *
 *
 *
 */

void memdb_set(struct memdb *db, char *key, struct object *val) {
	hashtable_put(db->dict[db->idx], key, (void *) val);
}

struct object *memdb_get(struct memdb *db, char *key) {
	return hashtable_get(db->dict[db->idx], key);
}
int memdb_contain(struct memdb *db, char *key, struct object *val) {
	return NULL == hashtable_get(db->dict[db->idx], key) ? 0 : 1;

}
struct object *memdb_del(struct memdb *db, char *key) {
	return hashtable_remove(db->dict[db->idx], key);
}

void memdb_free(struct memdb *db) {
	int i;
	for (i = 0; i < db->idx; ++i) {
		if (db->dict[i])
			hashtable_free(db->dict[i]);
	}
	free(db->dict);
	free(db);
}

