rm *.o pcspkrplay
gcc -ggdb -c -o music.o music.c
gcc -ggdb -c -o util.o util.c
gcc -ggdb -c -o compile.o compile.c
gcc -ggdb -o pcspkrplay music.o util.o compile.o pcspkrplay.c
