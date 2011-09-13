#include "music.h"

#define DEFAULT_OCTAVE (3)
#define DEFAULT_DIVISOR (32)
#define DEFAULT_BPM (50)

#define DECOMPILE_BUFFER (1024)

song *compilesong(char *songstr, int length);
