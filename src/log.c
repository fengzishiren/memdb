/*
 * log.c
 *
 *  Created on: 2013-9-12
 *      Author: lunatic
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "log.h"

#define STD_FORMAT "%s  %-6s  \t- %s\n"
#define TAG_FORMAT "%s  %-6s  [%s]\t- %s\n"
/**
 *
 * 日志纪录
 *
 */
static char *levels[] = { "VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
NULL };

static struct {
	enum log_level cur_level;
	const char *file_name;
	FILE *console, *of;
	size_t max_length;
	int file_index;
} logger = { .cur_level = VERBOSE, .console = NULL, .file_name = NULL, .of =
NULL, .max_length = 10 * 1024 * 1024, .file_index = 0 };

static inline long log_file_size() {
	long cur = ftell(logger.of);
	rewind(logger.of);/* point  begining logger.of file*/
	fseek(logger.of, 0, SEEK_END);
	long size = ftell(logger.of);
	fseek(logger.of, 0, cur);
	return size;
}

static inline void update_log_file() {
	char *buf = (char *) malloc(strlen(logger.file_name) + 10);
	fclose(logger.of);
	sprintf(buf, "%s%d", logger.file_name, logger.file_index + 1);
	logger.of = fopen(buf, "a");
	if (logger.of == NULL) {
		fprintf(stderr, "update log file %s failed!\n", buf);
	} else
		++logger.file_index;
	free(buf);
}

static inline char *cur_time(char *buf) {
	time_t tt;
	struct tm *t;
	tt = time(NULL);
	t = localtime(&tt);
	strftime(buf, 20, "%Y-%m-%d %H:%M:%S", t);
	return buf;
}
static inline void output(FILE *out, const char *level, const char *tag,
		const char *msg) {
	char buf[20];
	if (tag == NULL)
		fprintf(out, STD_FORMAT, cur_time(buf), level, msg);
	else
		fprintf(out, TAG_FORMAT, cur_time(buf), level, tag, msg);
	fflush(out);
}

static void format(enum log_level lv, const char *tag, const char *msg, va_list va) {
	char *buffer;
	int length = 1024;
	int n;
	/*	if (lv < logger.cur_level)
	 return;*/
	va_list args;
	/**
	 *
	 * 	The glibc implementation of the functions  snprintf()  and  vsnprintf()
	 *  conforms  to  the  C99  standard,  that is, behaves as described above,
	 *  since glibc version 2.1.  Until glibc 2.0.6 they would return  -1  when
	 *  the output was truncated.
	 *
	 * 								————摘自 man vsnprintf NOTES
	 */
	buffer = (char *) malloc(length);
	va_copy(args, va);
	n = vsnprintf(buffer, length, msg, args);
	va_end(args);

	if (n < 0) {
		free(buffer);
		return; /*ignore error!*/
	}
	if (n >= length)
		for (;;) {
			length = n + 1;
			buffer = (char *) realloc(buffer, length);
			va_copy(args, va);
			n = vsnprintf(buffer, length, msg, args);
			va_end(args);
			if (n < 0) {
				free(buffer);
				return; /*ignore error!*/
			}
			if (n < length)
				break;
		}
	if (logger.console != NULL) {
		logger.console = lv >= ERROR ? stderr : stdout;
		output(logger.console, levels[lv], tag, buffer);
	}
	if (logger.of != NULL) {
		if (log_file_size() > logger.max_length) {
			update_log_file();
		}
		if (logger.of != NULL)
			output(logger.of, levels[lv], tag, buffer);
	}
	free(buffer);
}
/*extern int strcasecmp (const char *__s1, const char *__s2);*/
enum log_level str2level(const char *level) {
	if (level == NULL)
		return VERBOSE;
	char **s = levels;
	while (*s) {
		if (strcmp(level, *s++) != 0)
			continue;
		else
			break;
	}
	return *s == NULL ? VERBOSE : (enum log_level) (s - 1 - levels);
}

char *level2str(enum log_level lv) {
	return levels[lv];
}

void log_init(enum log_level lv, const char *file, size_t _max_length) {
	logger.cur_level = lv;
	logger.console = stdout;
	if (file == NULL)
		return;
	logger.file_name = file;
	logger.max_length = _max_length == 0 ? 1024 * 1024 : _max_length;

	logger.of = fopen(file, "a");
	if (logger.of == NULL) {
		fprintf(stderr, "open log file %s failed!", file);
		abort();
	}
}

void log_log(enum log_level lv, const char *tag, const char *msg, ...) {
	if (lv < logger.cur_level)
		return;
	va_list va;
	va_start(va, msg);
	format(lv, tag, msg, va);
	va_end(va);
}

void log_debug(const char *msg_format, ...) {
	if (DEBUG < logger.cur_level)
		return;
	va_list va;
	va_start(va, msg_format);
	format(DEBUG, NULL, msg_format, va);
	va_end(va);
}
void log_info(const char *msg_format, ...) {
	if (INFO < logger.cur_level)
		return;
	va_list va;
	va_start(va, msg_format);
	format(INFO, NULL, msg_format, va);
	va_end(va);
}
void log_warn(const char *msg_format, ...) {
	if (WARN < logger.cur_level)
		return;
	va_list va;
	va_start(va, msg_format);
	format(WARN, NULL, msg_format, va);
	va_end(va);
}

void log_error(const char *msg_format, ...) {
	va_list va;
	va_start(va, msg_format);
	format(ERROR, NULL, msg_format, va);
	va_end(va);
}

void panic(const char *err, ...) {
	va_list va;
	va_start(va, err);
	format(FATAL, NULL, err, va);
	va_end(va);
	abort();
}
