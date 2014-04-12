/*
 * protocol.c
 *
 *  Created on: 2014年4月11日
 *      Author: epay111
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 *
 * *3 \r\n $3 \r\n SET \r\n $5 \r\n mykey\r\n $7 \r\n myvalue \r\n
 *
 */

struct args {
	int argc;
	char **argv;
} args = { .argv = NULL };

static int split(char *data) {
	char *p = data;
	if (*p != '*')
		goto err;

	while (*p) {
		if (*p == '\r' && *(p + 1) == '\n')
			break;
		p++;
	}
	if (*p == '\0')
		goto err;
	*p = '\0';
	p += 2;

	args.argc = (int) strtol(data + 1, NULL, 10);
	if (args.argc < 1)
		goto err;
	args.argv = malloc(sizeof(char *) * args.argc);

	char *tok;
	int i;
	for (i = 0; (tok = strtok(p, "\r\n")); ++i) {
		if (*tok != '$')
			goto err;

		int len = (int) strtol(tok + 1, NULL, 10);
		tok = strtok(NULL, "\r\n");
		if (tok == NULL || strlen(tok) != len)
			goto err;

		if (i >= args.argc)
			goto err;

		args.argv[i] = tok;

		if (p)
			p = NULL;
	}

	if (i != args.argc)
		goto err;

	return 0;

	err: return -1;
}
/*void parse(struct string *ss) {
 int i;
 for (i = 0; i < ss->length; ++i) {

 }
 }*/
extern void execute(int argc, char **argv);
int _main() {

	char req[] = "*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n";
	//char req[] = "*2\r\n$3\r\nGET\r\n$4\r\nname\r\n";
	split(req);

	int i;
	for (i = 0; i < args.argc; ++i) {
		printf("argc: %d\t argv: %s\n", i, args.argv[i]);
	}
	execute(args.argc, args.argv);
	return 0;
}
