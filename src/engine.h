/*
 * engine.h
 *
 *  Created on: 2014年4月13日
 *      Author: Lunatic Poet
 */

#ifndef ENGINE_H_
#define ENGINE_H_
#include "protocol.h"
#include "string.h"

/*
 * 执行引擎函数
 * 根据输入参数命令名找到对应的的函数并通过执行函数访问内存数据
 * 返回执行结果 注意该结果的string需要显式释放
 */
struct string *execute(struct command *cmd);

#endif /* ENGINE_H_ */
