/*
 * protocol.h
 *
 *  Created on: 2014年4月12日
 *      Author: epay111
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/**
 *
 * Redis 命令会返回多种不同类型的回复。

 通过检查服务器发回数据的第一个字节， 可以确定这个回复是什么类型：

 状态回复（status reply）的第一个字节是 "+"
 错误回复（error reply）的第一个字节是 "-"
 整数回复（integer reply）的第一个字节是 ":"


 多条批量回复（multi bulk reply）的第一个字节是 "*"
 *
 *
 *
 *
 */
/*
 *
 * 客户端： LRANGE mylist 0 3
 * 服务器： *4
 * 服务器： $3
 * 服务器： foo
 * 服务器： $3
 * 服务器： bar
 * 服务器： $5
 * 服务器： Hello
 * 服务器： $5
 * 服务器： World
 *
 * *5\r\n
 * :1\r\n
 * :2\r\n
 * :3\r\n
 * :4\r\n
 * $6\r\n
 * foobar\r\n
 */
#define PRO_MULTI "*"
/*
 * 客户端：GET mykey
 * 服务器：foobar
 * "$6\r\nfoobar\r\n"
 * 批量回复（bulk reply）的第一个字节是 "$"
 */
#define PRO_BULK "$"
#define PRO_INTEGER ":"
#define PRO_STATUS "+"
#define PRO_ERROR "-"

#define PRO_NEW_LINE "\r\n"

#define PRO_OK "+OK\r\n"

#define PRO_ERROR "-ERROR %s\r\n"
#define PRO_TYPE_ERROR "-WRONGTYPE %s\r\n"

#define PRO_INTEGER_DATA ":%lld\r\n"
#define PRO_STRING_DATA "$%d\r\n%s\rn"

#define PRO_NULL "$-1\r\n"

#endif /* PROTOCOL_H_ */
