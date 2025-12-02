
##################### USAGE ######################
# To build the compiler, run:
# make

# To run it on a provided example, example.basic:
# make run

# To clean binaries and generated .c files:
# make clean
##################################################

CXX = g++
CXXFLAGS = -std=c++2a -O2 -Wall -Wextra

SRC = ./cpp/compiler.cpp ./cpp/lexer.cpp
OUT = compile

BASIC = ./cpp/example.basic

.PHONY: all run clean

# Build the compiler
all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

# Run the compiler on example.basic
run: $(OUT)
	./$(OUT) $(BASIC)

clean:
	rm -f $(OUT)
	rm -f ./cpp/*.c
