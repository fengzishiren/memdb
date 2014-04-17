/*
 * dispatch.c
 *
 *  Created on: 2014年4月10日
 *      Author: epay111
 */
#include <stdlib.h>

#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#include "string.h"
#include "handler.h"
#include "dispatch.h"
#include "log.h"

static long long get_curtime() {
	long long tm;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tm = tv.tv_sec;
	tm = tm * 1000000 + tv.tv_usec;
	return tm;
}

static struct packet pack_in = { .data = NULL }, pack_out = { .data = NULL };

static struct packet *recv_packet(struct packet *pack) {
	int len = 0, total = 0;
	char *data;
	int fd = pack->fd;
	data = pack->data->value;

	while ((len = read(fd, data, 1024)) > 0) {
		total += len;
		if (len == 1024) {
			pack->data = string_expand(pack->data, pack->data->capacity + 1024);
			data = pack->data->value + total;
		}
		/*else
		 data += len;*/
	}
	if (len == -1 && total == 0) {
		log_error("send_packet: write error! %s", strerror(errno));
		return NULL;
	} else if (len == 0 && total == 0) {
		log_info("客户端断开连接! %s", strerror(errno));
		close(fd);
		return NULL;
	}
	pack->data->value[total] = '\0';
	pack->data->length = total;

	struct string ps = string_stack_escape(pack->data);
	log_debug("接收到的数据：%s", ps.value);

	return pack;
}

static void send_packet(struct packet *pack) {
	int len = 0, fd = pack->fd;
	char *data = pack->data->value;

	struct string ps = string_stack_escape(pack->data);
	log_debug("即将返回数据：%s", ps.value);

	size_t size = pack->data->length;
	if ((len = write(fd, data, size)) != size) {
		log_error("send_packet: write error!");
	}
}

static struct packet *get_recv_pack(int fd) {
	pack_in.fd = fd;
	pack_in.curtime = get_curtime();
	if (!pack_in.data)
		pack_in.data = string_alloc(1024);
	return recv_packet(&pack_in);
}

static inline struct packet *get_send_pack(int fd) {
	pack_out.fd = fd;
	pack_out.curtime = get_curtime();
	return &pack_out;
}

static inline void recycle_packet() {
	string_free(pack_out.data);
}

static inline void destory_packet() {
	string_free(pack_in.data);
	string_free(pack_out.data);
}

int dispatch(int fd) {

	log_debug("client fd:%d", fd);
	if (fd == -1) {
		destory_packet();
		return -1;
	}

	struct packet *pack_recv = get_recv_pack(fd);
	if (pack_recv == NULL)
		return -1;

	struct packet *pack_send = get_send_pack(fd);

	handle(pack_recv, pack_send);

	send_packet(pack_send);

	recycle_packet();
	return fd;
}
