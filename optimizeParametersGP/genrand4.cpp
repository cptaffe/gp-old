
 // c++ libs
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

 // c libs
#include <cstdlib>
#include <clocale>
#include <cstdio>
#include <cstring>
#include <ctype>
#include <ctime>

 // local libs
#include "poprand4.h"

using namespace std;

#define ARRAYSIZE             100

void leaf(char);
void buildString(char, char);
char leafArray[20];
char ArrayLeft[60];
char ArrayRight[60];

randpop p;

int main(int argc, char *argv[]) {  /* pickup command line arguments */
	if (argc > 0) {
		time_t t;
		FILE *stream;
		int y, x;
		char expresionOne[ARRAYSIZE];
		char expresionTwo[ARRAYSIZE];
		char opArray[2];
		char fileName[20];
		srand((unsigned) time(&t)); // seed both rands
		p.gp_srand((unsigned)time(NULL)); // seed the rand
		cout << "Stand by building population of size " << atoi(argv[1]) << "\n";
		fileName[0] = 0;
		strcpy(fileName, argv[2]);
		strcat(fileName, ".pop.dat");
		stream = fopen(fileName, "wt");
		y = atoi(argv[1]); // pop size
		x = 0;

		while (x < y) {
			expresionOne[0] = 0;
			strcpy(expresionOne, " ("); // start out with open "("
			strcat(expresionOne, p.operators(opArray)); // add the op
			buildString('l', 'y'); // 
			strcat(expresionOne, ArrayLeft);
			buildString('r', 'y');
			strcat(expresionOne, ArrayRight);
			strcat(expresionOne, ")@"); // end with close )
			expresionTwo[0] = 0;
			strcpy(expresionTwo, "("); // start out with open "("
			p.operators(opArray); // add the op
			strcat(expresionTwo, opArray); // add the op
			buildString('l', 'n'); // with no F in string
			strcat(expresionTwo, ArrayLeft);
			buildString('r', 'n');
			strcat(expresionTwo, ArrayRight);
			strcat(expresionTwo, ")"); // end with close )
			fprintf(stream, "%s", expresionOne);
			fprintf(stream, "%s", expresionTwo);
			fprintf(stream, "%s", "\n");
			x++;
		}

		fclose(stream); // close file
	} else {
		p.instruc();
	}

	return 0;
}

void buildString(char side, char boolein) {
	char function[60];
	int x;
	char opArray[2];
	char numberStr[7];
	function[0] = 0; // reset string to clear it
	strcpy(function, " ("); // start out with open "("
	strcat(function, p.operators(opArray)); // add the op
	x = (p.gp_rand() % 4); // makes 0 to 3

	if (x == 0) { // if 0 then 2 leaves
		leaf(boolein); // make new random leaf and put in leafArray string
		strcat(function, leafArray); // cat the leaf into function array
		leaf(boolein); // make new random leaf and put in leafArray string
		strcat(function, leafArray); // cat the leaf into function array
	}

	if (x == 1) { // if 1 then single on left, leaf on right
		strcat(function, p.numberStr(boolein, numberStr));
		leaf(boolein); // make new random leaf and put in leafArray string
		strcat(function, leafArray); // cat the leaf into function array
	}

	if (x == 2) { // if 2 then  leaf left single on right
		leaf(boolein); // make new random leaf and put in leafArray string
		strcat(function, leafArray); // cat the leaf into function array
		strcat(function, p.numberStr(boolein, numberStr));
	}

	if (x == 3) { // if 3 then  only one leaf
		strcat(function, p.numberStr(boolein, numberStr));
		strcat(function, p.numberStr(boolein, numberStr));
	}

	strcat(function, ")");

	if (side == 'l')
	{ strcpy(ArrayLeft, function); }
	else
	{ strcpy(ArrayRight, function); }
}

void leaf(char boolein) {
	char opArray[2];
	char numberStr[7];
	leafArray[0] = 0; // clear the array

	if (p.gp_rand() % 3 == 0 && boolein == 'y') // y = yes to F( no to k
	{ strcpy(leafArray, " F("); }
	else
	{ strcpy(leafArray, " ("); }

	strcat(leafArray, p.operators(opArray));
	strcat(leafArray, p.numberStr(boolein, numberStr));
	strcat(leafArray, p.numberStr(boolein, numberStr));
	strcat(leafArray, ")");
}
