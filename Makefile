all: pcspkrplay

pcspkrplay: compile.o music.o pcspkrplay.o util.o output.o str.o
	$(CC) $(LDFLAGS) -o $@ $^

compile.o: compile.c
	$(CC) $(CFLAGS) -c -o $@ $<

music.o: music.c
	$(CC) $(CFLAGS) -c -o $@ $<

pcspkrplay.o: pcspkrplay.c
	$(CC) $(CFLAGS) -c -o $@ $<

util.o: util.c
	$(CC) $(CFLAGS) -c -o $@ $<

output.o: output.c
	$(CC) $(CFLAGS) -c -o $@ $<

str.o: str.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f compile.o music.o pcspkrplay.o util.o output.o str.o pcspkrplay

.PHONY: clean
