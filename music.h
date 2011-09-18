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

#define TIMERHZ (596591.0)

typedef enum {
	C = 0,
	CSHARP,
	D,
	DSHARP,
	E,
	F,
	FSHARP,
	G,
	GSHARP,
	A,
	ASHARP,
	B,
	/* commands for playnote */
	REST,
	/* commands for playsong */
	BPM,
	NEXTC,
	PREVB,
	NOTHING
} note;

typedef enum {
	NORMAL,
	DOT,
	TRIPLET
} duration;

typedef struct command {
	note note;				/* note/command */
	unsigned int octave;	/* octave, also used as "data" for other commands */
	unsigned int divisor;	/* note length divisor */
	duration duration;		/* TRIPLET or DOT */
	struct command *next;			/* pointer to previous item */
	struct command *prev;			/* pointer to next item */
} command;

typedef struct {
	unsigned int bpm;		/* beats per minute */
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
