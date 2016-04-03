CC=gcc
CFLAGS=
DEPS = queue_functions.h
OBJ = main.c queue_functions.c test_functions.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
