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

#define FLAG_EQUAL		(1)
#define FLAG_GREATER	(2)

#include <stdio.h>

#include "music.h"

typedef struct vmstate {
	unsigned int regs[26];
	unsigned int flags;
	command *stack[16];
	unsigned int sp;
	command *last;
} vmstate;

typedef enum {
	PROGRAM_ENDED = 0,
	ILLEGAL_INSTRUCTION,
	BAD_ARGUMENT,
	BAD_REGISTER,
	DEVICE_ERROR,
	VM_ERROR,
	BREAKPOINT
} vmexception;

int playnote(int fd, int note, int octave);
vmexception playsong(int fd, song *s, vmstate *vm, void (*status)(int cur, int max), int step);
void printexception(vmexception e, song *s, vmstate *vm, FILE *out);
vmstate *initvm();

#endif
