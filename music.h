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

#ifndef _MUSIC_H
#define _MUSIC_H

#define DEFAULT_OCTAVE (3)
#define DEFAULT_DIVISOR (32)
#define DEFAULT_BPM (50)

#define NOTEREST	(0)

#define NOTECF		(14)
#define NOTEC		(1)
#define NOTECS		(2)

#define NOTEDF		(2)
#define NOTED		(3)
#define NOTEDS		(4)

#define NOTEEF		(4)
#define NOTEE		(5)
#define NOTEES		(6)

#define NOTEFF		(5)
#define NOTEF		(6)
#define NOTEFS		(7)

#define NOTEGF		(7)
#define NOTEG		(8)
#define NOTEGS		(9)

#define NOTEAF		(9)
#define NOTEA		(10)
#define NOTEAS		(11)

#define NOTEBF		(11)
#define NOTEB		(12)
#define NOTEBS		(13)

#define REGA		(0)
#define REGB		(1)
#define REGC		(2)
#define REGD		(3)
#define REGE		(4)
#define REGF		(5)
#define REGG		(6)
#define REGH		(7)
#define REGI		(8)
#define REGJ		(9)
#define REGK		(10)
#define REGL		(11)
#define REGM		(12)
#define REGN		(13)
#define REGO		(14)
#define REGP		(15)
#define REGQ		(16)
#define REGR		(17)
#define REGS		(18)
#define REGT		(19)
#define REGU		(20)
#define REGV		(21)
#define REGW		(22)
#define REGX		(23)
#define REGY		(24)
#define REGZ		(25)

#define REGBPM		REGB
#define REGDIVISOR	REGD
#define REGDURATION	REGL
#define REGOCTAVE	REGO

#define IMMED(x)	(x >> 8 & 0xFF)
#define IMMEDTOINT(x)	(x << 8)
#define LETTOREG(x)	(x - 'a')

#define DNORMAL		(0)
#define DDOT		(1)
#define DTRIPLET	(2)

typedef enum {
	NOTE,
	/* commands for playsong */
	MOV,
	ADD,
	SUB,
	CMP,
	JNE,
	JE,
	JL,
	JG,
	JMP,
	CFL,
	HALT,
	/* internal instructions */
	NOTHING
} instruction;

typedef struct command {
	instruction cmd;				/* note/command */
	unsigned int data;	/* note/data for extended/VM commands */
	unsigned int reg;	/* register for VM commands */
	struct command *next;			/* pointer to previous item */
	struct command *prev;			/* pointer to next item */
} command;

typedef struct {
	unsigned int count;		/* number of items (not used for playback) */
	command *first;			/* first item */
	command *last;			/* last item */
	command *current;		/* active item */
} song;

song *initsong();
int freesong(song *s);
int seeksong(song *s, int pos);
command *initcommand();
int freecommand(command *c);
int addcmd(command *c, song *s);
command *delcmd(song *s);
int rewindsong(song *s);
int nextcommand(song *s);
int prevcommand(song *s);

#endif
