/*
 * engine.h
 *
 *  Created on: 2014年4月13日
 *      Author: Lunatic Poet
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include "string.h"

typedef void *cmd_func(int argc, char **argv);

struct string *execute(int argc, char **argv);

#endif /* ENGINE_H_ */
