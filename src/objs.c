/*
 * objcol.c
 *
 *  Created on: 2014年4月12日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "string.h"
#include "list.h"
#include "memdb.h"
#include "protocol.h"

/*
 *
 *
 *
 * 传入数据的地址 生成对应的object
 *
 * 注意：仅INT类型存储数据副本 其他类型均为地址
 *
 *
 */
struct object *create_object(void *val, enum object_type ot) {
	struct object *v = malloc(sizeof(struct object));
	v->ot = ot;
	switch (ot) {
	case INT:
		/*int_t n = *((int_t *) val);*/
		v->val.num = *((int_t *) val);
		break;
	case STRING:
	case LIST:
	case SET:
	case SSET:
		v->val.add = val;
	}
	return v;
}

void delete_object(struct object *o) {
	switch (o->ot) {
	case INT:
		break;
	case STRING:
		free(o->val.add);
		break;
	case LIST:
		list_free((struct list *) o->val.add, 1);
		break;
	case SET:
		break;
	case SSET:
		break;
	}
	free(o);
}

char *str_clone(char *str) {
	size_t len = strlen(str);
	char *des = malloc(len + 1);
	memcpy(des, str, len);
	des[len] = '\0';
	return des;
}
/*
 *
 * _ls爲NULL時 创建新链表并加入元素
 *
 */
struct list *strs_fill_list(struct list *_ls, int n, char **strs) {

	struct list *ls = _ls == NULL ? list_new() : _ls;
	int i;
	for (i = 0; i < n; ++i) {
		list_add(ls, str_clone(strs[i]));
	}
	return ls;
}
