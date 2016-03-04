CC=gcc
CFLAGS=-I.
DEPS = test/header.h

TARGET = hello

NDIR = Network
_NOBJ = client.c

TDIR = test
_TOBJ = HelloWorld.c function.c

NOBJ = $(patsubst %,$(NDIR)/%,$(_NOBJ))
TOBJ = $(patsubst %,$(TDIR)/%,$(_TOBJ))

OBJ = $(TOBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


$(TARGET): $(OBJ)
	     gcc -o $@ $^ $(CFLAGS)


clean:
	$(RM) $(TARGET)
