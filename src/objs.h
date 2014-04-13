/*
 * objs.h
 *
 * object构造以及一些辅助性对象构造工具
 *
 *
 *  Created on: 2014年4月12日
 *      Author: Lunatic Poet
 */

#ifndef OBJCOL_H_
#define OBJS_H_

struct object *create_object(void *val, enum object_type ot);
char *str_to_str(char *str);

struct list *strs_to_list(struct list *ls,  int n, char **strs);

void delete_object(struct object *o);

#endif /* OBJCOL_H_ */
