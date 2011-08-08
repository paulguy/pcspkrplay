#include <stdlib.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include "freqs.h"
#include "music.h"

void playnote(int fd, command *cmd) {
	struct input_event ev;
	ev.type = EV_SND;
	ev.code = SND_TONE;

	if(cmd == NULL) {
		ev.value = 0;
		write(fd, &ev, sizeof(struct input_event));
	} else {
		if(cmd->note != REST) {
			ev.value = notefreqs[cmd->octave][cmd->note];
			write(fd, &ev, sizeof(struct input_event));
		} else {
			ev.value = 0;
			write(fd, &ev, sizeof(struct input_event));
		}
	}
}

void playsong(int fd, song *song, void (*status)(int cur, int max)) {
	int i;
	struct timespec req, rem;
	unsigned long int notelen;
	command *cmd;

	cmd = song->first;

	while(cmd->next != NULL) {
		if(cmd->note != NOTHING) {
			playnote(fd, cmd);

			notelen = 60000000000ul / song->bpm / cmd->divisor;
			if(cmd->duration == DOT) {
				notelen = notelen * 3 / 2;
			} else if(cmd->duration == TRIPLET) {
				notelen = notelen / 3;
			}
			req.tv_sec = notelen / 1000000000ul;
			req.tv_nsec = notelen % 1000000000ul;

			while(nanosleep(&req, &rem) == -1) {
				req.tv_sec = rem.tv_sec;
				req.tv_nsec = rem.tv_nsec;
			}
		}

		cmd = (command *)cmd->next;
	}

	playnote(fd, NULL);
}
