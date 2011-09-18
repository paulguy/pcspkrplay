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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compile.h"
#include "util.h"

song *compilesong(char *songstr, int length) {
	char cur;
	char tmpstr[4] = {0, 0, 0, 0};
	int t;
	int pos;
	song *s;
	command *c;

	/* long term */
	int octave;
	int divisor;
	duration duration;

	/* transient state */
	note note;
	int addnote;
	int data; /* ovarrides octave for special commands */

	s = initsong();
	if(s == NULL)
		return(NULL);

	octave = DEFAULT_OCTAVE;
	divisor = DEFAULT_DIVISOR;
	duration = NORMAL;
	s->bpm = DEFAULT_BPM;

	chartolower(songstr, length);
	for(pos = 0; pos < length; pos++) {
		addnote = 0;
		data = 0;

		cur = songstr[pos];

		/* start with larger strings first */
		if(pos < length - 1) {
			if(strncmp(&(songstr[pos]), "c#", 2) == 0) {
				addnote = 1;
				note = CSHARP;
			} else if(strncmp(&(songstr[pos]), "c$", 2) == 0) {
				addnote = 1;
				if(octave == 0) {
					note = C;
				} else {
					note = PREVB;
				}
			} else if(strncmp(&(songstr[pos]), "d#", 2) == 0) {
				addnote = 1;
				note = DSHARP;
			} else if(strncmp(&(songstr[pos]), "d$", 2) == 0) {
				addnote = 1;
				note = CSHARP;
			} else if(strncmp(&(songstr[pos]), "e#", 2) == 0) {
				addnote = 1;
				note = F;
			} else if(strncmp(&(songstr[pos]), "e$", 2) == 0) {
				addnote = 1;
				note = DSHARP;
			} else if(strncmp(&(songstr[pos]), "f#", 2) == 0) {
				addnote = 1;
				note = FSHARP;
			} else if(strncmp(&(songstr[pos]), "f$", 2) == 0) {
				addnote = 1;
				note = E;
			} else if(strncmp(&(songstr[pos]), "g#", 2) == 0) {
				addnote = 1;
				note = GSHARP;
			} else if(strncmp(&(songstr[pos]), "g$", 2) == 0) {
				addnote = 1;
				note = FSHARP;
			} else if(strncmp(&(songstr[pos]), "a#", 2) == 0) {
				addnote = 1;
				note = ASHARP;
			} else if(strncmp(&(songstr[pos]), "a$", 2) == 0) {
				addnote = 1;
				note = GSHARP;
			} else if(strncmp(&(songstr[pos]), "b#", 2) == 0) {
				addnote = 1;
				if(octave == 6) {
					note = C;
				} else {
					note = NEXTC;
				}
			} else if(strncmp(&(songstr[pos]), "b$", 2) == 0) {
				addnote = 1;
				note = ASHARP;
			}
		}
		if(addnote == 1) {
			pos++;
		} else {
			switch(songstr[pos]) {
			case 'c':
				addnote = 1;
				note = C;
				break;
			case 'd':
				addnote = 1;
				note = D;
				break;
			case 'e':
				addnote = 1;
				note = E;
				break;
			case 'f':
				addnote = 1;
				note = F;
				break;
			case 'g':
				addnote = 1;
				note = G;
				break;
			case 'a':
				addnote = 1;
				note = A;
				break;
			case 'b':
				addnote = 1;
				note = B;
				break;
			case 'x':
				addnote = 1;
				note = REST;
				break;
			case '0':
				octave = 0;
				break;
			case '1':
				octave = 1;
				break;
			case '2':
				octave = 2;
				break;
			case '3':
				octave = 3;
				break;
			case '4':
				octave = 4;
				break;
			case '5':
				octave = 5;
				break;
			case '6':
				octave = 6;
				break;
			case '-':
				if(octave > 0) {
					octave--;
				}
				break;
			case '+':
				if(octave > 0) {
					octave++;
				}
				break;
			case 'z':
				divisor = 64;
				break;
			case 't':
				divisor = 32;
				break;
			case 's':
				divisor = 16;
				break;
			case 'i':
				divisor = 8;
				break;
			case 'q':
				divisor = 4;
				break;
			case 'h':
				divisor = 2;
				break;
			case 'w':
				divisor = 1;
				break;
			case '.':
				if(duration == DOT) {
					duration = NORMAL;
				} else {
					duration = DOT;
				}
				break;
			case '!':
				if(duration == TRIPLET) {
					duration = NORMAL;
				} else {
					duration = TRIPLET;
				}
				break;
			case '&':
				while(pos < length) {
					pos++;
					if(songstr[pos] == '&') {
						break;
					}
				}
				break;
			case '*':
				pos++;
				if(pos < length) {
					switch(songstr[pos]) {
						case 'b':
							if(pos + 3 < length) {
								tmpstr[0] = songstr[pos + 1];
								tmpstr[1] = songstr[pos + 2];
								tmpstr[2] = songstr[pos + 3];
								t = atoi(tmpstr);
								if(t > 1) {
									addnote = 1;
									note = BPM;
									data = t;
								} else {
									return(NULL);
								}
								pos += 3;
							}
							break;
						case 'd':
							if(pos + 3 < length) {
								tmpstr[0] = songstr[pos + 1];
								tmpstr[1] = songstr[pos + 2];
								tmpstr[2] = songstr[pos + 3];
								t = atoi(tmpstr);
								if(t > 1) {
									divisor = t;
								} else {
									return(NULL);
								}
								pos += 3;
							}
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
			}
		}

		if(addnote == 1) {
			c = initcommand();
			if(c == NULL) {
				freesong(s);
				return(NULL);
			}

			if(data != 0) {
				c->octave = data;
			} else {
				c->octave = octave;
			}
			c->divisor = divisor;
			c->duration = duration;
			if(note == NEXTC) {
				if(c->octave == 6) {
					c->note = B;
				} else {
					c->octave++;
					c->note = C;
				}
			} else if(note == PREVB) {
				if(c->octave == 0) {
					c->note = C;
				} else {
					c->octave--;
					c->note = B;
				}
			} else {
				c->note = note;
			}

			addcmd(c, s);
		}
	}

	if(s->first == NULL) {
		freesong(s);
		return(NULL);
	}
	return(s);
}

str *decompilesong(song *s) {
	str *out;
	char tnum[] = "000";
	unsigned int octave;
	unsigned int divisor;
	duration duration;

	octave = DEFAULT_OCTAVE;
	divisor = DEFAULT_DIVISOR;
	duration = NORMAL;

	if(s->current == NULL)
		return(NULL);

	out = initstr();
	if(out == NULL)
		return(NULL);

	do {
		if(s->current->octave != octave && s->current->note != BPM) {
			if(s->current->octave <= 6) {
				tnum[0] = '0' + s->current->octave;
				concatchar(out, tnum, 1);
				octave = s->current->octave;
			} else {
				freestr(out);
				return(NULL);
			}
		}
		if(s->current->divisor != divisor) {
			switch(s->current->divisor) {
				case 1:
					concatchar(out, "w", 1);
					break;
				case 2:
					concatchar(out, "h", 1);
					break;
				case 4:
					concatchar(out, "q", 1);
					break;
				case 8:
					concatchar(out, "i", 1);
					break;
				case 16:
					concatchar(out, "s", 1);
					break;
				case 32:
					concatchar(out, "t", 1);
					break;
				case 64:
					concatchar(out, "z", 1);
					break;
				default:
					if(s->current->divisor > 999 || s->current->divisor < 1) {
						freestr(out);
						return(NULL);
					}
					concatchar(out, "*d", 2);
					tnum[0] = '0' + (s->current->divisor / 100);
					tnum[1] = '0' + (s->current->divisor / 10 % 10);
					tnum[2] = '0' + (s->current->divisor % 10);
					concatchar(out, tnum, 3);
					break;
			}
			divisor = s->current->divisor;
		}
		if(s->current->duration != duration) {
			switch(s->current->duration) {
				case DOT:
					concatchar(out, ".", 1);
					break;
				case TRIPLET:
					concatchar(out, "!", 1);
				 	break;
				case NORMAL:
					switch(duration) {
						case DOT:
							concatchar(out, ".", 1);
							break;
						case TRIPLET:
							concatchar(out, "!", 1);
							break;
						default:
							freestr(out);
							return(NULL);
							break;
					}
					break;
				default:
					freestr(out);
					return(NULL);
					break;
			}
			duration = s->current->duration;
		}
		switch(s->current->note) {
			case C:
				concatchar(out, "c", 1);
				break;
			case CSHARP:
				concatchar(out, "c#", 2);
				break;
			case D:
				concatchar(out, "d", 1);
				break;
			case DSHARP:
				concatchar(out, "d#", 2);
				break;
			case E:
				concatchar(out, "e", 1);
				break;
			case F:
				concatchar(out, "f", 1);
				break;
			case FSHARP:
				concatchar(out, "f#", 2);
				break;
			case G:
				concatchar(out, "g", 1);
				break;
			case GSHARP:
				concatchar(out, "g#", 2);
				break;
			case A:
				concatchar(out, "a", 1);
				break;
			case ASHARP:
				concatchar(out, "a#", 2);
				break;
			case B:
				concatchar(out, "b", 1);
				break;
			case REST:
				concatchar(out, "x", 1);
				break;
			case BPM:
				if(s->current->octave > 999 || s->current->octave < 1) {
					freestr(out);
					return(NULL);
				}
				concatchar(out, "*b", 2);
				tnum[0] = '0' + (s->current->octave / 100);
				tnum[1] = '0' + (s->current->octave / 10 % 10);
				tnum[2] = '0' + (s->current->octave % 10);
				concatchar(out, tnum, 3);
				break;
			default:
				freestr(out);
				return(NULL);
				break;
		}
	} while(nextcommand(s) == 0);

	return(out);
}
