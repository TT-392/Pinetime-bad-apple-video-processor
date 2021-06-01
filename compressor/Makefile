CC=gcc
CFLAGS= -fsanitize=address
DEPS = fileHandling.h struct.h
OBJ = main.o fileHandling.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compressor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o compressor
