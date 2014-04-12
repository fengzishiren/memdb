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

struct memdb memdb;

void memdb_init() {
	memdb.dict = malloc(sizeof(struct hashtable));
	hashtable_init(memdb.dict, NULL, NULL);
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

void memdb_set(char *key, struct object *val) {
	hashtable_put(memdb.dict, key, (void *) val);
}

struct object *memdb_get(char *key) {
	return hashtable_get(memdb.dict, key);
}
int memdb_contain(char *key, struct object *val) {
	return NULL == hashtable_get(memdb.dict, key) ? 0 : 1;

}
struct object *memdb_del(char *key) {
	return hashtable_remove(memdb.dict, key);
}

void memdb_free() {
	hashtable_free(memdb.dict);
	free(memdb.dict);
	free(memdb.intset);
}

