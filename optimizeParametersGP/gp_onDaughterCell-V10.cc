// gp_onDautherCell

// c libraries
#include <cstdlib> // c standard library (<stdlib.h>)
#include <cstdio>
#include <cstring> // c strings
#include <cctype> // c types
#include <ctime> // time, seed for random
#include <cmath>
#include <clocale>

// c++ libraries
#include <iostream>
#include <fstream>
#include <iomanip>

// local includes
#include "tstack2.h" // the stacks for ops & numbers
#include "loadxy.h" // struct for curve fit data
#include "gp_list.h" // list struct for population
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

#define stack_size 400
#define Max_Str_size 20000
#define ARRAYSIZE 100

void gp_srand(unsigned long long seed);
unsigned long long gp_rand(void);
long double recalcF(char expresion[], int &i, int x, int &divBy0, int &overSize, char *argv[]);
int calcfit(char expresion[], char *argv[]);
void switchCase(char expresion[], int &i, int x, long double k, int &divBy0, int &overSize, char *argv[]);
void getIndex(char stopChar, char *Astring, int &Aplace, int &Aend);
void getIndexLeafOnly(char stopChar, char *Astring, int &Aplace, int &Aend);
void first_run(char *argv[]);
void error(char *msg, char *argv[]);
void loadxy(char *file);
void makeExoGene(char *DynArrayTranspIn, int flip, char *argv[]);
void insertExoGene(char Astring[], char Bstring[], int flip, char *argv[]);
void kill(char *argv[]);
int pickForInsertion(char *argv[]); // 
int pickForGenejump(char *argv[]); // 
int pickForSelfCross(int, char *argv[]); // 
int pickForIntrons(char *argv[]); // 
int pickForSpliceOutGene(int, int, char *argv[]); // 
void writeOut(char *argv[]);
void geneJump(char *, char *argv[]);
void introns(char *, char *argv[]); // adds introns to string
void spliceOutGene(char *, int, int, char *argv[]); // remove gene
void selfCross(char Astring[], int, char *argv[]);
void buildIntron(char *arrayIn);
void exitOut(char *argv[], int fit);
void leaf(char);
void buildString(char, char);

randpop p;

char leafArray[20];
char ArrayLeft[60];
char ArrayRight[60];

// global variables an absolute necessity for random number system....
unsigned long long RandIx1, RandIx2, RandIx3;
double TrandArray[98];

// pop is data type list
gp_list pop;

// data type array for X & Y data from question file
XYclass arrayXY;

// the operands (numbers) stack of default size
Tstack2<long double> num(stack_size);

// the operators stack + - / *
Tstack2<char> op(stack_size);

struct ptr_struct {
	char *charPtr;
};

int StackSize = 0;
int highFitness = 0;
int size = 0;
int newSize = 0;
int fitSize = 0; // size of question file data
int g = 0; // generation count
char c[] = {27, 91, 72, 27, 91, 50, 74};
int scaleFitness = 1;

/* pickup command line arguments */
int main(int argc, char *argv[]) {
	int j;

	// error printing argv, exit
	if (argc < 25) {
		for (j = 0; j <= argc; j++) {
			printf("argv[%d]: %s\n", j, argv[j]);
		}

		exit(9);
	}

	char cmdStr[70];
	int gDiff, exitVal;
	int deltaPunkInk;
	int index = 0;
	int *gDiffArray;
	int minPopSize, maxPopSize, exitMultValue, founderMinFitness, intronProb,
		genejumpProb, ratioOfXoverMainadf;
	int pickForInsertionProb, pickForSelfCrossProb,
		pickForSpliceOutGeneProb;
	int minGeneSizeOut, maxGeneSizeOut;
	float multByNewSize;
	char *endptr; // for strtod
	maxPopSize = atoi(argv[2]);
	intronProb = atoi(argv[6]);
	genejumpProb = atoi(argv[7]);
	founderMinFitness = atoi(argv[8]);
	ratioOfXoverMainadf = atoi(argv[9]);
	exitMultValue = atoi(argv[10]);
	minPopSize = atoi(argv[11]);
	scaleFitness = atoi(argv[13]);
	pickForInsertionProb = atoi(argv[14]);
	pickForSelfCrossProb = atoi(argv[15]);
	gDiff = atoi(argv[16]);
	deltaPunkInk = atoi(argv[17]);
	multByNewSize = strtod(argv[18], &endptr);
	pickForSpliceOutGeneProb = atoi(argv[20]);
	minGeneSizeOut = atoi(argv[21]);
	maxGeneSizeOut = atoi(argv[22]);

	// shim to avoid an infinite loop.
	if (maxPopSize < minPopSize) {
		maxPopSize = minPopSize;
	}

	if (!(gDiffArray = new int[gDiff])) {
		cout << "ERROR\n";
		exit(9);
	}

	loadxy(argv[24]); // load the x & y from file to arraya
	gp_srand((unsigned)time(NULL)); // seed the rand
	cout.setf(ios::right, ios::adjustfield);
	// build command string
	cmdStr[0] = 0;
	strcpy(cmdStr, "./seedPopRand.pl ");
	strcat(cmdStr, argv[23]);
	strcat(cmdStr, ".");
	strcat(cmdStr, "seedSource.pop.log ");
	strcat(cmdStr, argv[3]); // randpopsize
	strcat(cmdStr, " ");
	strcat(cmdStr, argv[23]); // stem name for rand pop file
	j = 0;

	while (highFitness < founderMinFitness &&
			size < minPopSize) {
		while (pop.length() > maxPopSize) { kill(argv); }

		exitVal = system(cmdStr); // run command string in loop

		if (exitVal != 0) {
			fprintf(stderr, "Error\n%s\n", cmdStr);
			exit(9);
		}

		cout << "\nGeneration    Add to pop    Highfitness out of " << fitSize << "\n";
		first_run(argv); // first run puts data in linked list

		if (j++ > atoi(argv[1])) {
			cout << "exit 0" << "\n";
			exit(0);
		}
	}

	while (g < atoi(argv[1])) {
		g++;

		while (pop.length() > maxPopSize && g >= 2) { kill(argv); }

		newSize = pop.length();

		if (newSize < 3) {
			cout << "   Low population size only " << newSize << "\n";
			exitOut(argv, highFitness);
			return 0;
		}

		size = 0;

		for (j = 0; j <= newSize * multByNewSize; j++) {
			if (gp_rand() % pickForInsertionProb == 0) {
				pickForInsertion(argv);
			}

			if (gp_rand() % genejumpProb == 0) {
				pickForGenejump(argv);
			}

			if (gp_rand() % pickForSelfCrossProb == 0) {
				pickForSelfCross(ratioOfXoverMainadf, argv);
			}

			if (gp_rand() % intronProb == 0) {
				pickForIntrons(argv);
			}

			if (gp_rand() % pickForSpliceOutGeneProb == 0) {
				pickForSpliceOutGene(minGeneSizeOut, maxGeneSizeOut, argv);
			}
		}

		if ((highFitness * exitMultValue) <= g) {
			printf("\nLow fitness of %d for generation %d\n", highFitness, g);
			printf("          %d * %d = %d\n", highFitness, exitMultValue,
				   highFitness * exitMultValue);
			writeOut(argv); // dump data to file
			exitOut(argv, highFitness);
			return 0;
		}

		if (index < gDiff) {
			gDiffArray[index] = highFitness;
			index++;
		} else {
			if (highFitness - gDiffArray[0] < deltaPunkInk) {
				printf("\nLow level of punctuated equilibrium for generation %d\n%d = %d - %d\n",
					   g, highFitness - gDiffArray[0], highFitness, gDiffArray[0]);
				writeOut(argv); // dump data to file
				exitOut(argv, highFitness);
				delete gDiffArray;
				return 0;
			}

			for (j = 0; j < gDiff; j++) {
				gDiffArray[j] = gDiffArray[j + 1];
			}

			gDiffArray[gDiff - 1] = highFitness;
		}
	}

	writeOut(argv); // dump data to file
	exitOut(argv, highFitness);
	cout << "\nExit program pw\n";
	delete gDiffArray;
	return 0;
}


