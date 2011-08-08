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
	NEXTC,
	PREVB,
	REST,
	NOTHING
} note;

typedef enum {
	NORMAL,
	DOT,
	TRIPLET
} duration;

typedef struct {
	note note;
	unsigned int octave;
	unsigned int divisor;
	duration duration;
	void *next;
} command;

typedef struct {
	unsigned int bpm;
	unsigned int count;
	command *first;
} song;

void playnote(int fd, command *cmd);
void playsong(int fd, song *song, void (*status)(int cur, int max));
