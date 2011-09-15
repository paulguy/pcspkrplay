#ifndef _STR_H
#define _STR_H

#define READBUFFER (16)

typedef struct str {
	char *data;
	int strsize;
	int memsize;
} str;

str *initstr();
void freestr(str *t);
int concatchar(str* s, char *data, int len);
str *readstream(FILE *in);

#endif
