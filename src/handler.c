/*
 * handler.c
 *
 *  Created on: 2014年4月13日
 *      Author: lunatic
 */

#include <stdlib.h>

#include "dispatch.h"
#include "protocol.h"
#include "engine.h"
#include "log.h"

void handle(struct packet *req, struct packet *resp) {
	/*
	 *
	 *
	 *
	 *
	 */

	struct command *cmd;
	struct string ps = string_stack_escape(req->data);
	log_debug("待解析数据：%s", ps.value);

	if ((cmd = parse_to_command(req->data->value)) == NULL) {
		log_error("协议解析出错");
		resp->data = string_new("-ERROR 协议错误！");
		return;
	}

	log_debug("客户端数据校验通过！");
	log_debug("客户端数据解析成功！");

	int i;
	log_debug("argc: %d", cmd->argc);
	for (i = 0; i < cmd->argc; ++i) {
		log_debug("argc: %d\t argv: %s", i, cmd->argv[i]);
	}
	resp->data = execute(cmd);

	free(cmd->argv);
}
