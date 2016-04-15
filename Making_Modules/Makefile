# Which compiler to use
CC = clang-3.5

# Compiler flags go here.
CFLAGS += -std=gnu11 -g -Wall -Wextra

# Linker flags go here.
LDFLAGS += -lcomedi -lm -lpthread

# list of sources

ELEVSRC = panel_lights.c basic_light_functions.c basic_network_functions.c basic_elevator_functions.c elev.c io.c system_behaviour.c basic_queue_functions.c main.c

# program executable file name.
TARGET = elevator

# top-level rule, to compile everything.
all: $(TARGET)

# Define all object files.
ELEVOBJ = $(ELEVSRC:.c=.o)

# rule to link the program
$(TARGET): $(ELEVOBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile: create object files from C source files.
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# rule for cleaning re-compilable files.
clean:
	rm -f $(TARGET) $(ELEVOBJ)

rebuild:	clean all

.PHONY: all rebuild clean
