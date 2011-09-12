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

	octave = 3;
	divisor = 32;
	duration = NORMAL;
	s->bpm = 50;

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
								if(t > 0) {
									addnote = 1;
									note = BPM;
									data = t;
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

			if(data == 0) {
				c->octave = octave;
			} else {
				c->octave = data;
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

	return(s);
}
