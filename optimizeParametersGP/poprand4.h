// Philip Williams  2-2-99

#ifndef _poprand4_H_
#define _poprand4_H_

// #define MAX_SIZE 100

class randpop {
public:
	void instruc(); // print the instructions
	char *operators(char *ptr); // add random ops
	char *numberStr(char bol, char *numberStr); // add random numbers
	unsigned long gp_rand();
	void gp_srand(unsigned long seed);
};
#endif
