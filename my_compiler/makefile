CC=g++
CFLAGS=-g -std=c++2a
BIN=./bin

cpp: ./cpp/*.cpp | $(BIN)
	$(CC) $(CFLAGS) ./cpp/*.cpp -o $(BIN)/compiler

compileall: cpp
	$(BIN)/compiler ./scripts/1.basic
	$(BIN)/compiler ./scripts/2.basic
	$(BIN)/compiler ./scripts/3.basic
	$(BIN)/compiler ./scripts/4.basic
	$(BIN)/compiler ./scripts/5.basic
	$(BIN)/compiler ./scripts/6.basic
	$(BIN)/compiler ./scripts/7.basic
	$(BIN)/compiler ./scripts/8.basic
	$(BIN)/compiler ./scripts/9.basic

runall: compileall
	gcc ./scripts/1.c -o $(BIN)/1basic && $(BIN)/1basic
	gcc ./scripts/2.c -o $(BIN)/2basic && $(BIN)/2basic
	gcc ./scripts/3.c -o $(BIN)/3basic && $(BIN)/3basic
	gcc ./scripts/4.c -o $(BIN)/4basic && $(BIN)/4basic
	gcc ./scripts/5.c -o $(BIN)/5basic && $(BIN)/5basic
	gcc ./scripts/6.c -o $(BIN)/6basic && $(BIN)/6basic
	gcc ./scripts/7.c -o $(BIN)/7basic && $(BIN)/7basic
	gcc ./scripts/8.c -o $(BIN)/8basic && $(BIN)/8basic
	gcc ./scripts/9.c -o $(BIN)/9basic && $(BIN)/9basic

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -f $(BIN)/compiler $(BIN)/1basic $(BIN)/2basic $(BIN)/3basic \
	      $(BIN)/4basic $(BIN)/5basic $(BIN)/6basic $(BIN)/7basic \
	      $(BIN)/8basic $(BIN)/9basic \
	      ./scripts/1.basic.c ./scripts/2.basic.c ./scripts/3.basic.c \
	      ./scripts/4.basic.c ./scripts/5.basic.c ./scripts/6.basic.c \
	      ./scripts/7.basic.c ./scripts/8.basic.c ./scripts/9.basic.c
