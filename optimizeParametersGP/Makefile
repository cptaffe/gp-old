SRC=gp_onDaughterCell-V10.cc gp_list.cpp randInsertTransposon.cpp
BIN=gp_onDaughterCell-V10
CFLAGS=-Wall -g 
CC=clang++

all: gp_onDaughterCell-V10

gp_onDaughterCell-V10:
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm $(BIN)

run:
	./$(BIN)