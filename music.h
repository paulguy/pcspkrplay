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
int playnote(int fd, command *cmd);
int playsong(int fd, song *song, void (*status)(int cur, int max));

#endif
