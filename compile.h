#include "music.h"
#include "str.h"

#define DEFAULT_OCTAVE (3)
#define DEFAULT_DIVISOR (32)
#define DEFAULT_BPM (50)

song *compilesong(char *songstr, int length);
str *decompilesong(song *s);