void selfCross(char Astring[], int ratioOfXoverMainadf, char *argv[]) {
	int Aplace = 0;
	int Bplace = 0; // postions holders
	int Aend = 0;
	int Bend = 0; // postions holders
	int x, y;
	int index;
	char stopChar;
	char same = 't'; // track if same substrings
	char firstXover[Max_Str_size];
	char secondXover[Max_Str_size];
	stopChar = '@'; // start at 0 and end at '@'or

	if (gp_rand() % ratioOfXoverMainadf == 0) {
		stopChar = 0; // start at @ to end of string
	}

	index = 0; // use index as a counter for now

	while (same == 't') {
		if (index > 5) { return; } // only the same so give up and return

		index++;
		getIndex(stopChar, Astring, Aplace, Aend);
		getIndex(stopChar, Astring, Bplace, Bend);

		if ((Aplace - Aend) == (Bplace - Bend)) { // the strings are same length
			x = Aplace; // test to see if the are the same data too
			y = Bplace;

			if (x == Aend && y == Bend) { same = 't'; } // same substrings so go again
			else { same = 'f'; }
		}

		if (Aplace == Aend || Bplace == Bend || Aplace == 0 || Bplace == 0) { same = 't'; }
		else { same = 'f'; }
	}

	x = 0; // x is the positional index for the strArray string

	while (x < Aplace) { // load the Ahead into strArray
		firstXover[x] = Astring[x];
		x++;
	}

	index = Bplace;

	while (index < Bend) { // load the Bmiddle
		firstXover[x] = Astring[index];
		index++;
		x++;
	}

	index = Aend;

	while (Astring[index] != 0) {
		firstXover[x] = Astring[index];
		x++; // load the Atail into strArray
		index++;
	}

	firstXover[x] = 0; // end with null
	calcfit(firstXover, argv);
	x = 0; // x is the positional index for the string

	while (x < Bplace) { // load the Bhead into strArray
		secondXover[x] = Astring[x];
		x++;
	}

	if (secondXover[x - 1] != ')') {
		secondXover[x] = ' ';
		x++;
	}

	index = Aplace;

	while (index < Aend) { // load the Amiddle
		secondXover[x] = Astring[index];
		index++;
		x++;
	}

	index = Bend;

	while (Astring[index] != 0) {
		secondXover[x] = Astring[index];
		x++; // load the tail
		index++;
	}

	secondXover[x] = 0; // end with null
	calcfit(secondXover, argv);
	return;
}

// put leaf in dynamic array
// return array containing intron
void buildIntron(char *arrayIn) {
	int zero_or_one;
	const char multByOne[] = "\(* (/";
	const char subtractZero[] = "\(+ (-";
	char leafStr[8];
	leafStr[0] = 0;
	strcat(leafStr, arrayIn);
	arrayIn[0] = 0;
	zero_or_one = gp_rand() % 2; // two choices  0 or 1

	if (zero_or_one == 0) { strcat(arrayIn, multByOne); }
	else { strcat(arrayIn, subtractZero); }

	strcat(arrayIn, leafStr);
	strcat(arrayIn, leafStr);
	return;
}

