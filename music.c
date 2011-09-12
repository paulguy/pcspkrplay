#include <stdlib.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include "freqs.h"
#include "music.h"

song *initsong() {
	song *s;

	s = malloc(sizeof(song));
	if(s == NULL) {
		return(NULL);
	}

	s->bpm = 1;
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

	free(s);

	return(0);
}

int seeksong(song *s, int pos) {
	int i;

	s->current = s->first;
	for(i = 0; i <= pos; i++)
		s->current = s->current->next;

	return(0);
}

command *initcommand() {
	command *c;

	c = malloc(sizeof(command));
	if(c == NULL) {
		return(NULL);
	}

	c->note = NOTHING;
	c->octave = 0;
	c->divisor = 1;
	c->duration = NORMAL;
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

int playnote(int fd, command *c) {
	struct input_event ev;
	ev.type = EV_SND;
	ev.code = SND_TONE;

	if(c == NULL) {
		ev.value = 0;
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			return(-1);
		return(-1);
	} else {
		switch(c->note) {
			case C:
			case CSHARP:
			case D:
			case DSHARP:
			case E:
			case F:
			case FSHARP:
			case G:
			case GSHARP:
			case A:
			case ASHARP:
			case B:
				if(c->octave < 0 || c->octave > 6)
					return(-2);
				ev.value = (int)(notefreqs[c->octave][c->note]);
				if(write(fd, &ev, sizeof(struct input_event)) < 0)
					return(-1);
				break;
			default:
				ev.value = 0;
				if(write(fd, &ev, sizeof(struct input_event)) < 0)
					return(-1);
		}
	}
}

int playsong(int fd, song *s, void (*status)(int cur, int max)) {
	int i;
	struct timespec req, rem;
	unsigned long int notelen;

	i = 1;
	do {
		status(i, s->count);

		if(s->current->note <= REST) {
			if(playnote(fd, s->current) < 0)
				return(-1);

			notelen = 60000000000ul / s->bpm / s->current->divisor;
			if(s->current->duration == DOT) {
				notelen = notelen * 3 / 2;
			} else if(s->current->duration == TRIPLET) {
				notelen = notelen / 3;
			}
			req.tv_sec = notelen / 1000000000ul;
			req.tv_nsec = notelen % 1000000000ul;

			while(nanosleep(&req, &rem) == -1) {
				req.tv_sec = rem.tv_sec;
				req.tv_nsec = rem.tv_nsec;
			}
		} else {
			switch(s->current->note) {
				case BPM:
					s->bpm = s->current->octave;
					break;
				default:
					break;
			}
		}

		i++;
	} while(nextcommand(s) == 0);

	if(playnote(fd, NULL) < 0)
		return(-1);
}
