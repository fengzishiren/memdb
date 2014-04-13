/*
 * handler.h
 * 对数据包进行处理
 * 调用执行引擎访问内存数据
 *  Created on: 2014年4月13日
 *      Author: lunatic
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "dispatch.h"

void handle(struct packet *req, struct packet *resp);

#endif /* HANDLER_H_ */