void getIndexLeafOnly(char stopChar, char *Astring, int &Aplace, int &Aend) {
	int randA;
	int leafCountA = 0;
	int count;
	int Astart;
	char *ptr; // pointer to the start of the string

	if (stopChar == '@') {
		Astart = 0; // cross the main function
	} else { // else start at '@' and end at nul
		ptr = strchr(Astring, '@'); // user strchr to find the @
		Astart = ptr - Astring; // start is position of @ +1
	}

	Aplace = Astart; // here Aplace is an index

	while (Astring[Aplace] != stopChar) { // till at stopChar
		if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' ||
				Astring[Aplace] == 'k') {
			leafCountA++; // count number of leaves in string
		}

		Aplace++; // move and test the next char
	}

	randA = (gp_rand() % leafCountA + 1);
	Aplace = Astart; // here Aplace becomes the position
	count = 0;

	while (count < randA) {
		Aplace++;

		if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' ||
				Astring[Aplace] == 'k') {
			count++; // count the leaves till = randA
		}
	}

	// test for sign on leaf
	if (Astring[Aplace - 1] == '-') {
		Aplace--; // if negative backup one
	}

	Aend = Aplace;

	while (Astring[Aend] != ')' && Astring[Aend] != ' ' && Astring[Aend] != '(' &&
			Astring[Aend] != stopChar && Astring[Aend] != 0) {
		Aend++; // Aend is now the position for end of leaf
	}

	Aplace--; // and backup one to pickup space on front end
	return;
}

void insertExoGene(char Astring[], char transposIn[], int flip, char *argv[]) {
	// Astring is the original genome, transposon for insertion
	int Aplace = 0;
	int Aend = 0;
	int x, y;
	char stopChar;
	char leafArray[Max_Str_size];
	char tmpArray[Max_Str_size];
	char charArray[Max_Str_size];
	// Save the leaf here to make into an intron
	const char paren[] = ")";

	if (flip == 0) {
		stopChar = '@'; // start at 0 and end at '@'or
	} else {
		stopChar = '\0'; // start at @ to end of string
	}

	getIndexLeafOnly(stopChar, Astring, Aplace, Aend);
	// For the Astring find some leaf node to add to
	x = 0;
	y = Aplace;

	while (y < Aend) {
		leafArray[x] = Astring[y];
		x++;
		y++;
	}

	leafArray[x] = 0;
	buildIntron(leafArray);
	x = y = 0;

	while (y < Aplace) {
		tmpArray[x] = Astring[y];
		x++;
		y++;
	}

	tmpArray[x] = ' ';
	x++;
	tmpArray[x] = 0;
	// Copy the intron
	strcat(tmpArray, leafArray);
	strcat(tmpArray, paren);
	// Copy the transposon
	strcat(tmpArray, transposIn);
	strcat(tmpArray, paren);
	// Copy the tail
	x = 0;
	y = Aend;

	while (Astring[y] != 0) {
		charArray[x] = Astring[y];
		x++;
		y++;
	}

	charArray[x] = 0;
	// Copy the tail
	strcat(tmpArray, charArray);
	transposIn[0] = 0; // clear array
	strcpy(transposIn, tmpArray);
	return;
}

// insert exogenous gene
void makeExoGene(char *DynArrayTranspIn, int flip, char *argv[]) {
	char expresion[ARRAYSIZE];
	expresion[0] = 0;

	if (flip == 0) {
		// build main random exogenous gene
		buildString('r', 'y');
		strcat(expresion, ArrayRight);
	} else {
		buildString('l', 'n'); // with no F in string
		strcat(expresion, ArrayLeft);
	}

	strcpy(DynArrayTranspIn, expresion);
	return;
}

// Note: pickForSpliceOutGene, pickForIntrons, pickForSelfCross,
// pickForGenejump,
// share a large amount of code. Actually, everything
// except for the last function before the delete is exactly
// the same.

