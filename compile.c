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
	char tmpstr[11] = "00000000"; /* maximum number is 16777215 */
	int t;
	int pos;
	song *s;
	command *c;

	/* transient state */
	int addnew;
	int cmd;
	int data; /* ovarrides octave for special commands */
	int reg;

	s = initsong();
	if(s == NULL)
		return(NULL);

	chartolower(songstr, length);
	for(pos = 0; pos < length; pos++) {
		addnew = 0;
		cmd = 0;
		reg = 0;
		data = 0;

		cur = songstr[pos];

		/* start with larger strings first */
		if(pos < length - 1) {
			if(strncmp(&(songstr[pos]), "c#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTECS);
			} else if(strncmp(&(songstr[pos]), "c$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTECF);
			} else if(strncmp(&(songstr[pos]), "d#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEDS);
			} else if(strncmp(&(songstr[pos]), "d$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEDF);
			} else if(strncmp(&(songstr[pos]), "e#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEES);
			} else if(strncmp(&(songstr[pos]), "e$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEEF);
			} else if(strncmp(&(songstr[pos]), "f#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEFS);
			} else if(strncmp(&(songstr[pos]), "f$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEFF);
			} else if(strncmp(&(songstr[pos]), "g#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEGS);
			} else if(strncmp(&(songstr[pos]), "g$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEGF);
			} else if(strncmp(&(songstr[pos]), "a#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEAS);
			} else if(strncmp(&(songstr[pos]), "a$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEAF);
			} else if(strncmp(&(songstr[pos]), "b#", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEBS);
			} else if(strncmp(&(songstr[pos]), "b$", 2) == 0) {
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEBF);
			}
		}
		if(addnew == 1) {
			pos++;
		} else {
			switch(songstr[pos]) {
			case 'c':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEC);
				break;
			case 'd':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTED);
				break;
			case 'e':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEE);
				break;
			case 'f':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEF);
				break;
			case 'g':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEG);
				break;
			case 'a':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEA);
				break;
			case 'b':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEB);
				break;
			case 'x':
				addnew = 1;
				cmd = NOTE;
				data = IMMED(NOTEREST);
				break;
			case '0':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(0);
				break;
			case '1':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(1);
				break;
			case '2':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(2);
				break;
			case '3':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(3);
				break;
			case '4':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(4);
				break;
			case '5':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(5);
				break;
			case '6':
				addnew = 1;
				cmd = MOV;
				reg = REGOCTAVE;
				data = IMMED(6);
				break;
			case '-':
				addnew = 1;
				cmd = SUB;
				reg = REGOCTAVE;
				data = IMMED(1);
				break;
			case '+':
				addnew = 1;
				cmd = ADD;
				reg = REGOCTAVE;
				data = IMMED(1);
				break;
			case 'z':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(64);
				break;
			case 't':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(32);
				break;
			case 's':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(16);
				break;
			case 'i':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(8);
				break;
			case 'q':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(4);
				break;
			case 'h':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(2);
				break;
			case 'w':
				addnew = 1;
				cmd = MOV;
				reg = REGDIVISOR;
				data = IMMED(1);
				break;
			case '.':
				addnew = 1;
				cmd = MOV;
				reg = REGDURATION;
				data = IMMED(DDOT);
				break;
			case '!':
				addnew = 1;
				cmd = MOV;
				reg = REGDURATION;
				data = IMMED(DTRIPLET);
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
							if(pos + 1 < length) {
								if(songstr[pos + 1] >= 'a' && songstr[pos + 1] <= 'z' ) {
									addnew = 1;
									cmd = MOV;
									reg = REGBPM;
									data = LETTOREG(songstr[pos + 1]);
									pos++;
								} else if(pos + 3 < length) {
									tmpstr[0] = songstr[pos + 1];
									tmpstr[1] = songstr[pos + 2];
									tmpstr[2] = songstr[pos + 3];
									tmpstr[3] = '\0';
									t = atoi(tmpstr);
									if(t > 1) {
										addnew = 1;
										cmd = MOV;
										reg = REGBPM;
										data = IMMED(t);
									} else {
										return(NULL);
									}
									pos += 3;
								} else {
									return(NULL);
								}
							}
							break;
						case 'd':
							if(pos + 1 < length) {
								if(songstr[pos + 1] >= 'a' && songstr[pos + 1] <= 'z' ) {
									addnew = 1;
									cmd = MOV;
									reg = REGDIVISOR;
									data = LETTOREG(songstr[pos + 1]);
									pos++;
								} else if(pos + 3 < length) {
									tmpstr[0] = songstr[pos + 1];
									tmpstr[1] = songstr[pos + 2];
									tmpstr[2] = songstr[pos + 3];
									tmpstr[3] = '\0';
									t = atoi(tmpstr);
									if(t > 1) {
										addnew = 1;
										cmd = MOV;
										reg = REGDIVISOR;
										data = t;
									} else {
										return(NULL);
									}
									pos += 3;
								} else {
									return(NULL);
								}
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

		if(addnew == 1) {
			c = initcommand();
			if(c == NULL) {
				freesong(s);
				return(NULL);
			}

			c->cmd = cmd;
			c->data = data;
			c->reg = reg;
			addcmd(c, s);
		}
	}

	if(s->first == NULL) {
		freesong(s);
		return(NULL);
	}
	return(s);
}
/*
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
*/
