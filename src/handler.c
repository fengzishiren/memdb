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

	struct pro_args *args;
	if ((args = parse_pro(req->data->value)) == NULL) {
		resp->data = string_new("-ERROR 协议错误！");
		return;
	}

	int i;
	log_debug("argc:%d", args->argc);
	for (i = 0; i < args->argc; ++i) {
		log_debug("argc: %d\t argv: %s\n", i, args->argv[i]);
	}
	resp->data = execute(args->argc, args->argv);

	free(args);
}
