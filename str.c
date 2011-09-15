#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "str.h"

str *initstr() {
	str *t;

	t = malloc(sizeof(str));
	t->data = malloc(1);
	t->data[0] = '\0';
	t->strsize = 1;
	t->memsize = 1;

	return(t);
}

void freestr(str *t) {
	free(t->data);
	free(t);
}

int concatchar(str* s, char *data, int len) {
	int target;
	int newsize;
	char *t;

	target = len + s->strsize;
	if(target > s->memsize) {
		newsize = s->memsize;
		while(newsize < target)
			newsize *= 2;
		t = realloc(s->data, newsize);
		if(t == NULL)
			return(-1);
		s->data = t;
	} else {
		newsize = s->memsize;
	}

	memcpy(&(s->data[s->strsize - 1]), data, len);
	s->strsize = target;
	s->memsize = newsize;
	s->data[s->strsize - 1] = '\0';
	return(0);
}

str *readstream(FILE *in) {
	str *data;
	int size;
	char buffer[READBUFFER];

	data = initstr();
	if(data == NULL)
		return(NULL);

	while(!feof(in) || size == 0) {
		size = fread(&buffer, 1, READBUFFER, in);
		if(concatchar(data, buffer, size) < 0) {
			freestr(data);
			return(NULL);
		}
	}

	return(data);
}
