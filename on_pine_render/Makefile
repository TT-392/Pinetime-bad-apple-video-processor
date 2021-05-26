CC=gcc
CFLAGS= -lncursesw
DEPS = pinencurses.h bad_apple.h
OBJ = main.o pinencurses.o bad_apple.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pine: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o pine
