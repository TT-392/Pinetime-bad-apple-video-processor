CC=gcc
DEPS = fileHandling.h struct.h
OBJ = main.o fileHandling.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

formatter: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o formatter
