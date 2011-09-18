#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#include "music.h"
#include "util.h"
#include "compile.h"
#include "output.h"
#include "str.h"

char DEFAULT_DEVICE[] =  "/dev/input/by-path/platform-pcspkr-event-spkr";

void statusout(int cur, int max) {
	fprintf(stderr, "%u/%u\r", cur, max);
}

void nostatus(int cur, int max) {
	return;
}

void usage(char *name) {
	fprintf(stderr, "USAGE: %s <options>\n\
Options:\n\
--decompile / -d          - Decompile string, used for outputting size-optimized code.\n\
--device    / -e device   - Select event device, default is %s.\n\
--filename  / -f filename - Read from filename instead of standard input.\n\
--quiet     / -q          - Don't play, only compile (and optionally decompile) then quit.\n\
--display   / -s          - Display playback status.\n\
--help      / -h          - Print help/usage.  Refer to README for more information.\n\
", name, DEFAULT_DEVICE);
	exit(-1);
}

int main(int argc, char **argv) {
	int i;

	int speaker;
	int sndcap;

	song *s;
	str *sstr;

	char arg;
	char *device;
	FILE *in;
	int decompile, playback, display;

	device = DEFAULT_DEVICE;
	in = stdin;
	decompile = 0;
	playback = 1;
	display = 0;

	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strlen(argv[i]) < 2)
				usage(argv[0]);
			if(argv[i][1] == '-') {
				if(strlen(argv[i]) < 3)
					usage(argv[0]);
				if(strcmp(&(argv[i][2]), "decompile") == 0)
					arg = 'd';
				else if(strcmp(&(argv[i][2]), "device") == 0)
					arg = 'e';
				else if(strcmp(&(argv[i][2]), "filename") == 0)
					arg = 'f';
				else if(strcmp(&(argv[i][2]), "help") == 0)
					arg = 'h';
				else if(strcmp(&(argv[i][2]), "quiet") == 0)
					arg = 'q';
				else if(strcmp(&(argv[i][2]), "display") == 0)
					arg = 'd';
				else
					usage(argv[0]);
			} else {
				arg = argv[i][1];
			}
			switch(arg) {
				case 'd':
					decompile = 1;
					break;
				case 'e':
					if(i + 1 < argc) {
						device = argv[i + 1];
						i++;
					} else {
						usage(argv[0]);
					}
					break;
				case 'f':
					if(i + 1 < argc) {
						if(in != stdin)
							fclose(in);
						in = fopen(argv[i + 1], "rb");
						if(in == NULL) {
							fprintf(stderr, "Couldn't open file %s.\n", argv[i + 1]);
							exit(-1);
						}
						i++;
					} else {
						usage(argv[0]);
					}
					break;
				case 'q':
					playback = 0;
					break;
				case 's':
					display = 1;
					break;
				case 'h':
				default:
					usage(argv[0]);
					break;
			}
		} else {
			usage(argv[0]);
		}
	}

	speaker = open(device, O_RDWR);
	if(speaker == -1) {
		perror("open");
		exit(-2);
	}

	if(ioctl(speaker, EVIOCGBIT(0, EV_MAX), &sndcap) < 0) {
		perror("ioctl");
		exit(-3);
	}

	if(sndcap & EV_SND) {
		fprintf(stderr, "Device %s does not support sound.\n", argv[1]);
		exit(-4);
	}

	sstr = initstr();
	if(sstr == NULL)
		return(-1);

	sstr = readstream(in);
	s = compilesong(sstr->data, sstr->strsize);
	if(s == NULL) {
		fprintf(stderr, "Song compile failed.\n");
		exit(-5);
	}
	freestr(sstr);
	if(decompile == 1) {
		rewindsong(s);
		sstr = decompilesong(s);
		fprintf(stderr, "%s\n", sstr->data);
	}
	if(playback == 1) {
		rewindsong(s);
		if(display == 1) {
			playsong(speaker, s, statusout);
			fprintf(stderr, "\n");
		} else {
			playsong(speaker, s, nostatus);
		}
	}

	exit(0);
}
