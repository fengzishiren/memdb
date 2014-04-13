#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <strings.h>
#include <netinet/tcp.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>

#include "epoll.h"
#include "dispatch.h"
#include "log.h"


static inline void epoll_panic(const char *err) {
/*	perror(err);
	exit(1)*/;
	panic("%s:\n\t%s", err, strerror(errno));
}

char *get_client_ip(int fd) {
	socklen_t rsa_len = sizeof(struct sockaddr_in);
	struct sockaddr_in rsa;
	char *ip = NULL;
	int port = 0;
	if (getpeername(fd, (struct sockaddr *) &rsa, &rsa_len) == 0) {
		ip = inet_ntoa(rsa.sin_addr);
		port = ntohs(rsa.sin_port);
	}
	(void)port;
	return ip;
}

/*设置socket非阻塞模式*/

static inline void setnonblocking(int sfd) {
	int ops = fcntl(sfd, F_GETFL);

	if (ops != -1) {
		ops |= O_NONBLOCK;
		ops = fcntl(sfd, F_SETFL, ops);
	}
	if (ops == -1) {
		epoll_panic("setnonblocking error!");
	}
}

struct epoll *epoll_open(int port) {
	struct epoll* epoll = malloc(sizeof(struct epoll));

	if ((epoll->listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		epoll_panic("socket error!");
	}
	setnonblocking(epoll->listenfd);
	bzero(&epoll->local, sizeof(epoll->local));
	epoll->local.sin_family = AF_INET;
	epoll->local.sin_addr.s_addr = htonl(INADDR_ANY);
	epoll->local.sin_port = htons(port);

	if (bind(epoll->listenfd, (struct sockaddr *) &epoll->local,
			sizeof(epoll->local)) < 0) {
		epoll_panic("bind error!");
	}
	listen(epoll->listenfd, 20);

	if ((epoll->epfd = epoll_create(10)) == -1) {
		epoll_panic("epoll_create error!");
	}

	epoll->ev.events = EPOLLIN;
	epoll->ev.data.fd = epoll->listenfd;

	if (epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, epoll->listenfd, &epoll->ev)
			== -1) {
		epoll_panic("epoll_ctl error!");
	}
	log_debug("打开epoll，监听端口:%d", port);

	return epoll;

}
int epoll_loop(struct epoll *epoll) {
	int addrlen, conn_sock, nfds, fd, i;

	log_info("开始接收客户端请求。。。");

	for (;;) {
		nfds = epoll_wait(epoll->epfd, epoll->events, MAX_EVENTS, -1);
		if (nfds == -1) {
			epoll_panic("epoll_wait error!");
		}

		for (i = 0; i < nfds; ++i) {
			fd = epoll->events[i].data.fd;
			if (fd == epoll->listenfd) {
				while ((conn_sock = accept(epoll->listenfd,
						(struct sockaddr *) &epoll->remote,
						(socklen_t *) &addrlen)) > 0) {
					setnonblocking(conn_sock);
					epoll->ev.events = EPOLLIN | EPOLLET;
					epoll->ev.data.fd = conn_sock;
					if (epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, conn_sock,
							&epoll->ev) == -1) {
						epoll_panic("epoll_ctl: add");
					}
				}
				if (conn_sock == -1) {
					if (errno != EAGAIN && errno != ECONNABORTED
							&& errno != EPROTO && errno != EINTR)
						epoll_panic("accept");
				}
			} else {
				dispatch(fd);
				/*char buf[BUFSIZ];
				n = 0;
				while ((nread = read(fd, buf + n, BUFSIZ - 1)) > 0) {
					n += nread;
				}
				if (nread == -1 && errno != EAGAIN) {
					perror("read error");
				}
				buf[n] = '\0';
				log_info("\n=========client:%d\n%s===========", fd, buf);

				sprintf(buf, "server: OK\r\n");
				int nwrite, data_size = strlen(buf);
				n = data_size;
				while (n > 0) {
					nwrite = write(fd, buf + data_size - n, n);
					if (nwrite < n) {
						if (nwrite == -1 && errno != EAGAIN) {
							perror("write error");
						}
						break;
					}
					n -= nwrite;
				}
				// close(fd);*/
			}

		}
	}

	return 0;
}
void epoll_close(struct epoll *epoll) {
	free(epoll);
	log_debug("关闭epoll");
}
/*
int main() {
	struct epoll *epoll = epoll_open(8080);
	epoll_loop(epoll);
	epoll_close(epoll);
	return 0;
}*/
