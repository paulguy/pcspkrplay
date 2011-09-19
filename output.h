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

#ifndef _OUTPUT_H
#define _OUTPUT_H

#define THROW(x) return(mkexception(x, vm, s)

#define FLAG_EQUAL		(1)
#define FLAG_GREATER	(2)

#include "music.h"

typedef struct vmstate {
	unsigned int regs[26];
	unsigned int flags;
} vmstate;

typedef enum {
	PROGRAM_ENDED = 0,
	ILLEGAL_INSTRUCTION,
	BAD_ARGUMENT,
	BAD_REGISTER,
	DEVICE_ERROR
} exceptions;

char exstr[5][20] = {
	"PROGRAM_ENDED",
	"ILLEGAL_INSTRUCTION",
	"BAD_ARGUMENT",
	"BAD_REGISTER",
	"DEVICE_ERROR"
};

char exstrlong[5][52] = {
	"The program reached it's end with no error.",
	"An illegal instruction was encountered.",
	"A bad value was used in the argument field.",
	"A nonexistent register was attempted to be used.",
	"An error with a device occurred."
};

typedef struct vmexception {
	exceptions e;
	vmstate *vm;
	song *s;
} vmexception;

int playnote(int fd, int note, int octave);
vmexception *playsong(int fd, song *s, void (*status)(int cur, int max));
vmexception *mkexception(exception ex, vmstate *vm, song *s);
void printexception(vmexception *e, FILE *out);

#endif
