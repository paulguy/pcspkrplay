#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BLAH (1.059463094)
double octaves[] = {262, 523, 1047, 2093, 4106, 8372, 16744};

int main(int argc, char **argv) {
	int i, j;

	printf("int notefreqs[7][12] = {\n");
	for(i = 0; i < 7; i++) {
		printf("\t{");
		for(j = 0; j < 12; j++) {
			if(j == 11) {
				printf("%i", 1193182 / (int)(octaves[6 - i] * pow(BLAH, 11 - j)));
			} else {
				printf("%i, ", 1193182 / (int)(octaves[6 - i] * pow(BLAH, 11 - j)));
			}
		}
		if(i == 6) {
			printf("}\n");
		} else {
			printf("}, \n");
		}			
	}
	printf("};\n");
}
