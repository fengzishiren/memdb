/*
 * transport.c
 *
 *  Created on: 2014年4月11日
 *      Author: lunatic
 */

#include <stdlib.h>
#include <string.h>

#include "transport.h"
#include "memdb.h"
/**
 *
 *
 * INT, STRING, LIST, SET, SSET,
 *
 *
 */

int err_stat = 0; /*ok*/
/*
 * get set add append
 *
 *
 */

static inline struct object *pack_value(struct object val) {
	struct object *v = malloc(sizeof(struct object));
	memcpy(v, &val, sizeof(struct object));
	return v;
}
static inline void *unpack_value(struct object *v) {
	void *add = v->val.add;
	free(v);
	return add;
}

static void del_int(void *addr) {

}

static void del_string(void *addr) {
	free(addr);
}

static void del_list(void *addr) {

}
static void del_set(void *addr) {

}

static void del_sset(void *addr) {

}

static void del_by_type(void *addr, enum object_type vt) {
	switch (vt) {
	case INT:
		del_int(addr);
		break;
	case STRING:
		del_string(addr);
		break;
	case LIST:
		del_list(addr);
		break;
	case SET:
		del_set(addr);
		break;
	case SSET:
		del_sset(addr);
		break;
	}
}

void set_obj(char *key, struct object val) {
	struct object *v = pack_value(val);
	memdb_set(key, v);
}
/*exist 1, Or 0 , -1 on error*/
int exist_obj(char *key, enum object_type vt) {
	struct object *v = memdb_get(key);
	if (v == NULL)
		return 0;
	else if (v->it != vt) {
		err_stat = OP_TYPE_ERROR;
		return -1;
	} else
		return 1;
}

void *get_obj(char *key, enum object_type vt) {
	struct object *v = memdb_get(key);
	if (v == NULL)
		return v;
	else if (v->it != vt) {
		err_stat = OP_TYPE_ERROR;
		return NULL;
	} else
		return v->val.add;
}

int del_obj(char *key, enum object_type vt) {
	struct object *v = memdb_del(key);
	if (v == NULL)
		return 0;
	else if (v->it != vt) {
		err_stat = OP_TYPE_ERROR;
		return -1;
	} else {
		del_by_type(unpack_value(v), vt);
		return 1;
	}
}

