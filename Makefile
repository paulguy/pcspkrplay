OBJS   = compile.o music.o pcspkrplay.o util.o output.o str.o
TARGET = pcspkrplay

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
