# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -g

# Target executable name
TARGET = sfl_program

# List of object files
OBJS = dump.o mem_alloc.o mem_free.o read_write.o sfl.o

# Default target, which builds the program
all: $(TARGET)

# Rule to compile dump.c into dump.o
dump.o: dump.c dump.h
	$(CC) $(CFLAGS) -c dump.c

# Rule to compile mem_alloc.c into mem_alloc.o
mem_alloc.o: mem_alloc.c mem_alloc.h
	$(CC) $(CFLAGS) -c mem_alloc.c

# Rule to compile mem_free.c into mem_free.o
mem_free.o: mem_free.c mem_free.h
	$(CC) $(CFLAGS) -c mem_free.c

# Rule to compile read_write.c into read_write.o
read_write.o: read_write.c read_write.h
	$(CC) $(CFLAGS) -c read_write.c

# Rule to compile sfl.c into sfl.o
sfl.o: sfl.c
	$(CC) $(CFLAGS) -c sfl.c

# Rule to link object files and create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)
