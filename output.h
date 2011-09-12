#ifndef _OUTPUT_H
#define _OUTPUT_H

#include "music.h"

int playnote(int fd, command *c);
int playsong(int fd, song *s, void (*status)(int cur, int max));

#endif
