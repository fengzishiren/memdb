/*
 * transport.h
 *
 *  Created on: 2014年4月11日
 *      Author: lunatic
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "memdb.h"
enum {
	OP_OK, OP_CMD_NOT_FOUND, OP_ARG_ERROR, OP_TYPE_ERROR
};


extern int err_stat;

char *detect_err();

void set_obj(char *key, void *val, enum object_type vt);
/*exist 1, Or 0 , -1 on error*/
int exist_obj(char *key, enum object_type vt);

void *get_obj(char *key, enum object_type vt);

int del_obj(char *key, enum object_type vt);

#endif /* TRANSPORT_H_ */
