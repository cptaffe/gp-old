SRC=gp_onDaughterCell-V10.cc gp_list.cpp randInsertTransposon.cpp
BIN=gp_onDaughterCell-V10
CFLAGS=-Wall -g 
CC=clang++
DATA_FILE=X^5+2X^4-3X^3+2X^2+4X+5.dat
PARAMS=458 488 669 0.12 -0.02 14 8 1 10 9 5770 2 3 4 2 16 11 1.1 416 1 24 1057 compute-1-9  $(DATA_FILE)

all: gp_onDaughterCell-V10

gp_onDaughterCell-V10:
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm $(BIN)

run:
	./$(BIN) $(PARAMS)