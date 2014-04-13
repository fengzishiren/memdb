/*
 * log.h
 *
 *  Created on: 2014年4月10日
 *      Author: lunatic
 */
#ifndef LOG_H_
#define LOG_H_

/**
 * 日志部分
 */
enum Level {
	VERBOSE, DEBUG, INFO, WARN, ERROR, FATAL
};
/**
 *
 * 终止程序运行
 *
 *
 */
void panic(const char *err, ...);
/**
 *
 * Log基本操作
 *
 */
/*
 *
 * 如果找不到对应的leve级别 则返回默认VERBOSE
 *
 */
enum Level str2level(const char *level);
/*
 * 默认开启控制台log，file设置为非NULL， 则开启同时将log输出只文件
 * @param file == NULL log不输出到文件
 *
 */
void log_init(enum Level lv, const char *file, size_t max_len);
/*
 *
 * 2013-12-21 19:59:11  DEBUG   [main.c]	- FUCK (其中tag 为 main.c)
 * if (tag == NULL)
 * 	2013-12-21 19:59:11  DEBUG   	- fuck
 */
void log_log(enum Level lv, const char *tag, const char *format, ...);

/*
 * @brief debug级的格式化log 等价于debug() + printf()
 *
 * Note
 * 	原生输出：2013-11-07 12:05:47  DEBUG
 */
void log_debug(const char *, ...);

/*
 * @brief info级的格式化log 等价于info() + printf()
 *
 * Note
 * 	输出不带换行符号（'\n'）
 * 	原生输出：2013-11-07 12:05:47  INFO
 */
void log_info(const char *, ...);

/*
 * @brief warn级的格式化log 等价于warn() + printf()
 *
 * Note
 * 	输出不带换行符号（'\n'）
 * 	原生输出：2013-11-07 12:05:47  WARN
 */
void log_warn(const char *, ...);

/*
 * @brief error级的格式化log 等价于error() + printf()
 * Note
 * 	输出不带换行符号（'\n'）
 * 	原生输出：2013-11-07 12:05:47  ERROR
 */
void log_error(const char *, ...);

#endif /* LOG_H_ */
