/*
 * dispatch.h
 *
 *  Created on: 2014年4月13日
 *      Author: lunatic
 */

#ifndef DISPATCH_H_
#define DISPATCH_H_



struct client_info {
	int fd;
	char ip[26];
	long long start_time;
};


struct packet {
	int fd;
	long long curtime;
	struct string *data;
};



int dispatch(int fd);

#endif /* DISPATCH_H_ */
