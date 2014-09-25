// Phil H Williams Jan-15-20013

// c++ libs
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

// c libs
#include <math.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

// local libs
#include "poprand4.h"

using namespace std;

#define M1 259200L
#define IA1 7141L
#define IC1 54773L
#define RM1 (1.0/M1)
#define M2 134456L
#define IA2 8121L
#define IC2 28411L
#define RM2 (1.0/M2)
#define M3 243000L
#define IA3 4561L
#define IC3 51349L

char *randpop::operators(char *ptr) {
	char operators[] = {'*', '-', '+', '/'};
	ptr[0] = operators[rand() % 4];
	ptr[1] = 0;
	return ptr;
}

char *randpop::numberStr(char bol, char *numberStr) {
	float number;
	int dec, sign, ndig = 4;
	int i, x, k = 0;
	char zeros[4];
	char *floatNum;
	char floats[5];
	int y;
	int negOrpos;
	x = (rand() % 6); // makes 0 to 5

	if (x < 2) {
		if (bol == 'n') {
			k = (rand() % 4); // makes 0 to 3
		}

		if (k < 2 && bol == 'n') {
			negOrpos = rand() % 2; // two choices  0 or 1

			if (negOrpos == 0) { strcpy(numberStr, " k"); }
			else { strcpy(numberStr, " -k"); }
		} else {
			negOrpos = rand() % 2; // two choices  0 or 1

			if (negOrpos == 0) { strcpy(numberStr, " X"); }
			else { strcpy(numberStr, " -X"); }
		}
	} else { // then a number
		number = rand() % 10000;
		number = number / 10000;

		// float 0 ?
		if (number == 0) {
			number = .0001;
		}

		floatNum = fcvt(number, ndig, &dec, &sign);
		strcpy(floats, floatNum);
		zeros[0] = 0; // clear string
		i = 0;

		while (dec < 0) { // add zeros to front of number if needed
			zeros[i] = '0';
			i++;
			dec++;
		}

		zeros[i] = 0; // end with nul
		numberStr[0] = 0; // set string to nul
		negOrpos = rand() % 2; // two choices  0 or 1

		if (negOrpos == 0) {
			strcpy(numberStr, " -."); // put -decimal point in array
		} else {
			strcpy(numberStr, " ."); // put +decimal point in array
		}

		strcat(numberStr, zeros); // then zeros
		y = strlen(floats) - 1;

		while (floats[y] == '0') { // remove zeros from end of number
			floats[y] = '\0'; // replace zero with null
			y--;
		}

		strcat(numberStr, floats); // and cat the number
	}

	return numberStr;
}

void randpop::instruc() {
	printf("\n");        /* the start up screen */
	printf("          rangen@ [n]\n\n");
	printf("          n    random genarate n formulas\n");
}
