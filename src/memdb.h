/*
 * memdb.h
 *
 *  Created on: 2014年4月11日
 *      Author: lunatic
 */

#ifndef MEMDB_H_
#define MEMDB_H_

#define INSET_DEAFULT_SIZE 1024

#define DB_COUNT 16

typedef char byte;
typedef int64_t int_t;

struct intset {
	size_t capacity;
	size_t size;
	byte bytes[];
};

enum object_type {
	INT, STRING, LIST, SET, SSET,
};

struct object {
	enum object_type ot;
	union {
		void *add;
		int_t num;
	} val;
};
struct memdb {
	size_t count;
	size_t idx;
	struct hashtable **dict;
};

struct memdb *memdb_new(size_t dbcnt);

void memdb_set(struct memdb *db, char *key, struct object *val);

struct object *memdb_get(struct memdb *db, char *key);

int memdb_contain(struct memdb *db, char *key, struct object *val);

struct object *memdb_del(struct memdb *db, char *key);

void memdb_free(struct memdb *db);
#endif /* MEMDB_H_ */
