/*
 * Copyright (c) 2011 Paul LaMendola
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "music.h"

song *initsong() {
	song *s;

	s = malloc(sizeof(song));
	if(s == NULL) {
		return(NULL);
	}

	s->count = 0;
	s->first = NULL;
	s->last = NULL;
	s->current = NULL;

	return(s);
}

int freesong(song *s) {
	command *cur;
	command *next;

	cur = s->first;
	while(cur != NULL) {
		next = cur->next;
		if(freecommand(cur) < 0)
			return(-1);
		cur = next;
	}

	free(s->offsets);
	free(s);

	return(0);
}

int seeksong(song *s, int pos) {
	int i;

	if(pos > s->count)
		return(-2);
	s->current = s->offsets[pos];

	return(0);
}

command *initcommand() {
	command *c;

	c = malloc(sizeof(command));
	if(c == NULL) {
		return(NULL);
	}

	c->cmd = NOP;
	c->reg = REGA;
	c->data = IMMED(0);
	c->prev = NULL;
	c->next = NULL;

	return(c);
}

int freecommand(command *c) {
	if(c == NULL)
		return(-2);

	free(c);

	return(0);
}

int addcmd(command *c, song *s) {
	if(s->current == NULL) { /* Either no data or appending */
		if(s->first == NULL) { /* No data yet */
			s->first = c;
			s->last = c;
			s->current = NULL;
		} else { /* Appending */
			s->last->next = c;
			c->prev = s->last;
			s->last = c;
			s->current = NULL;
		}
	} else { /* Add note before current position */
		if(s->current == s->first) { /* Prepending */
			s->current->prev = c;
			c->next = s->current;
			s->first = c;
		} else { /* Add before current */
			c->prev = s->current->prev;
			c->next = s->current;
			s->current->prev->next = c;
			s->current->prev = c;
		}
	}

	s->count++;

	return(0);
}

command *delcmd(song *s) {
	command *cur, *set;

	if(s->current == NULL) /* No commands */
		return(NULL);

	cur = s->current;
	if(s->current == s->first) {
		if(s->current == s->last) { /* only 1 command */
			s->first = NULL;
			s->last = NULL;
			s->current = NULL;
		} else { /* At beginning */
			set = s->current->next;
			s->first = s->current->next;
			s->current->next->prev = NULL;
			s->current->next = NULL;
			s->current = set;
		}
	} else {
		if(s->current == s->last) { /* At end */
			s->last = s->current->prev;
			s->current->prev->next = NULL;
			s->current->prev = NULL;
			s->current = NULL;
		} else { /* In the middle somewhere */
			set = s->current->next;
			s->current->prev->next = s->current->next;
			s->current->next->prev = s->current->prev;
			s->current = set;
		}
	}

	return(cur);
}
	
int rewindsong(song *s) {
	s->current = s->first;

	return(0);
}

int nextcommand(song *s) {
	if(s->current == NULL)
		return(-2);

	s->current = s->current->next;

	if(s->current == NULL)
		return(1);
	return(0);
}

int prevcommand(song *s) {
	if(s->current->prev == NULL)
		return(-2);

	s->current = s->current->prev;

	return(0);
}
