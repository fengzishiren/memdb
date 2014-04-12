/*
 * objcol.h
 *
 *  Created on: 2014年4月12日
 *      Author: epay111
 */

#ifndef OBJCOL_H_
#define OBJCOL_H_

struct object *str2object(void *val, enum object_type ot);

struct list *strs2list(char **strs, int n);

struct string *list2prostr(struct list *ls);

struct string *obj2prostr(struct object *o);

void del_obj(struct object *o);

#endif /* OBJCOL_H_ */
