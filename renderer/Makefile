CC=gcc
CFLAGS= -lncursesw
DEPS = fileHandling.h struct.h pinencurses.h
OBJ = main.o fileHandling.o pinencurses.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

renderer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o renderer
