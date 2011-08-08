#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "music.h"
#include "util.h"

int compilesong_MZX(song *song, char *songstr, int length) {
	char cur;
	int pos;
	command *cmd;

	/* long term */
	int octave;
	int divisor;
	duration duration;

	/* transient state */
	note note;
	int addnote;

	octave = 3;
	divisor = 32;
	duration = NORMAL;

	chartolower(songstr, length);

	song->bpm = 50;
	song->count = 1;

	song->first = malloc(sizeof(command));
	if(cmd == NULL) {
		return -1;
	}
	cmd = song->first;
	cmd->note = NOTHING;

	for(pos = 0; pos < length; pos++) {
		addnote = 0;

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
				if(duration != NORMAL) {
					duration = DOT;
				}
				break;
			case '!':
				if(duration != NORMAL) {
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
			default:
				break;
			}
		}

		if(addnote == 1) {
			cmd->octave = octave;
			cmd->divisor = divisor;
			cmd->duration = duration;
			if(note == NEXTC) {
				if(cmd->octave == 6) {
					cmd->note = B;
				} else {
					cmd->octave++;
					cmd->note = C;
				}
			} else if(note == PREVB) {
				if(cmd->octave == 0) {
					cmd->note = C;
				} else {
					cmd->octave--;
					cmd->note = B;
				}
			} else {
				cmd->note = note;
			}

			cmd->next = malloc(sizeof(command));
			if(cmd->next == NULL) {
				return -1;
			}
			cmd = cmd->next;
			cmd->note = NOTHING;

			song->count++;
		}
	}

	return 0;
}