int pickForSpliceOutGene(int minGeneSizeOut, int maxGeneSizeOut, char *argv[]) {
	char *strptr; // char pointer for strings
	char *DynArrayPick; // pointer for dynamic arrays
	unsigned long long spin_1;
	unsigned long long maxSpiner;
	char strArray[Max_Str_size];
	maxSpiner = pop.getMaxSpin(); // spiner loaded here
	spin_1 = (gp_rand() % (maxSpiner - 1)) + 1;
	strptr = pop.SpinString(spin_1);
	// copy the string so the pointer can be removed
	strcpy(strArray, strptr);
	strptr = 0; // null the pointer for the kill

	if (!(DynArrayPick = new char[strlen(strArray) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	strcpy(DynArrayPick, strArray);
	// SPECIFIC PICKFOR* CODE BEGIN
	spliceOutGene(DynArrayPick, minGeneSizeOut, maxGeneSizeOut, argv);
	// SPECIFIC PICKFOR* CODE END
	delete DynArrayPick;
	return 0;
}

int pickForIntrons(char *argv[]) {
	char *strptr; // char pointer for strings
	char *DynArrayPick; // pointer for dynamic arrays
	unsigned long long spin_1;
	unsigned long long maxSpiner;
	char strArray[Max_Str_size];
	maxSpiner = pop.getMaxSpin(); // spiner loaded here
	spin_1 = (gp_rand() % (maxSpiner - 1)) + 1;
	strptr = pop.SpinString(spin_1);
	// copy the string so the pointer can be removed
	strcpy(strArray, strptr);
	strptr = 0; // null the pointer for the kill

	if (!(DynArrayPick = new char[strlen(strArray) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	strcpy(DynArrayPick, strArray);
	// SPECIFIC PICKFOR* CODE BEGIN
	introns(DynArrayPick, argv);
	// SPECIFIC PICKFOR* CODE END
	delete DynArrayPick;
	return 0;
}

int pickForSelfCross(int ratioOfXoverMainadf, char *argv[]) {
	char *strptr; // char pointer for strings
	char *DynArrayPick; // pointer for dynamic arrays
	unsigned long long spin_1;
	unsigned long long maxSpiner;
	char strArray[Max_Str_size];
	maxSpiner = pop.getMaxSpin(); // spiner loaded here
	spin_1 = (gp_rand() % (maxSpiner - 1)) + 1;
	strptr = pop.SpinString(spin_1);
	// copy the string so the pointer can be removed
	strcpy(strArray, strptr);
	strptr = 0; // null the pointer for the kill

	if (!(DynArrayPick = new char[strlen(strArray) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	strcpy(DynArrayPick, strArray);
	// SPECIFIC PICKFOR* CODE BEGIN
	selfCross(DynArrayPick, ratioOfXoverMainadf, argv);
	// SPECIFIC PICKFOR* CODE END
	delete DynArrayPick;
	return 0;
}

int pickForGenejump(char *argv[]) {
	char *strptr; // char pointer for strings
	char *DynArrayPick; // pointer for dynamic arrays
	unsigned long long spin_1;
	unsigned long long maxSpiner;
	char strArray[Max_Str_size];
	maxSpiner = pop.getMaxSpin(); // spiner loaded here
	spin_1 = (gp_rand() % (maxSpiner - 1)) + 1;
	strptr = pop.SpinString(spin_1);
	// copy the string so the pointer can be removed
	strcpy(strArray, strptr);
	strptr = 0; // null the pointer for the kill

	if (!(DynArrayPick = new char[strlen(strArray) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	strcpy(DynArrayPick, strArray);
	// SPECIFIC PICKFOR* CODE BEGIN
	geneJump(DynArrayPick, argv);
	// SPECIFIC PICKFOR* CODE END
	delete DynArrayPick;
	return 0;
}

int pickForInsertion(char *argv[]) {
	char *strptr; // char pointer for strings
	char *DynArrayPick; // pointer for dynamic arrays
	char *DynArrayTransp; // pointer for dynamic arrays
	unsigned long long spin_1;
	unsigned long long maxSpiner;
	char strArray[Max_Str_size];
	char *pc;
	int flip = 0;
	maxSpiner = pop.getMaxSpin(); // spiner loaded here
	spin_1 = (gp_rand() % (maxSpiner - 1)) + 1;
	strptr = pop.SpinString(spin_1);
	// copy the string so the pointer can be removed
	strcpy(strArray, strptr);
	strptr = 0;

	if (!(DynArrayPick = new char[strlen(strArray) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	strcpy(DynArrayPick, strArray);
	// SPECIFIC PICKFOR* CODE BEGIN
	pc = strchr(DynArrayPick, 'F');

	if (pc) {
		flip = gp_rand() % 2; // two choices  0 or 1
	} else {
		flip = 0;
	}

	if (!(DynArrayTransp = new char[Max_Str_size])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	makeExoGene(DynArrayTransp, flip, argv);
	insertExoGene(DynArrayPick, DynArrayTransp, flip, argv);
	calcfit(DynArrayTransp, argv);
	delete DynArrayTransp; // recoop the mem
	// SPECIFIC PICKFOR* CODE END
	delete DynArrayPick; // recoop the mem
	return 0;
}

void loadxy(char *questionData) {
	FILE *XYfile;
	int i;
	char temp[100];

	if ((XYfile = fopen(questionData, "r")) == NULL) {
		cout << " File question.dat could not be opened";
		exit(9);
	} else {
		while (!feof(XYfile)) {
			fgets(temp, 99, XYfile);
			fitSize++;
		}

		fitSize--;
		rewind(XYfile); // resart the file at the begining

		for (i = 0; i < fitSize; i++) {
			fscanf(XYfile, "%s", (char *) &temp); // scan a number X
			arrayXY.loadX(i, atoi(temp));
			fscanf(XYfile, "%s", (char *) &temp); // scan a number Y
			arrayXY.loadY(i, atol(temp));
		}

		fclose(XYfile);
	}
}

void first_run(char *argv[]) {
	FILE *file_in;
	char line[8500]; // line holds the exspresion from file
	char char1;
	int x; // for char
	char cmdStr[60];
	cmdStr[0] = 0;
	strcpy(cmdStr, argv[23]); // argv[15] node name stem
	strcat(cmdStr, ".pop.dat");

	if ((file_in = fopen(cmdStr, "r")) == NULL) {
		cout << " File " << cmdStr << " could not be opened\n";
		exit(9);
	} else {
		// do until end of file
		do {
			fscanf(file_in, "%c", &char1); // scan a char
			x = 0;

			// loop to load the array
			while (char1 != '\n') {
				line[x] = char1;
				x++;
				fscanf(file_in, "%c", &char1);
			}

			line[x] = '\0'; // add null to end the array

			if (strlen(line) != 0)
			{ calcfit(line, argv); }
		} while (!feof(file_in));

		fclose(file_in);
	}
}

int calcfit(char expresion[], char *argv[]) {
	char *endptr; // SHIM for strtod
	double tite;
	int x, i, minKeepFitness;
	int fit, maxChroLen;
	int testadd = 0;
	int divBy0 = 0;
	long double temp1;
	char tempchar;
	int minKeepFit = 0;
	int overSize = 0;
	float threshold, thresholdDiviser;
	maxChroLen = atoi(argv[19]);
	threshold = strtod(argv[4], &endptr);
	thresholdDiviser = strtod(argv[5], &endptr);
	minKeepFitness = atoi(argv[12]);

	if (strlen(expresion) > maxChroLen) {
		return 1;
	}

	fit = 0;

	if (g == 0) {
		tite = strtod(argv[4], &endptr);
		minKeepFit = 0;
	} else {
		tite = (threshold + thresholdDiviser);

		if (minKeepFitness < 0) {
			minKeepFit = highFitness - 1;
		} else {
			minKeepFit = minKeepFitness;
		}
	}

	// for loop to change x and y and test them
	for (x = 0; x < fitSize; x++) {
		i = 0;

		while (expresion[i] != '@') {
			// 0.0 for k
			switchCase(expresion, i, x, 0.0, divBy0, overSize, argv);

			if (divBy0 == 2 || overSize == 2) {
				while (num.isEmpty() != 1) { num.pop(temp1); }

				while (op.isEmpty() != 1) { op.pop(tempchar); }

				return 1; // flush stacks and bail
			}
		}

		if (divBy0 == 1) {
			fit--;
		}

		if (overSize == 1) {
			fit--;
		}

		num.pop(temp1); // pop the answer

		if (num.isEmpty() != 1 || op.isEmpty() != 1) {
			error((char *) " Main Stacks not empty", argv);
		}

		// if tite is large then the delta can be large
		if (fabs(arrayXY.getY(x) - temp1) <= tite) { fit++; }
	}

	if (fit >= minKeepFit) {
		if (fit <= scaleFitness) {
			fit = 1;
		}

		testadd = pop.add(expresion, fit);

		while (testadd == 1) {
			kill(argv);
			testadd = pop.add(expresion, fit);
		}

		// set the global fitness
		if (fit > highFitness) {
			highFitness = fit;
		}

		size++;

		// if found approximate solution
		if (fit == fitSize) {
			cout << "*****  Found Approxamate Solution  *****" << "\n\n";
			cout << ' ' << expresion << "\n";
			writeOut(argv);
			exitOut(argv, fit);
			exit(1);
		}
	}

	return 0;
}

void geneJump(char *strIn, char *argv[]) {
	char *strOut;
	int x, y;
	strOut = strchr(strIn, 'F'); // test for F in string

	if (strOut) {
		return; // if has F then return
	}

	strOut = strchr(strIn, 'k'); // test for k in string

	if (strOut) {
		return; // if has k then return
	}

	// made and tested array for swap
	if (!(strOut = new char[strlen(strIn) + 1])) {
		cout << "ERROR out of RAM\n";
		exit(9);
	}

	y = x = 0;

	// set index x to @ in string
	while (strIn[x] != '@') {
		x++;
	}

	x++; // move past @

	// copy from @ to end of string
	while (strIn[x] != 0) {
		strOut[y] = strIn[x];
		x++;
		y++;
	}

	// last half now first
	strOut[y] = '@'; // put in @
	y++; // and move up
	x = 0;

	while (strIn[x] != '@') { // copy from start to @ of string
		strOut[y] = strIn[x];
		x++;
		y++;
	}

	// last half now first
	strOut[y] = 0; // end with nul
	calcfit(strOut, argv);
	delete strOut;
}

void spliceOutGene(char *strPtr, int minGeneSizeOut, int maxGeneSizeOut,
				   char *argv[]) {
	int zero_or_one;
	int place = 0;
	int end = 0;
	int startSeg = 0;
	int endSeg = 0;
	char strArray[Max_Str_size];
	char terminalGene[Max_Str_size];
	char terminalBranch[Max_Str_size];
	int x, y;
	// Pick the main function or the ADF for splicing out a gene segment
	x = 0;

	while ((end - place < minGeneSizeOut) || (end - place > maxGeneSizeOut)) {
		x++;

		if (x > 10) {
			return;
		}

		zero_or_one = gp_rand() % 2; // two choices  0 or 1

		if (zero_or_one == 0) {
			getIndex('@', strPtr, place, end);
		} else {
			getIndex('\0', strPtr, place, end);
		}

		// Pick ADF for splicing out a gene segment
	}

	// If it starts at the beginning or ends at the end of the string then don't use it just return
	if (place == 0 || place == end) {
		return;
	}

	// Build branch gene segment
	x = 0; // x is the positional index
	y = place;
	terminalBranch[x++] = '@';

	// load
	while (y < end) {
		terminalBranch[x++] = strPtr[y++];
	}

	terminalBranch[x] = 0;
	getIndex('\0', terminalBranch, startSeg, endSeg);

	while (startSeg == 1) {
		getIndex('\0', terminalBranch, startSeg, endSeg);
	}

	x = 0; // x is the positional index
	y = startSeg;

	// load
	while (y < endSeg) {
		terminalGene[x++] = terminalBranch[y++];
	}

	terminalGene[x] = 0;
	x = 0; // x is the positional index

	// load the head into strArray
	while (x < place) {
		strArray[x] = strPtr[x];
		x++;
	}

	y = 0;

	while (terminalGene[y] != 0) {
		strArray[x] = terminalGene[y];
		x++;
		y++;
	}

	y = end;

	while (strPtr[y] != 0) {
		strArray[x] = strPtr[y];
		x++;
		y++;
	}

	strArray[x] = 0;
	calcfit(strArray, argv);
	return;
}

// adds introns to string
void introns(char *Astring, char *argv[]) {
	float number;
	int dec, sign, ndig = 4;
	int i, x, count;
	char zeros[6];
	char *floatNum;
	char floats[5];
	int zero_or_one;
	char numberStr[10];
	int place = 0;
	int end = 0;
	char strArray[Max_Str_size];
	zero_or_one = gp_rand() % 2; // two choices  0 or 1

	if (zero_or_one == 0) { getIndex('@', Astring, place, end); }
	else { getIndex('\0', Astring, place, end); }

	if (place == 0 || place == end) { return; }

	number = gp_rand() % 10000;
	number = number / 10000;

	// float 0 ?
	if (number == 0) {
		number = .0001;
	}

	floatNum = fcvt(number, ndig, &dec, &sign);
	strcpy(floats, floatNum);
	zeros[0] = 0; // clear string
	i = 0;

	// add zeros to front of number if needed
	while (dec < 0) {
		zeros[i] = '0';
		i++;
		dec++;
	}

	zeros[i] = 0; // end with nul
	numberStr[0] = 0; // set string to nul
	zero_or_one = gp_rand() % 2; // two choices  0 or 1

	if (zero_or_one == 0) {
		strcpy(numberStr, " -."); // put -decimal point in array
	} else {
		strcpy(numberStr, " ."); // put +decimal point in array
	}

	strcat(numberStr, zeros); // then zeros
	strcat(numberStr, floats); // and cat the number
	x = 6;

	// remove zeros from end of number
	while (numberStr[x] == '0') {
		x--;
	}

	numberStr[++x] = '\0';
	zero_or_one = gp_rand() % 2; // two choices  0 or 1 now use for intron type
	x = 0; // x is the positional index for the strArray string

	// load the head into strArray
	while (x < place) {
		strArray[x] = Astring[x];
		x++;
	}

	// load the first part of the intron
	if (strArray[x - 1] != ')' || strArray[x - 1] != ' ') {
		strArray[x] = ' ';
		x++;
	}

	strArray[x] = '(';
	x++;

	if (zero_or_one == 0) {
		strArray[x] = '*';
	} else {
		strArray[x] = '+';
	}

	x++;
	strArray[x] = ' '; // just a space
	x++;
	strArray[x] = '('; // makes "(
	x++;

	if (zero_or_one == 0) {
		strArray[x] = '/'; // makes "(*(/"
	} else {
		strArray[x] = '-'; // else "(+(-"
	}

	x++;
	// load the number and / part of the intron (? (? .123 .123)
	i = 0;

	// load the number first time
	while (numberStr[i] != 0) {
		strArray[x] = numberStr[i];
		i++;
		x++;
	}

	i = 0;

	// load the number last part
	while (numberStr[i] != 0) {
		strArray[x] = numberStr[i];
		i++;
		x++;
	}

	strArray[x] = ')'; // close paren ')'
	x++;
	count = place; // use count for index

	// load the Bmiddle
	while (count < end) {
		strArray[x] = Astring[count];
		count++;
		x++;
	}

	strArray[x] = ')'; // close paren ')'
	x++;
	count = end; // count can again be used for an index

	while (Astring[count] != 0) {
		strArray[x] = Astring[count];
		x++; // load the tail into strArray
		count++;
	}

	strArray[x] = 0; // end with null
	calcfit(strArray, argv);
}

void getIndex(char stopChar, char *Astring, int &Aplace, int &Aend) {
	int randA;
	int openParenA = 0;
	int leafCountA = 0;
	int closeParen;
	int count;
	int Astart;
	char *ptr; // pointer to the start of the string

	if (stopChar == '@') {
		Astart = 0; // cross the main function
	} else {
		// else start at '@' and end at nul
		ptr = strchr(Astring, '@'); // user strchr to find the @
		Astart = ptr - Astring; // start is position of @ +1
	}

	Aplace = Astart; // here Aplace is an index

	// till at stopChar
	while (Astring[Aplace] != stopChar) {
		if (Astring[Aplace] == '(') {
			openParenA++; // count number of open paren in string
		}

		Aplace++; // move and test the next char
	}

	Aplace = Astart; // here Aplace is an index

	// till at stopChar
	while (Astring[Aplace] != stopChar) {
		if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' ||
				Astring[Aplace] == 'k') {
			leafCountA++; // count number of leaves in string
		}

		Aplace++; // move and test the next char
	}

	randA = (gp_rand() % (openParenA + leafCountA) + 1);

	// use branch
	if (randA <= openParenA) {
		Aplace = Astart; // here Aplace becomes the position
		count = 0;

		while (count < randA) {
			if (Astring[Aplace] == '(') {
				count++; // count the open paren till = randA
			}

			Aplace++; // Aplace becomes the position for this randa open
		}

		Aend = Aplace;
		closeParen = 0;

		while (closeParen < 1) {
			if (Astring[Aend] == '(') {
				closeParen--; // subtract for every open paren
			}

			if (Astring[Aend] == ')') {
				closeParen++; // add for every close paren
			}

			Aend++; // Aend is now the index for the close paren
		} // Aplace is the index for the start

		Aplace--; // backup to get '('

		if (Astring[Aplace - 1] == 'F') {
			Aplace--; // backup to get 'F'
		}

		if (Astring[Aplace - 1] == ' ') {
			Aplace--; // backup to get ' ' (space)
		}
	} else {
		// it is a leaf randA > openParenA
		randA = (randA - openParenA);
		Aplace = Astart; // here Aplace becomes the position
		count = 0;

		while (count < randA) {
			Aplace++;

			if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' || Astring[Aplace] == 'k')
			{ count++; } // count the leaves till = randA
		}

		// test for sign on leaf
		if (Astring[Aplace - 1] == '-') {
			Aplace--; // if negative backup one
		}

		Aend = Aplace;

		while (Astring[Aend] != ')' && Astring[Aend] != ' ' && Astring[Aend] != '(' &&
				Astring[Aend] != stopChar &&
				Astring[Aend] != 0) {
			Aend++; // Aend is now the position for end of leaf
		}

		Aplace--; // and backup one to pickup space on front end
	}
}

// kill the weekest random 3
void kill(char *argv[]) {
	int k, i, j, x;

	for (x = 0; x < 3; x++) {
		k = pop.getMin();
		i = pop.getMinCount(k);
		j = gp_rand() % i; // makes j the rand pick of lowest fitness value
		pop.first(); // set current to first one in list

		if (j == 0 && pop.getFit() == k) {
			pop.remove(); // remove
		} else {
			if (pop.getFit() == k) { i = 1; } // getfitness of current node
			else { i = 0; }

			while (i < j) {
				// set current to next one in list
				if (pop.next() == 1) {
					error((char *) "no pop; end of list", argv);
				}

				// getfitness of current node
				if (pop.getFit() == k) { i++; }
			}

			// remove the node that current points to
			pop.remove();
		}
	}
}

long double recalcF(char expresion[], int &i, int x, int &divBy0, int &overSize,
					char *argv[]) {
	char *atptr; // @ptr
	char *kptr;
	long double temp1 = 0;
	int index;
	int countParen = -1;
	char *F;
	atptr = strchr(expresion, '@'); // set pointer to @
	// next look for the k in the @ string
	kptr = strchr(atptr, 'k');
	F = strchr(atptr, 'F');

	if (F) {
		cout << "\n" << atptr << "\n";
		exit(9);
	}

	i += 2; // first move past the 'F', then past the '('

	while (countParen < 0) {
		if (expresion[i] == '(') {
			countParen--;
		}

		if (expresion[i] == ')') {
			countParen++;
		}

		// if it has k then calc F(.....) else just run it up
		if (kptr) {
			// x is the index for x
			switchCase(expresion, i, x, temp1, divBy0, overSize,
					   argv);
		} else {
			i++; // ignore F(....) just index past
		}
	}

	if (kptr) {
		// pop the value for k
		num.pop(temp1);
	}

	index = 1; // past the @

	if (atptr[index] == '@') {
		cout << atptr << "\n";
		error((char *) "problem here", argv);
	}

	// till end of string
	while (atptr[index] != 0) {
		// temp1 is sent as k value
		switchCase(atptr, index, x, temp1, divBy0, overSize,
				   argv); // x is the index for x
	}

	num.pop(temp1); // pop the answer
	return temp1;
}
// -------------------------------------------------------------

void switchCase(char expresion[], int &i, int x, long double k, int &divBy0,
				int &overSize, char *argv[]) {
	long double temp1;
	long double temp2;
	char tempchar;
	char float_str[9]; // to hold floating point number as string
	int sign = 1; // 1 or -1
	int j;
	int tooLarge = 10000000;
	double tooSmall = 0.0000001;

	switch (expresion[i]) {
		case 'F':
			if (num.push(recalcF(expresion, i, x, divBy0, overSize,
								 argv))) {} // the value is now in the stack
			else {
				error((char *) "expresion F num Stack full", argv);
			}

			break;

		case '+':
		case '*':
		case '/':
			if (op.push(expresion[i])) {}
			else {
				cout << "g = " << g << "\n";
				cout << expresion << "\n";
				error((char *) "expresion Op Stack full", argv);
			}

			i++;
			break;

		case '-':
			if (expresion[i - 1] == '(') { // then it is an op
				if (op.push(expresion[i])) {}
				else {
					cout << expresion << "\n";
					error((char *) "expresion Op Stack full", argv);
				}

				i++;
			} else { // it is a negative sign
				sign = -1; // set sign to negative
				i++; // so go past the negative sign

				if (expresion[i] == '.') { // if number
				case '.':
					j = 0;

					while (expresion[i] != ' ' && expresion[i] != ')' && expresion[i] != '@' &&
							expresion[i] != 0) {
						float_str[j] = expresion[i];
						i++;
						j++;
					}

					float_str[j] = '\0'; // end with null

					if (num.push(sign * atof(float_str))) {}
					else {
						error((char *) "expresion num at float Stack full", argv);
					}
				} else { // else not number is k or X or F
					if (expresion[i] == 'k') { // if X
					case 'k':
						if (num.push(sign * k)) {} // load X from array into stack
						else {
							error((char *) "expresion k num Stack full", argv);
						}
					}

					if (expresion[i] == 'X') { // if X
					case 'X':
						if (num.push(sign * arrayXY.getX(x))) {} // load X from array into stack
						else {
							error((char *) "expresion X num Stack full", argv);
						}
					}

					i++; // past the X or F or k
					sign = 1; // reset used or not
				}
			}

			break;

		case '(': // open parenthesis
		case ' ': // space
			i++;
			break;

		case ')':
			num.pop(temp1); // pop top of stack last on first off
			num.pop(temp2);
			op.pop(tempchar);

			switch (tempchar) {
				case '*':
					if ((temp2 + temp1) > tooLarge) {
						overSize++;

						if (num.push(temp2 * temp1 / 2)) {}
						else {
							error((char *) "expresion Num Stack full", argv);
						}
					} else {
						if (num.push(temp2 * temp1)) {}
						else {
							error((char *) "expresion Num Stack full", argv);
						}
					}

					break;

				case '/':
					if (temp1 == 0 || temp2 == 0 || temp1 < tooSmall) {
						if (num.push(1.0)) {}
						else {
							error((char *) "expresion Num Stack full", argv);
						}

						if (temp1 == 0 || temp2 == 0) {
							divBy0++;
						} else {
							overSize++;
						}
					} else {
						if (num.push(temp2 / temp1)) {}
						else {
							error((char *) "expresion Num Stack full", argv);
						}
					}

					break;

				case '+':
					if (num.push(temp2 + temp1)) {}
					else {
						error((char *) "expresion Num Stack full", argv);
					}

					break;

				case '-':
					if (num.push(temp2 - temp1)) {}
					else {
						error((char *) "expresion Num Stack full", argv);
					}

					break;
			}

			i++; // go past the ')'
			break;
	}
}

void gp_srand(unsigned long long seed) {
	unsigned int  j;
	// seed the first routine
	RandIx1 = (IC1 + seed) % M1;
	// which is then used to seed the second
	RandIx1 = (IA1 * RandIx1 + IC1) % M1;
	RandIx2 = RandIx1 % M2;
	// and third routines
	RandIx1 = (IA1 * RandIx1 + IC1) % M1;
	RandIx3 = RandIx1 % M3;

	// Fill the random variable table with sequential uniform values
	for (j = 1; j <= 97; j++) {
		// Deviates generated by the first two routines
		RandIx1 = (IA1 * RandIx1 + IC1) % M1;
		RandIx2 = (IA2 * RandIx2 + IC2) % M2;
		// Low and high order pieces combined here to create value
		TrandArray[j] = (RandIx1 + RandIx2 * RM2) * RM1;
	}
}

unsigned long long gp_rand(void) {
	double temp;
	unsigned int j;
	// Except when initializing, this is where we start.
	// Generate the next number for each sequence.
	RandIx1 = (IA1 * RandIx1 + IC1) % M1;
	RandIx2 = (IA2 * RandIx2 + IC2) % M2;
	RandIx3 = (IA3 * RandIx3 + IC3) % M3;
	// Use the third sequence to get an integer between 1 and 97
	j = 1 + ((97 * RandIx3) / M3);

	if (j > 97 || j < 1) { cout << "gplgp_rand() ??"; }

	// Return that table entry, and refill it.
	temp = TrandArray[j] * M3;
	TrandArray[j] = (RandIx1 + RandIx2 * RM2) * RM1;
	return (unsigned long long) temp;
}

void error(char *msg, char *argv[]) {
	writeOut(argv); // dump data to file
	cout << "ERROR: " << msg << "." << "\n";
	exit(9);
}

void exitOut(char *argv[], int fit) {
	FILE *stream;
	char fileName[30];
	fileName[0] = 0;
	strcpy(fileName, argv[23]);
	strcat(fileName, ".results.dat");
	stream = fopen(fileName, "a"); // append to file
	// print all 24 args
	fprintf(stream,
			"%3d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			fit, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7],
			argv[8], argv[9], argv[10], argv[11], argv[12], argv[13], argv[14], argv[15],
			argv[16], argv[17], argv[18], argv[19], argv[20], argv[21], argv[22], argv[23],
			argv[24]);
	fclose(stream);
}

void writeOut(char *argv[]) {
	FILE *stream;
	char fileName[30];
	fileName[0] = 0;
	strcpy(fileName, argv[23]);
	strcat(fileName, ".pop.log");
	stream = fopen(fileName, "a"); // append to file
	pop.first(); // set to first in list

	do {
		fprintf(stream, "%3d,%s\n", pop.getFit(), pop.giveData());
	} while (pop.next() == 0); // till end of list

	fclose(stream);
}

void buildString(char side, char boolein) {
	char function[60];
	int x;
	char opArray[2];
	char numberStr[7];
	function[0] = 0; // reset string to clear it
	strcpy(function, "("); // start out with open "("
	strcat(function, p.operators(opArray)); // add the op
	x = (gp_rand() % 4); // makes 0 to 3

	// if 0 then 2 leaves
	if (x == 0) {
		// make new random leaf and put in leafArray string
		leaf(boolein);
		// cat the leaf into function array
		strcat(function, leafArray);
		// make new random leaf and put in leafArray string
		leaf(boolein);
		// cat the leaf into function array
		strcat(function, leafArray);
	}

	// if 1 then single on left, leaf on right
	if (x == 1) {
		strcat(function, p.numberStr(boolein, numberStr));
		// make new random leaf and put in leafArray string
		leaf(boolein);
		// cat the leaf into function array
		strcat(function, leafArray);
	}

	// if 2 then  leaf left single on right
	if (x == 2) {
		// make new random leaf and put in leafArray string
		leaf(boolein);
		// cat the leaf into function array
		strcat(function, leafArray);
		strcat(function, p.numberStr(boolein, numberStr));
	}

	// if 3 then  only one leaf
	if (x == 3) {
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

	// y = yes to F( no to k
	if (gp_rand() % 3 == 0 && boolein == 'y') {
		strcpy(leafArray, " F(");
	} else {
		strcpy(leafArray, " (");
	}

	strcat(leafArray, p.operators(opArray));
	strcat(leafArray, p.numberStr(boolein, numberStr));
	strcat(leafArray, p.numberStr(boolein, numberStr));
	strcat(leafArray, ")");
}

#undef M1
#undef IA1
#undef IC1
#undef RM1
#undef M2
#undef IA2
#undef IC2
#undef RM2
#undef M3
#undef IA3
#undef IC3
