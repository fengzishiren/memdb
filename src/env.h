/*
 * env.h
 *
 *  Created on: 2014年4月14日
 *      Author: Lunatic Poet
 */

#ifndef ENV_H_
#define ENV_H_
#include "log.h"

struct global_environ {
	int log_closed;
	enum log_level lv;
	/*待填充*/
	size_t dbcnt;
	struct memdb *db;
	char *version;
	char *name;
};

extern struct global_environ env;

#endif /* ENV_H_ */
