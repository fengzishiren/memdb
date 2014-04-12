/*
 * memdb.h
 *
 *  Created on: 2014年4月11日
 *      Author: lunatic
 */

#ifndef MEMDB_H_
#define MEMDB_H_

#define INSET_DEAFULT_SIZE 1024
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
	enum object_type it;
	union {
		void *add;
		int_t num;
	} val;
};
struct memdb {
	struct intset *intset;
	struct hashtable *dict;
};

struct object *memdb_get(char *key);

void memdb_set(char *key, struct object *val);

int memdb_contain(char *key, struct object *val);

struct object *memdb_del(char *key);

void memdb_free();
#endif /* MEMDB_H_ */
