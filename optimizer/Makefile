CC=gcc
CFLAGS=-pthread -fsanitize=address
DEPS = fileHandling.h optimization.h utils.h struct.h
OBJ = main.o fileHandling.o utils.o optimization.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

optimizer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o optimizer
