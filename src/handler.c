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
	if ((cmd = parse_to_command(req->data->value)) == NULL) {
		resp->data = string_new("-ERROR 协议错误！");
		return;
	}

	int i;
	log_debug("argc:%d", cmd->argc);
	for (i = 0; i < cmd->argc; ++i) {
		log_debug("argc: %d\t argv: %s\n", i, cmd->argv[i]);
	}
	resp->data = execute(cmd);

	free(cmd);
}
