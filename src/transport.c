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
static inline struct object *pack_object(void *val, enum object_type ot) {
	struct object *v = malloc(sizeof(struct object));
	v->ot = ot;
	switch (ot) {
	case INT:
		int_t n = *((int_t *) val);
		v->val.num = n;
		break;
	case STRING:
	case LIST:
	case SET:
	case SSET:
		v->val.add = val;
	}
	return v;
}
static inline void *unpack_object(struct object *v) {
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

/*OP_OK, OP_CMD_NOT_FOUND, OP_ARG_ERROR, OP_TYPE_ERROR*/
static char *errset[] = { "", "", "", "" };

char *detect_err() {
	if (err_stat == OP_OK)
		return NULL;
	else {
		int i = err_stat;
		err_stat = OP_OK;
		return errset[i];
	}
}

void set_obj(char *key, void *val, enum object_type ot) {
	struct object *v = pack_object(val, ot);
	memdb_set(key, v);
}
/*exist 1, Or 0 , -1 on error*/
int exist_obj(char *key, enum object_type vt) {
	struct object *v = memdb_get(key);
	if (v == NULL)
		return 0;
	else if (v->ot != vt) {
		err_stat = OP_TYPE_ERROR;
		return -1;
	} else
		return 1;
}

void *get_obj(char *key, enum object_type vt) {
	struct object *v = memdb_get(key);
	if (v == NULL)
		return v;
	else if (v->ot != vt) {
		err_stat = OP_TYPE_ERROR;
		return NULL;
	} else
		return v->val.add;
}

int del_obj(char *key, enum object_type vt) {
	struct object *v = memdb_del(key);
	if (v == NULL)
		return 0;
	else if (v->ot != vt) {
		err_stat = OP_TYPE_ERROR;
		return -1;
	} else {
		del_by_type(unpack_object(v), vt);
		return 1;
	}
}

