 // Philip Williams  2-2-99

#ifndef _poprand4_H_
#define _poprand4_H_

 // #define MAX_SIZE 100

class randpop {
public:
	void instruc(); // print the instructions
	//void operators(); // add random ops
	//	   char *operators(); // add random ops
	//	   int operators(char *ptr); // add random ops
	char *operators(char *ptr); // add random ops
	char *numberStr(char bol, char *numberStr); // add random numbers
	//	   char *leafStr; // pointer to leaf string
	//	   char numberStr[6]; // the array of char
	//	   char ops[2]; // the array of char
	// ~gp_rand();
	unsigned long gp_rand();
	void gp_srand(unsigned long seed);
	// private:
};
#endif
