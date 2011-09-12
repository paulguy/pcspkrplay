all: pcspkrplay

pcspkrplay: compile.o music.o pcspkrplay.o util.o
	$(CC) $(LDFLAGS) -o $@ $^

compile.o: compile.c
	$(CC) $(CFLAGS) -c -o $@ $<

music.o: music.c
	$(CC) $(CFLAGS) -c -o $@ $<

pcspkrplay.o: pcspkrplay.c
	$(CC) $(CFLAGS) -c -o $@ $<

util.o: util.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f compile.o music.o pcspkrplay.o util.o pcspkrplay

.PHONY: clean
