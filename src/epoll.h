/*
 * epoll.h
 *
 *  Created on: 2014年4月13日
 *      Author: Lunatic Poet
 */

#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include <netinet/in.h>

#define MAX_EVENTS 10
#define PORT 8080

struct epoll {
	struct epoll_event ev, events[MAX_EVENTS];
	struct sockaddr_in local, remote;
	int listenfd;
	int epfd;
};

struct epoll *open_epoll(int port);
int start_epoll(struct epoll *epoll);
void close_epoll(struct epoll *epoll);

#endif /* EPOLL_H_ */
