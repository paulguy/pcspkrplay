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

void statusout(int cur, int max) {
	fprintf(stderr, "%u/%u\r", cur, max);
}

int main(int argc, char **argv) {
	int speaker;
	song *s;
	str *sstr;

	int sndcap;

	if(argc < 2) {
		fprintf(stderr, "USAGE: %s <path to event device>\n", argv[0]);
		exit(-1);
	}

	speaker = open(argv[1], O_RDWR);
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

	sstr = readstream(stdin);
	s = compilesong(sstr->data, sstr->strsize);
	if(s == NULL) {
		fprintf(stderr, "Song compile failed.\n");
		exit(-5);
	}
	freestr(sstr);

	rewindsong(s);
	sstr = decompilesong(s);
	fprintf(stderr, "%s\n", sstr->data);
	rewindsong(s);
	playsong(speaker, s, statusout);
	fprintf(stderr, "\n");

	exit(0);
}
