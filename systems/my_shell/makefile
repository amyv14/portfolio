
CC = gcc
CFLAGS = -g -Wall -Werror
INCLUDE = -I.

# Object files
OBJS = shell.o parser.o dynamicstring.o

# Directory for binaries
BIN = ./bin

# Default target -> build the shell
all: $(BIN)/shell

# Build directory if it doesn't exist
$(BIN):
	mkdir -p $(BIN)

# Link the shell binary
$(BIN)/shell: $(BIN) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Compile shell.o
shell.o: shell.c parser.h dynamicstring.h
	$(CC) $(CFLAGS) $(INCLUDE) -c shell.c -o shell.o

# Compile parser.o
parser.o: parser.c parser.h dynamicstring.h
	$(CC) $(CFLAGS) $(INCLUDE) -c parser.c -o parser.o

# Compile dynamicstring.o
dynamicstring.o: dynamicstring.c dynamicstring.h
	$(CC) $(CFLAGS) $(INCLUDE) -c dynamicstring.c -o dynamicstring.o

# Removes the binary and object files
clean:
	rm -f $(BIN)/shell *.o



# cd systems/my_shell/
# make all 
# ./bin/shell
# echo "Hello, world"
# help
# ls -l | grep .c
