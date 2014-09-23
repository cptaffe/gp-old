/* Genetic Programming: Phil H Williams
*/

//#include <mkl_vsl.h>
//#include "mkl.h"

// http://software.intel.com/sites/products/documentation/hpc/mkl/vslnotes/7_6_Example_of_VSL_Use.htm

#include <iostream>
using namespace std;

#include <fstream>
#include <cstdlib>
#include <math.h>

//#include <curses.h>
#include <iomanip>
#include <locale.h>
#include <stdio.h>

#include <string.h>
#include  <ctype.h>
#include   <time.h>
#include <stdlib.h>
// added

#include "tstack2.h" // the stacks for ops & numbers
#include "loadxy.h"   // struct for curve fit data
#include "gp_list.h" // list struct for population

#include "poprand4.h"

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

void gp_srand(unsigned long long seed);
unsigned long long gp_rand(void);

long double recalcF(char expresion[], int &i, int x, int &divBy0, int &overSize, char *argv[]);
//int calcfit(char expresion[], char *argv[], int scaleFit);
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

// added for insertExoGene
#define ARRAYSIZE             100
void leaf(char);
void buildString(char, char);
randpop p;
char leafArray[20];
char ArrayLeft[60];
char ArrayRight[60];
// added for insertExoGene

// global variables an absolute necessity for random number system....
unsigned long long RandIx1, RandIx2, RandIx3;
double TrandArray[98];

gp_list pop;  // pop is data type list
XYclass arrayXY;  // data type array for X & Y data from question file
// from loadxy.h

//long double number;
//har charx;
//template long double Tstack2<number> num(stack_size);  //  the operands (numbers) stack of default size
//template char Tstack2<charx> op(stack_size);         //  the operators stack + - / *

Tstack2<long double> num(stack_size);  //  the operands (numbers) stack of default size
Tstack2<char> op(stack_size);         //  the operators stack + - / *

struct ptr_struct {
  char *charPtr;
};

int StackSize = 0;
int highFitness = 0;
int size = 0;
int newSize = 0;
int fitSize = 0; // size of question file data
int g = 0; // generation count
char c[]={27,91,72,27,91,50,74};
int scaleFitness = 1;


int main (int argc, char *argv[])  /* pickup command line arguments */
{
  int j;
//  char strArray[Max_Str_size]; // put fragments into one string
//  char intronArray[100];
//  char cmdStr[70];
//  char *questionFile;

  printf("%s",c);  // unix used as clear
/*    Syntax
      argv[0] the executable file name
      argv[1] [50 to 500] g max number of generations
      argv[4] [100 to 600] popsize, max pop size
      argv[5] [500 to 6500] randpopsize, size of initial pop
      argv[6] [0.01 to 0.09] threshold, how close is ycalc to ytrue
#      argv[2] [1 to 10] Xover ratio how often for self cross over

    argv[7] [1 to 10] thresholdDivisor, how close is ycalc to ytrue after divsion
    argv[8] [1 to 12] intronProb, how often will introns be added
    argv[9] [1 to 12] genejumpProb, how often will genejump run
    argv[10] [1 to 12] founderMinFitness, min fitness or run randpop again
    argv[11] [1 to 11] ratioOfXoverMainadf, ratio of xover main/adf Xmain
    if (rand % argv == 0) then Xadf  2 = 50 50 chance
    1/argv = chance

    argv[12] [1 to 12] exitMultValue, mult by fitness
    argv[13] [600 to 5600] popTargetSize, population target size, minPopSize
    argv[14] [1 to 6 | X (highfit -1)] minKeepFitness, min fit value to put in population
    argv[15] [1 to 10 ] scaleFitness, scaler

argv[x] [1 to 10] pickForInsertionProb, 

    argv[16] [8 to 20] gDiff, number of generations between fit1 and fit2 for testing punctuated equilibrium 
    argv[17] [5 to 20] deltaPunkInk, level of punctuated equilibrium reached or exit out
    argv[18] [0.8 to 1.3] multByNewSize, mult by newPopSize
    
    argv[19] [200 to 1200] maxChroLen, max chrom length

    argv[20] [1 to 10] pickForSpliceOutGeneProb
    argv[21] [20 to 50] minGeneSizeOut
    argv[22] [50 to 1000] maxGeneSizeOut

    argv[23] [CPUx] name of CPU running on used as stem
    argv[24] Question file example [3X^2+2X-3.dat]
*/
  if (argc < 25) {
//    cout << "\nSyntax\nargv1[g 20 to 500] argv2[max popsize 500 to 600] argv3[randpopsize 500 to 4000] argv4[threshold] argv5[threshold diviser] argv6[intron prob] argv7[genejump prob] argv8[founder min fitness 1 to 5] argv9[ratio of xover main/adf 1] argv10[exit mult-value] argv11[min pop target size] argv12[min keep fitness 1 to 6 or X] argv13[scalar] argv14[pickForInsertion prob 1 to 10] argv15[pickForSelfCross prob 1 to 10] argv16[8 to 20] argv17[5 to 20] argv18[CPUx] argv19[question file name]\n";
    for (j = 0; j <= argc; j++) {
      cout << "argv" << j << " " << argv[j] << "\n";
    }
    cout << "\n\n";
    exit(9);
  }

  char cmdStr[70];
  int gDiff, exitVal;
  int deltaPunkInk;
  int index = 0;
  int *gDiffArray;
  int minPopSize, maxPopSize, exitMultValue, founderMinFitness, intronProb, genejumpProb, ratioOfXoverMainadf;
  int minKeepFitness, pickForInsertionProb, pickForSelfCrossProb, pickForSpliceOutGeneProb;
//  int maxChroLen,
  int minGeneSizeOut, maxGeneSizeOut;
//  float threshold, thresholdDiviser, multByNewSize;
  float multByNewSize;
  char *endptr; // for strtod

  maxPopSize = atoi(argv[2]);
//  randpopsize = atoi(argv[3]);
//  threshold = atoi(argv[4]);
//  thresholdDiviser = atoi(argv[5]);
  intronProb = atoi(argv[6]);
  genejumpProb = atoi(argv[7]);
  founderMinFitness = atoi(argv[8]);
  ratioOfXoverMainadf = atoi(argv[9]);
  exitMultValue = atoi(argv[10]);
  minPopSize = atoi(argv[11]);
//  minKeepFitness = atoi(argv[12]);
  scaleFitness = atoi(argv[13]);
  pickForInsertionProb = atoi(argv[14]);
  pickForSelfCrossProb = atoi(argv[15]);
  gDiff = atoi(argv[16]);
  deltaPunkInk = atoi(argv[17]);
  multByNewSize = strtod(argv[18], &endptr);
//  maxChroLen = atoi(argv[19]);
  pickForSpliceOutGeneProb = atoi(argv[20]);
  minGeneSizeOut = atoi(argv[21]);
  maxGeneSizeOut = atoi(argv[22]);
//argv[23] [CPUx] name of CPU or node running on used as stem
//argv[24] Question file example [3X^2+2X-3.dat]

   /* This is needed to avoid an infinite loop. */
  if (maxPopSize < minPopSize) {
    maxPopSize = minPopSize;
  }

  if (!(gDiffArray = new int[gDiff])) { 
    cout << "ERROR\n";
    exit(9);
  }
//for (j = 0; j < gDiff; j++) {
// gDiffArray[j] = 200;
//cout << gDiffArray[j] << ",";
//}

//return 1;

//  loadxy();  // load the x & y from file to array
  loadxy(argv[24]);  // load the x & y from file to arraya
//  cout << "\n\n" << questionFile << " " argv[14] << "\n";
//  cout << "\n" << argv[17] << "\n";
//exit(1);
  
  gp_srand( (unsigned)time(NULL) ); // seed the rand

  cout.setf(ios::right,ios::adjustfield);

  cmdStr[0] = 0;

  strcpy(cmdStr, "./seedPopRand.pl "); // building string from parts
  strcat(cmdStr, argv[23]); // the stem name for the rand pop file 
  strcat(cmdStr, "."); // building string from parts
  strcat(cmdStr, "seedSource.pop.log "); // building string from parts
  strcat(cmdStr, argv[3]); // randpopsize
  strcat(cmdStr, " ");
  strcat(cmdStr, argv[23]); // the stem name for the rand pop file

//cout << cmdStr << "\n";
//exit(1);
  /* While the fitness is less than the minimum founder fitness, continue with first_run function,
   * also continue if the minimum population is not reached.
   */
//  while (highFitness < atoi(argv[8]) && size < atoi(argv[11])) {  //argv[x] = min fit to go
  j = 0;
  while (highFitness < founderMinFitness && size < minPopSize) {  //argv[x] = min fit to go
//    while (pop.length()/2 > atoi(argv[2]))  kill(argv);
    while (pop.length() > maxPopSize) kill(argv);
    exitVal = system(cmdStr);  // run command string in loop
    if (exitVal != 0) { 
      fprintf(stderr, "Error\n%s\n",cmdStr);
      exit(9);
    }
//      fprintf(stderr, "I am in stderr");
//    cout << cmdStr << "\n";
//exit(1);
    cout << "\nGeneration    Add to pop    Highfitness out of " << fitSize << "\n";
    first_run(argv); // first run puts data in linked list
    if (j++ > atoi(argv[1])) {
      cout << "exit 0" << "\n";
      exit(0);
    }
  }

//exit(1);
//cout << "after first run and gp_srand" << endl;

  while (g < atoi(argv[1])) { // argv[1] = number of generations
    g++;      // increment generation count
//    cout << setw(4) << g << "\r";
//    while (pop.length() > atoi(argv[2]) && g >= 2) kill(argv);
    while (pop.length() > maxPopSize && g >= 2) kill(argv);
    newSize = pop.length();
    if (newSize < 3) {
      cout <<"   Low population size only " << newSize << "\n";
      exitOut(argv, highFitness );
      return 0;
    }
//    cout << endl << "Size of population " << setw(4) << newSize << endl;
//    cout << "\n" << "multByNewSize " << multByNewSize << "\n";

//exit(1);

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
    } // end for

    if ((highFitness * exitMultValue) <= g) { 
      printf("\nLow fitness of %d for generation %d\n",highFitness,g);
      printf("          %d * %d = %d\n", highFitness, exitMultValue, highFitness * exitMultValue);
      writeOut(argv); // dump data to file
      exitOut(argv, highFitness);
      return 0;
    }
 
    if (index < gDiff) {
      gDiffArray[index] = highFitness;
//printf("\nload gDiffArray index = %d with %d\n",index, gDiffArray[index]);
      index++;
    }
    else {
      if (highFitness - gDiffArray[0] < deltaPunkInk) { 
        printf("\nLow level of punctuated equilibrium for generation %d\n%d = %d - %d\n",g,highFitness - gDiffArray[0],highFitness,gDiffArray[0]);
//        for (j = 0; j < gDiff; j++) {
//          cout << gDiffArray[j] << ",";
//        }
//        cout << "\n";
        writeOut(argv); // dump data to file
        exitOut(argv, highFitness);
//        cout << "delete gDiffArray\n";
        delete gDiffArray;
        return 0;
      }
      for (j = 0; j < gDiff; j++) {
        gDiffArray[j] = gDiffArray[j +1];
      }
      gDiffArray[gDiff -1] = highFitness;
    }

  }
  
  writeOut(argv); // dump data to file
  exitOut(argv, highFitness);
  cout << "\nExit program pw\n";
  delete gDiffArray;
  return 0;
}
//-------------------------------------------------------------
void selfCross(char Astring[], int ratioOfXoverMainadf, char *argv[])
{
  int Aplace = 0;
  int Bplace = 0;   // postions holders
  int Aend = 0;
  int Bend = 0;   // postions holders
  int x, y;
  int index;
  char stopChar;
  char same = 't'; // track if same substrings
  char firstXover[Max_Str_size];
  char secondXover[Max_Str_size];

//cout << "\n->" << Astring << "<-Astring,selfCross\n"; 

  stopChar = '@';  // start at 0 and end at '@'or
  if (gp_rand() % ratioOfXoverMainadf == 0 ) {
    stopChar = 0; // start at @ to end of string
  }

//cout << "start crossover " << argv[10] << endl;

  index = 0; // use index as a counter for now
  while (same == 't') {
    //cout << " index " << index << endl;
    if (index > 5) return; // only the same so give up and return
    index++;
    getIndex(stopChar, Astring, Aplace, Aend);
    getIndex(stopChar, Astring, Bplace, Bend);
    if ((Aplace - Aend) == (Bplace - Bend)) { // the strings are same length
      x = Aplace;    // test to see if the are the same data too
      y = Bplace;
/*      while (Astring[x] == Astring[y]) {
	x++;
	y++;
      }
*/
      if (x == Aend && y == Bend) same = 't';  // same substrings so go again
      else same = 'f';
    }
    if (Aplace == Aend || Bplace == Bend || Aplace == 0 || Bplace == 0) same = 't';
    else same = 'f';
  }

  x = 0; // x is the positional index for the strArray string
  while (x < Aplace) {   // load the Ahead into strArray
    firstXover[x] = Astring[x];
    x++;
  }

/*
  if (firstXover[x -1] != ')' || firstXover[x -1] != ' ') {  
    firstXover[x] = ' ';
    x++;
  }
*/
  index = Bplace;
  while (index < Bend) {  // load the Bmiddle
    firstXover[x] = Astring[index];
    index++;
    x++;
  }

  index = Aend;
  while (Astring[index] != 0) {
    firstXover[x] = Astring[index];
    x++;                // load the Atail into strArray
    index++;
  }
  firstXover[x] = 0; // end with null

//cout << "\n->" << firstXover << "<-firstXover,selfCross\n"; 
//  calcfit(firstXover, argv, scaleFitness);
  calcfit(firstXover, argv);
//cout << "after calcfit 1" << endl;

  x = 0; // x is the positional index for the string
  while (x < Bplace) {   // load the Bhead into strArray
    secondXover[x] = Astring[x];
    x++;
  }

  if (secondXover[x -1] != ')') {  
    secondXover[x] = ' ';
    x++;
  }

  index = Aplace;
  while (index < Aend) {  // load the Amiddle
    secondXover[x] = Astring[index];
    index++;
    x++;
  }
  
  index = Bend;
  while (Astring[index] != 0) {
    secondXover[x] = Astring[index];
    x++;                // load the tail
    index++;
  }
  secondXover[x] = 0; // end with null

//cout << "\n->" << secondXover << "<-secondXover,selfCross\n"; 
//cout << "before calcfit 2" << endl;
//  calcfit(secondXover, argv, scaleFitness);
  calcfit(secondXover, argv);
//cout << "after calcfit 2" << endl;

  return;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
void buildIntron(char *arrayIn)
{ // Send the leaf in this dynamic array then return array containing the intron.
  int zero_or_one;
  const char multByOne[] = "\(* (/";
  const char subtractZero[] = "\(+ (-";
  char leafStr[8];
//  int x;
 
  leafStr[0] = 0; 
  strcat(leafStr, arrayIn);

  arrayIn[0] = 0;

  zero_or_one = gp_rand() % 2; // two choices  0 or 1
 
  if (zero_or_one == 0) strcat(arrayIn, multByOne);
  else strcat(arrayIn, subtractZero); 

  strcat(arrayIn, leafStr);
  strcat(arrayIn, leafStr);

//  cout << " Intron " << "\n" << arrayIn << "\n";

  return;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
void getIndexLeafOnly(char stopChar, char *Astring, int &Aplace, int &Aend)
{
  int randA;
//  int openParenA = 0;
  int leafCountA = 0;
  int closeParen;
  //int x, y;
  int count;
  int Astart;
  char *ptr;  // pointer to the start of the string

//cout << "getindex" << endl;

  if (stopChar == '@') {
    Astart = 0;          // cross the main function
  }
  else {  // else start at '@' and end at nul
    ptr = strchr(Astring,'@'); // user strchr to find the @
    Astart = ptr-Astring; // start is position of @ +1
  }

  Aplace = Astart; // here Aplace is an index
  while (Astring[Aplace] != stopChar) { // till at stopChar
    if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' || Astring[Aplace] == 'k') {
      leafCountA++;  // count number of leaves in string
    }
    Aplace++;    // move and test the next char
  }

  randA = (gp_rand() % leafCountA +1);

  Aplace = Astart; // here Aplace becomes the position
  count = 0;
  while (count < randA) {
    Aplace++;
    if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' || Astring[Aplace] == 'k') {
      count++;  // count the leaves till = randA
    }
  }
    //test for sign on leaf
  if (Astring[Aplace-1] == '-') {
    Aplace--; // if negative backup one
  }
  Aend = Aplace;
  while (Astring[Aend] != ')' && Astring[Aend] != ' ' && Astring[Aend] != '(' && Astring[Aend] != stopChar && Astring[Aend] != 0) {
	Aend++;  // Aend is now the position for end of leaf
  }
  Aplace--; // and backup one to pickup space on front end
  return;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
//void insertExoGene(char strArray[], char Astring[], char Bstring[], char *argv[])
void insertExoGene(char Astring[], char transposIn[], int flip, char *argv[])
{                                  // Astring is the original genome, transposon for insertion
  int Aplace = 0;
//  int Bplace = 0;   // postions holders
  int Aend = 0;
//  int Bend = 0;   // postions holders
  int x, y;
//  int index;
  char stopChar;
//  char *DynArray1; // pointer for dynamic arrays
//  char strArray[Max_Str_size]; 
//  int leafLen;
//  char leafArray[8];
  char leafArray[Max_Str_size];
  char tmpArray[Max_Str_size];
  char charArray[Max_Str_size];
// Save the leaf here to make into an intron
  const char paren[] = ")";

  if (flip == 0) {
    stopChar = '@';  // start at 0 and end at '@'or
  }
  else {
    stopChar = '\0'; // start at @ to end of string
  }

//cout << "Astring\n->" << Astring << "<-\n";

  getIndexLeafOnly(stopChar, Astring, Aplace, Aend);
// For the Astring find some leaf node to add to

  x = 0;
  y = Aplace;
//cout << "Aend " << Aend << "\n";
  while (y < Aend) {
//    cout << y << "\n";
    leafArray[x] = Astring[y];
    x++;
    y++;
  }
  leafArray[x] = 0;
  
//cout << "\n->" << leafArray << "<-\n";
//cout << "\n->" << transposIn << "<-\n";
//  leafLen = strlen(leafArray); 
//  while (!(DynArray1 = new char[strlen(Astring) + strlen(Bstring) +(2*leafLen +5)])) kill(argv);
//  strcat(DynArray1, leafArray);
  buildIntron(leafArray);
// The leaf array now contains the leaf built into an intron

//cout << "\n->" << leafArray << "<-\n";
// intron now in 
//  cout << " Intron " << "\n" << DynArray1 << "\n";
//  cout << " exoGen " << "\n" << Bstring << "\n";
//  delete DynArray1;

// Copy the head to the tmp array
//  tmpArray[0] = 0;
  x = 0;
  y = 0;
  while (y < Aplace) {
    tmpArray[x] = Astring[y];
    x++;
    y++;
  }
  tmpArray[x] = ' ';
  x++;
  tmpArray[x] = 0;
//  tmpArray[strlen(tmpArray) +1] = ' ';
// Copy the intron
  strcat(tmpArray, leafArray);
  strcat(tmpArray, paren);
// Copy the transposon
  strcat(tmpArray, transposIn);
  strcat(tmpArray, paren);
//  tmpArray[strlen(tmpArray) +1] = ')';
//  tmpArray[strlen(tmpArray) +2] = 0;

// memcpy( subbuff, &buff[10], 4 );
// Copy the tail 
  x = 0;
  y = Aend;
  while (Astring[y] != 0) {
    charArray[x] = Astring[y];
//printf("%c\n",Astring[y]);
    x++;
    y++;
  }
  charArray[x] = 0;

// Copy the tail 
  strcat(tmpArray, charArray);
//cout << "tmpArray:\n->" << tmpArray << "<-\n";
//cout << "charArray:\n->" << charArray << "<-\n";

  transposIn[0] = 0; // clear array
  strcpy(transposIn, tmpArray);  
//cout << "transposIn:\n->" << transposIn << "<-\n";

//exit(1);

  return;
}
//-------------------------------------------------------------
//void makeExoGene(char strArray[], char Astring[], char *argv[])
void makeExoGene(char *DynArrayTranspIn, int flip, char *argv[])
// insert exogenous gene
{
  char expresion[ARRAYSIZE];
  char opArray[2];

// Find out if the main has any ADF
//  pc = strchr(Astring, 'F');
// If it has an ADF then flip to see if we work the main or the ADF
// If no ADF then only work the main

//  cout < "pointer pc " << pc << "\n";

  expresion[0] = 0;
  if (flip == 0) {
// build main random exogenous gene
  buildString('r','y');
  strcat(expresion, ArrayRight);
  }  

//===============================================================

  else {

//  strcat(expresionADF, "("); // start out with open "("
//  p.operators(opArray); // add the op
//  strcat(expresionADF, opArray); // add the op
  buildString('l','n');   //   with no F in string
  strcat(expresion, ArrayLeft);
//  buildString('r','n');
//  strcat(expresionADF, ArrayRight);
//  strcat(expresionADF, ")"); // end with close )
  }
//printf("\n->%s<-\n", expresion);

  strcpy(DynArrayTranspIn, expresion);

// test for fitness

  return;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
int pickForSpliceOutGene(int minGeneSizeOut, int maxGeneSizeOut, char *argv[])
{
  char *strptr;   // char pointer for strings
  char *DynArrayPick; // pointer for dynamic arrays
//  char *DynArrayTransp; // pointer for dynamic arrays
  unsigned long long spin_1;
  unsigned long long maxSpiner;
  char strArray[Max_Str_size]; 
  char *pc;
  int flip = 0;
	
  maxSpiner = pop.getMaxSpin(); // spiner loaded here
  spin_1 = (gp_rand() % (maxSpiner -1)) +1;
  strptr = pop.SpinString(spin_1);
  strcpy(strArray, strptr); // copy the string so the pointer can be removed
  strptr = 0; //null the pointer for the kill

//  if (!(gDiffArray = new int[gDiff])) {
//    cout << "ERROR\n";
//    exit(9);
//  }


//  while (!(DynArrayPick = new char[strlen(strArray)+1])) kill(argv);
  if (!(DynArrayPick = new char[strlen(strArray)+1])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }
  strcpy(DynArrayPick, strArray); 

  spliceOutGene(DynArrayPick, minGeneSizeOut, maxGeneSizeOut,argv);

  delete DynArrayPick;  
}
//-------------------------------------------------------------
int pickForIntrons(char *argv[])
{
  char *strptr;   // char pointer for strings
  char *DynArrayPick; // pointer for dynamic arrays
//  char *DynArrayTransp; // pointer for dynamic arrays
  unsigned long long spin_1;
  unsigned long long maxSpiner;
  char strArray[Max_Str_size]; 
  char *pc;
  int flip = 0;
	
  maxSpiner = pop.getMaxSpin(); // spiner loaded here
  spin_1 = (gp_rand() % (maxSpiner -1)) +1;
  strptr = pop.SpinString(spin_1);
  strcpy(strArray, strptr); // copy the string so the pointer can be removed
  strptr = 0; //null the pointer for the kill
//  while (!(DynArrayPick = new char[strlen(strArray)+1])) kill(argv);
  if (!(DynArrayPick = new char[strlen(strArray)+1])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }
  strcpy(DynArrayPick, strArray); 

  introns(DynArrayPick, argv);

  delete DynArrayPick;  
}
//-------------------------------------------------------------
//-------------------------------------------------------------
int pickForSelfCross(int ratioOfXoverMainadf, char *argv[])
{
  char *strptr;   // char pointer for strings
  char *DynArrayPick; // pointer for dynamic arrays
//  char *DynArrayTransp; // pointer for dynamic arrays
  unsigned long long spin_1;
  unsigned long long maxSpiner;
  char strArray[Max_Str_size]; 
  char *pc;
  int flip = 0;
	
  maxSpiner = pop.getMaxSpin(); // spiner loaded here
  spin_1 = (gp_rand() % (maxSpiner -1)) +1;
  strptr = pop.SpinString(spin_1);
  strcpy(strArray, strptr); // copy the string so the pointer can be removed
  strptr = 0; //null the pointer for the kill
//  while (!(DynArrayPick = new char[strlen(strArray)+1])) kill(argv);
  if (!(DynArrayPick = new char[strlen(strArray)+1])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }

  strcpy(DynArrayPick, strArray); 

  selfCross(DynArrayPick, ratioOfXoverMainadf, argv);

//cout << "\n->" << DynArrayPick << "<-\n";

  delete DynArrayPick;  
}
//-------------------------------------------------------------
//-------------------------------------------------------------
int pickForGenejump(char *argv[])
{
  char *strptr;   // char pointer for strings
  char *DynArrayPick; // pointer for dynamic arrays
//  char *DynArrayTransp; // pointer for dynamic arrays
  unsigned long long spin_1;
  unsigned long long maxSpiner;
  char strArray[Max_Str_size]; 
  char *pc;
  int flip = 0;
	
  maxSpiner = pop.getMaxSpin(); // spiner loaded here
  spin_1 = (gp_rand() % (maxSpiner -1)) +1;
  strptr = pop.SpinString(spin_1);
  strcpy(strArray, strptr); // copy the string so the pointer can be removed
  strptr = 0; //null the pointer for the kill
//  while (!(DynArrayPick = new char[strlen(strArray)+1])) kill(argv);
  if (!(DynArrayPick = new char[strlen(strArray)+1])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }

  strcpy(DynArrayPick, strArray); 

//cout << "\n->" << DynArrayPick << "<-\n";
  geneJump(DynArrayPick, argv);

  delete DynArrayPick;  
}
//-------------------------------------------------------------
int pickForInsertion(char *argv[])
{
  char *strptr;   // char pointer for strings
  char *DynArrayPick; // pointer for dynamic arrays
  char *DynArrayTransp; // pointer for dynamic arrays
  unsigned long long spin_1;
  unsigned long long maxSpiner;
  char strArray[Max_Str_size]; 
  char *pc;
  int flip = 0;
	
  maxSpiner = pop.getMaxSpin(); // spiner loaded here
  spin_1 = (gp_rand() % (maxSpiner -1)) +1;
  strptr = pop.SpinString(spin_1);
  strcpy(strArray, strptr); // copy the string so the pointer can be removed
  strptr = 0; //null the pointer for the kill
//  while (!(DynArrayPick = new char[strlen(strArray)+1])) kill(argv);
  if (!(DynArrayPick = new char[strlen(strArray)+1])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }

  strcpy(DynArrayPick, strArray); 
  
  pc = strchr(DynArrayPick, 'F');

  if (pc) {
    flip = gp_rand() % 2; // two choices  0 or 1
  }
  else {
    flip = 0;
  }
//	  geneJump(DynArray1, argv);
//printf("\n->%d<-\n", flip);

//  cout << DynArrayPick << "\n";

// Find out if the main has any ADF
//  *pc = strchr(DynArray1, 'F');
// If it has an ADF then flip to see if we work the main or the ADF
// If no ADF then only work the main

//  while (!(DynArrayTransp = new char[Max_Str_size])) kill(argv);
  if (!(DynArrayTransp = new char[Max_Str_size])) {
    cout << "ERROR out of RAM\n";
    exit(9);
  }

//            is the Astring, is new array ready for building
  makeExoGene(DynArrayTransp, flip, argv);
//  cout << DynArrayTransp << "\n";
//insertExoGene(char Astring[], char Bstring[], int flip, char *argv[])
  insertExoGene(DynArrayPick, DynArrayTransp, flip, argv);
//  cout << DynArrayTransp << "\n";

//calcfit(DynArrayTransp, argv, scaleFitness);
calcfit(DynArrayTransp, argv);
//      cout << "\nout of function calcfit\n";


// 1 ) takes an array holding the leaf from the Astring
// 2 )  replaces the leaf with the transposon plus leaf within the intron

  delete DynArrayTransp; // recoop the mem
  delete DynArrayPick; // recoop the mem

//exit(1);

  return 0;
}
//-------------------------------------------------------------
void loadxy(char *questionData)
{

  FILE *XYfile;
  int i;
  char temp[100];

//   if ((XYfile = fopen("question.dat", "r" )) == NULL) {
 if ((XYfile = fopen(questionData, "r" )) == NULL) {
      cout <<" File question.dat could not be opened";
      exit(9);
   }
   else {
     while (!feof(XYfile)) {
       fgets(temp, 99, XYfile);
       fitSize++;
     }
     fitSize--;
     rewind(XYfile);  // resart the file at the begining
     for (i = 0; i < fitSize; i++) {
       fscanf(XYfile, "%s", &temp);   // scan a number X
       arrayXY.loadX(i,atoi(temp));
       fscanf(XYfile, "%s", &temp);   // scan a number Y
       arrayXY.loadY(i,atol(temp));
       //printf("x = %d   y = %ld  index = %d\n",arrayXY.getX(i),arrayXY.getY(i),i);
     }
     fclose(XYfile);
     //while (!kbhit()) {}
   }
}
// *************************************************************

void first_run(char *argv[])
{
  FILE *file_in;
  char line[8500]; // line holds the exspresion from file
  char char1;
  int x; // for char
//  int count = 0;
  char *F;
  char *atptr;
  char cmdStr[60];

  cmdStr[0] = 0;

  strcpy(cmdStr, argv[23]); //argv[15] node name stem 
  strcat(cmdStr, ".pop.dat"); 
//cout << endl<< cmdStr << endl;
//   cout << endl;

//exit(1);


//   if ((file_in = fopen("DualCore.pop.dat", "r" )) == NULL) {
   if ((file_in = fopen(cmdStr, "r" )) == NULL) {
      cout <<" File " << cmdStr << " could not be opened\n";
      exit(9);
   }
   else {
      do {            // till end off file
	 fscanf(file_in, "%c", &char1);   // scan a char
	 x = 0;
	 while (char1 != '\n') { // loop to load the array
	    line[x] = char1;
	    x++;
	    fscanf(file_in, "%c", &char1);
	 } // end while char1
	 line[x] = '\0';          // add null to end the array

//cout << count << ' ' << line << endl;
	 if (strlen(line) != 0)
//calcfit(line, argv, scaleFitness);
calcfit(line, argv);
//	   count++;
      }  while (!feof(file_in));             // till end off file
      fclose(file_in);
      //printf("             End Calc\n");
   }
//cout <<" end first run" << endl;
//exit(0);
} //end function

// *************************************************************
// *************************************************************
int calcfit(char expresion[], char *argv[])
{
  char *endptr; // for strtod
  double tite;                   /* from 1 to .6 as g goes from 1 to 50  */
  int x,i, minKeepFitness;
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

//cout << "\n->"<< expresion << "<-\n";



  if (strlen(expresion) > maxChroLen) {
//  if (strlen(expresion) > ) {
//cout << endl << atoi(argv[18]) << "\n" << atoi(argv[19])<< "\n" << expresion << endl;
//    exit(1);
    return 1;
  }

  fit = 0;
//cout << " start calcfit\n";

  if (g == 0) {
    tite = strtod(argv[4], &endptr);
    minKeepFit = 0;
  }
  else {
    tite = (threshold + thresholdDiviser);
//    if (argv[12] == "X") {
    if (minKeepFitness < 0) {
      minKeepFit = highFitness -1;
    }
    else {
      minKeepFit = minKeepFitness;
    }
  }

//cout << endl << "tite = " << tite << endl << "argv[5] = " << argv[5] << endl << "argv[6] = " << argv[6] << endl;
   
//cout << endl << "argf5 = " << argf5 << endl << "argf6 = " << argf6 << endl;

//cout << endl << endl << expresion << endl;

  for (x = 0; x < fitSize; x++) {  // for loop to change x and y and test them
    //i = 1; // start at one because only ( or space at index zero
    i = 0;
    while (expresion[i] != '@') {
//cout << expresion[i]; // i is incremented in the switchCase call
      switchCase(expresion, i, x, 0.0, divBy0, overSize, argv); // 0.0 for k
      if (divBy0 == 2 || overSize == 2)  {
	while (num.isEmpty() != 1) num.pop(temp1);
	while (op.isEmpty() != 1) op.pop(tempchar);
//cout << endl << "div by zero" << endl;
	return 1; // flush stacks and bail out //////////////////////////////<<<<<  int here
      }
    }
    if (divBy0 == 1) fit--;
    if (overSize == 1) fit--;

    num.pop(temp1);   // pop the answer
    if (num.isEmpty() != 1 || op.isEmpty() != 1) {
      error(" Main Stacks not empty", argv);
    }       // if tite is large then the delta can be large
    if (fabs(arrayXY.getY(x) - temp1) <= tite) fit++;
  }

//  cout << "\r" << setw(4) << g << "            " << setw(5) << size << "            " << setw(4) << highFitness << " ";

  if (fit >= minKeepFit) {
    if (fit <= scaleFitness) { fit = 1; }
    testadd = pop.add(expresion, fit);
    while (testadd == 1) {
      kill(argv);
      testadd = pop.add(expresion, fit);
    }
   
    if (fit > highFitness) {
//      cout << "\r" << setw(4) << g << "            " << setw(5) << size << "            " << setw(4) << highFitness << " \n";
      highFitness = fit;  // set the global fitness
    //graphOutput(current-start, g, highFitness, expresion);
    }
    
    size++;
//    printf("%s",c);  // unix used as clear

    if (fit == fitSize) { // found it
      cout << "*****  Found Approxamate Solution  *****" << "\n\n";
      cout << ' ' << expresion << "\n";
      writeOut(argv);
      //graphOutput(current - start, g, highFitness, expresion);
      exitOut(argv, fit);
      exit(1);
    }
    
  }
//cout << " end calcfit\n";
  return 0;  
}

//-------------------------------------------------------------

void geneJump(char *strIn, char *argv[])
{
  char *strOut;
  int x, y;

//cout << "\nrun geneJump\n";
  strOut = strchr(strIn,'F'); // test for F in string
  if (strOut) return; // if has F then return

  strOut = strchr(strIn,'k'); // test for k in string
  if (strOut) return; // if has k then return

  if (!(strOut = new char[strlen(strIn)+1])) {
//    printf("Insufficient RAM for Gene Jump Data");
    cout << "ERROR out of RAM\n";
    exit(9);
//    return;
  } // made and tested array for swap
  y = 0;
  x = 0;
  while (strIn[x] != '@') x++; // set index x to @ in string
  x++; // move past @
  while (strIn[x] != 0) { // copy from @ to end of string
    strOut[y] = strIn[x];
    x++;
    y++;
  } // last half now first
  strOut[y] = '@'; // put in @
  y++; // and move up

  x = 0;
  while (strIn[x] != '@') { // copy from start to @ of string
    strOut[y] = strIn[x];
    x++;
    y++;
  } // last half now first
  strOut[y] = 0; // end with nul
  //cout << endl << strOut << endl;
//while (!kbhit()) {}

//cout << "\n->" << strOut << "<-\n" << endl;
//  calcfit(strOut, argv, scaleFitness);
  calcfit(strOut, argv);
  delete strOut;
//cout << "\nend geneJump" << endl;
}

//-------------------------------------------------------------
//-------------------------------------------------------------
void spliceOutGene(char *strPtr, int minGeneSizeOut, int maxGeneSizeOut, char *argv[])  // 
{
  int zero_or_one;
  int place = 0;
  int end = 0;
  int startSeg = 0;
  int endSeg = 0;
  int index;
  char strArray[Max_Str_size]; 
  char terminalGene[Max_Str_size];
  char terminalBranch[Max_Str_size];
  int geneIndex = 0;
  int x,y;

  // Pick the main function or the ADF for splicing out a gene segment
  x = 0;
  while ((end - place < minGeneSizeOut) || (end - place > maxGeneSizeOut)) {
    x++;
    if (x > 10) return;
    zero_or_one = gp_rand() % 2; // two choices  0 or 1
    if (zero_or_one == 0) getIndex('@', strPtr, place, end);
    else getIndex('\0', strPtr, place, end);
    // Pick ADF for splicing out a gene segment
  }

  // If it starts at the beginning or ends at the end of the string then don't use it just return
  if (place == 0 || place == end) return;

  // Build branch gene segment
  x = 0; // x is the positional index
  y = place;
  terminalBranch[x++] = '@';
  while (y < end) {   // load 
    terminalBranch[x++] = strPtr[y++];
  }
  terminalBranch[x] = 0;
//cout << "\n->" << terminalBranch << "<-terminalBranch\n";

//  while (strlen(terminalBranch) -1 == strlen() {
  getIndex('\0', terminalBranch, startSeg, endSeg);
  while (startSeg == 1) {
    getIndex('\0', terminalBranch, startSeg, endSeg);
  }
//exit(1);
  x = 0; // x is the positional index
  y = startSeg;
  while (y < endSeg) {   // load 
    terminalGene[x++] = terminalBranch[y++];
  }
  terminalGene[x] = 0;
//cout << "\n->" << terminalGene << "<-terminalGene\n";

  x = 0; // x is the positional index 
  while (x < place) {   // load the head into strArray
    strArray[x] = strPtr[x];
    x++;
  }
  y = 0;
  while (terminalGene[y] != 0) {   // 
    strArray[x] = terminalGene[y];
    x++;
    y++;
  }
  y = end;
  while (strPtr[y] != 0) {   // 
    strArray[x] = strPtr[y];
    x++;
    y++;
  }
  strArray[x] = 0;

//cout << "\n->" << strPtr << "<-original\n";
//cout << strArray << "<-new\n";
  // From the gene segment to remove get the first terminal gene
//exit(1);
  // Replace this gene segment branch with first terminal gene
//cout << "start calcfit\n";
  calcfit(strArray, argv);
//cout << "\ncalcfit done\n";

  return;
}
//-------------------------------------------------------------
void introns(char *Astring, char *argv[])  // adds introns to string
{
  float number;
  int dec, sign, ndig = 4;
  int i, x, count;
  int holdFit;
  char zeros[6];
  char *floatNum;
  char floats[5];
  int zero_or_one;
  char numberStr[10];
  int place = 0;
  int end = 0;
  char strArray[Max_Str_size]; 

//  cout << " Intron start  " << "\n";

  zero_or_one = gp_rand() % 2; // two choices  0 or 1
  if (zero_or_one == 0) getIndex('@', Astring, place, end);
  else getIndex('\0', Astring, place, end);

  if (place == 0 || place == end) return;
  
  number = gp_rand() % 10000;
//  while (number > 1) {
    number = number/10000;
//    number = number/10;
//  }
  if (number == 0)          // float 0 ?
    number = .0001;

    //floats = fcvt(number, ndig, &dec, &sign);
  floatNum = fcvt(number, ndig, &dec, &sign);
  strcpy(floats, floatNum);
//  strcpy(floats, " ");
//printf("\nnumber-> %f dec -> %d floatNum %s\n",number, dec, floatNum);
//cout << floatNum << "\n";

    //floats[++y] = '\0';
  zeros[0] = 0; // clear string
  i = 0;
  while (dec < 0) {     // add zeros to front of number if needed
    zeros[i] = '0';
    i++;
    dec++;
  }
  zeros[i] = 0; // end with nul

  numberStr[0] = 0;        // set string to nul
  zero_or_one = gp_rand() % 2; // two choices  0 or 1
  if (zero_or_one == 0) { strcpy(numberStr, " -."); }// put -decimal point in array
  else { strcpy(numberStr, " ."); } // put +decimal point in array
  strcat(numberStr, zeros); // then zeros
  strcat(numberStr, floats);  // and cat the number
  x = 6;
  while (numberStr[x] == '0') {  // remove zeros from end of number
    x--;
  }
//  strcat(numberStr, " ");  // and cat the space 
  numberStr[++x] = '\0';
  zero_or_one = gp_rand() % 2; // two choices  0 or 1 now use for intron type

  x = 0; // x is the positional index for the strArray string
  while (x < place) {   // load the head into strArray
    strArray[x] = Astring[x];
    x++;
  }
  /*  load the first part of the intron */
  if (strArray[x -1] != ')' || strArray[x -1] != ' ') {
    strArray[x] = ' ';
    x++;
  }
  strArray[x] = '(';
  x++;
  if (zero_or_one == 0) strArray[x] = '*';
  else strArray[x] = '+';
  x++;

  strArray[x] = ' ';  // just a space
  x++;

  strArray[x] = '(';  // makes "(
  x++;
  if (zero_or_one == 0) strArray[x] = '/';  // makes "(*(/"
  else strArray[x] = '-';          // else "(+(-"
  x++;

  /*  load the number and / part of the intron (? (? .123 .123) */
  i = 0;
  while (numberStr[i] != 0) {  // load the number first time
    strArray[x] = numberStr[i];
    i++;
    x++;
  }

  i = 0;
  while (numberStr[i] != 0) {   // load the number last part
    strArray[x] = numberStr[i];
    i++;
    x++;
  }
  strArray[x] = ')'; // close paren ')'
  x++;

  count = place; // use count for index
  while (count < end) {  // load the Bmiddle
    strArray[x] = Astring[count];
    count++;
    x++;
  }

  strArray[x] = ')'; // close paren ')'
  x++;

  /////////////
  count = end; // count can again be used for an index
  while (Astring[count] != 0) {
    strArray[x] = Astring[count];
    x++;                // load the tail into strArray
    count++;
  }
  strArray[x] = 0; // end with null
//cout << strArray;

  /* precondition is that current points to this data node */

//cout << "\n->" << strArray << "<-\n" << endl;
//  calcfit(strArray, argv, scaleFitness);
  calcfit(strArray, argv);
//cout << "\npast calcfit for intron" << endl;

  /* replace old string in current node with longer one */
//move(1, 4); // blot the exp
//cout << " Intron  end  ";
}
//-------------------------------------------------------------

void getIndex(char stopChar, char *Astring, int &Aplace, int &Aend)
{
  int randA;
  int openParenA = 0;
  int leafCountA = 0;
  int closeParen;
  //int x, y;
  int count;
  int Astart;
  char *ptr;  // pointer to the start of the string

//cout << "getindex" << endl;

  if (stopChar == '@') {
    Astart = 0;          // cross the main function
  }
  else {  // else start at '@' and end at nul
    ptr = strchr(Astring,'@'); // user strchr to find the @
    Astart = ptr-Astring; // start is position of @ +1
  }

  Aplace = Astart; // here Aplace is an index
  while (Astring[Aplace] != stopChar) { // till at stopChar
    if (Astring[Aplace] == '(') {
      openParenA++;  // count number of open paren in string
    }
    Aplace++;    // move and test the next char
  }
  Aplace = Astart; // here Aplace is an index
  while (Astring[Aplace] != stopChar) { // till at stopChar
    if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' || Astring[Aplace] == 'k') {
      leafCountA++;  // count number of leaves in string
    }
    Aplace++;    // move and test the next char
  }

    randA = (gp_rand() % (openParenA+leafCountA) +1);

    if (randA <= openParenA) {  // use branch
      Aplace = Astart; // here Aplace becomes the position
      count = 0;
      //randA = (gp_rand() % (openParenA-1)) +1;
      while (count < randA) {
	if (Astring[Aplace] == '(') {
	  count++;  // count the open paren till = randA
        }
	Aplace++;  // Aplace becomes the position for this randa open
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
	Aend++;  // Aend is now the index for the close paren
      }          // Aplace is the index for the start
      Aplace--; // backup to get '('
      if (Astring[Aplace -1] == 'F') {
        Aplace--; // backup to get 'F'
      }  
      if (Astring[Aplace -1] == ' ') {
        Aplace--; // backup to get ' ' (space)
      }  
    }
    else {  // it is a leaf randA > openParenA
      randA = (randA-openParenA);
      //randA = (gp_rand() % (leafCountA-1)) +1;
      Aplace = Astart; // here Aplace becomes the position
      count = 0;
      while (count < randA) {
	Aplace++;
	if (Astring[Aplace] == '.' || Astring[Aplace] == 'X' || Astring[Aplace] == 'k')
	//if (Astring[Aplace] == '.' || Astring[Aplace] == 'X')
	  count++;  // count the leaves till = randA
      }
    //test for sign on leaf
      if (Astring[Aplace-1] == '-') {
	Aplace--; // if negative backup one
      }
      Aend = Aplace;
      while (Astring[Aend] != ')' && Astring[Aend] != ' ' && Astring[Aend] != '(' && Astring[Aend] != stopChar && Astring[Aend] != 0) {
	Aend++;  // Aend is now the position for end of leaf
      }
      Aplace--; // and backup one to pickup space on front end
    }
}

//-------------------------------------------------------------
//-------------------------------------------------------------

void kill(char *argv[]) // kill the weekest random 3
{
  int k,i,j,x;

//pop.sort(); // sort the list high first
//pop.last();   // set current to first one in list
//pop.first();   // set current to first one in list
//pop.remove(); // remove the node that current points to

  for (x = 0; x < 3; x++) {
    k = pop.getMin();
    i = pop.getMinCount(k);
    //if (i == 0) cout <<"      i = zero ";
//i is now the count of the number of lowest fitness value
    j = gp_rand() % i; // makes j the rand pick of lowest fitness value
    pop.first();   // set current to first one in list
    if (j == 0 && pop.getFit() == k) pop.remove(); // remove 
    else {
      //pop.first();   // set current to first one in list
      if (pop.getFit() == k) i = 1;   // getfitness of current node
      else i = 0;
      while (i < j) {
	if (pop.next() == 1) error("no pop; end of list", argv);  // set current to next one in list
	if (pop.getFit() == k) i++;   // getfitness of current node
      }
      pop.remove(); // remove the node that current points to
    }
  }

}

//-------------------------------------------------------------
//-------------------------------------------------------------

long double recalcF(char expresion[], int &i, int x, int &divBy0, int &overSize, char *argv[])
//void recalcF(char expresion[], int &i, int x, int &divBy0)
{   // (* (- X(* X(+ X 0.888)) (+ X F(- X 0.999)))@(- k X)
// first find the end of the function by counting open and closed
// paren F(* 2(F(/ 7 -X)))   or just use the last closing paren )?
  //int i = 2; // move past the F and first '('
  //char *Fptr;  // pointer to start of sub function F
  char *atptr;  // @ptr
  char *kptr;
  long double temp1 = 0;
  int index;
  int countParen = -1;
  char *F;

//cout << endl << "recalc function" << endl << expresion << endl;

  atptr = strchr(expresion,'@'); // set pointer to @
  // next look for the k in the @ string
  kptr = strchr(atptr,'k');
  F = strchr(atptr,'F');

  if (F) {
    cout << "\n" << atptr << "\n";
    exit(9);
  }

  i++; // first move past the F
  i++; // then past the (
  while (countParen < 0) {
    if (expresion[i] == '(') countParen--;
    if (expresion[i] == ')') countParen++;
    if (kptr) { // if it has k then calc F(.....) else just run it up
      switchCase(expresion, i, x, temp1, divBy0, overSize, argv); // x is the index for x
    }
    else i++; // ignore F(....) just index past
  }
//  if (kptr) num.pop<long double>(temp1);   // pop the value for k
//  if (kptr) num.pop<double>(temp1);   // pop the value for k
  if (kptr) num.pop(temp1);   // pop the value for k
  index = 1; // past the @
if (atptr[index] == '@') { 
cout << atptr << "\n";
error("problem here", argv); 
}
  while (atptr[index] != 0) { //till end of string
			      // temp1 is sent as k value
    switchCase(atptr, index, x, temp1, divBy0, overSize, argv); // x is the index for x
  }
  // send the answer as k
  // can it be used in a recursion ?
  //while still in function loop
  // return the poped result
//  num.pop<long double>(temp1);   // pop the answer
//  num.pop<double>(temp1);   // pop the answer
  num.pop(temp1);   // pop the answer

  return temp1;
}
//-------------------------------------------------------------

void switchCase(char expresion[], int &i, int x, long double k, int &divBy0, int &overSize, char *argv[])
{
  long double temp1;
  long double temp2;
//  long double temp3;
  char tempchar;
  char float_str[9];  // to hold floating point number as string
  int sign = 1;  // 1 or -1
  int j;
  int tooLarge = 10000000;
  double tooSmall = 0.0000001;


//cout << "->" << expresion << "<-\n";
//cout << " start sub\n";


	switch(expresion[i]) {
	    case 'F': if (num.push(recalcF(expresion, i, x, divBy0, overSize, argv))) {} // the value is now in the stack
		      else { error("expresion F num Stack full", argv); }
		      break;

	    case '+':
	    case '*':
	    case '/': if (op.push(expresion[i])) {}
		      else {
			cout << "g = " << g << "\n";
                        cout << expresion << "\n";
			error("expresion Op Stack full", argv);
		      }
		      i++;
		      break;
	    case '-': if (expresion[i-1] == '(') { // then it is an op
			if (op.push(expresion[i])) {}
			else { 
			  cout << expresion << "\n";
			  error("expresion Op Stack full", argv);
			}
			i++;
		      }
		      else { // it is a negative sign
			sign = -1;   // set sign to negative
			i++;         // so go past the negative sign
			if (expresion[i] == '.') { // if number
	    case '.':	  j = 0;
			  while (expresion[i] != ' ' && expresion[i] != ')' && expresion[i] != '@' && expresion[i] != 0) {
			    float_str[j] = expresion[i];
			    i++;
			    j++;
			  }
			  float_str[j] = '\0'; // end with null
			  if (num.push(sign * atof(float_str))) {}
			  else { error("expresion num at float Stack full", argv); }
			}
			else { // else not number is k or X or F
			  if (expresion[i] == 'k') { // if X
	    case 'k':       if (num.push(sign * k)) {} //load X from array into stack
			    else { error("expresion k num Stack full", argv); }
			  }
			  if (expresion[i] == 'X') { // if X
	    case 'X':       if (num.push(sign * arrayXY.getX(x))) {} //load X from array into stack
			    else { error("expresion X num Stack full", argv); }
			  }
			  i++; // past the X or F or k
			  sign = 1;   // reset used or not

			}
		      }
		      break;
	    case '(': // open parenthesis
	    case ' ': // space
		      i++;
		      break;
	    case ')': num.pop(temp1);  // pop top of stack last on first off
		      num.pop(temp2);
		      op.pop(tempchar);
		      switch (tempchar) {
			case '*': //cout << temp2 << "*" << temp1 << "\n";
//                                  temp3 = (temp2 * temp1);
//  cout << temp3 << " = " << temp2 << "*" << temp1 << "\n";
                                  if ((temp2 + temp1) > tooLarge) {
                                    overSize++;
                                    if (num.push(temp2 * temp1/2)) {}
				    else { error("expresion Num Stack full", argv); }
                                  }
                                  else {
                                    if (num.push(temp2 * temp1)) {}
				    else { error("expresion Num Stack full", argv); }
                                  }
				  break;
			case '/': if (temp1 == 0 || temp2 == 0 || temp1 < tooSmall) {
                                    if (num.push(1.0)) {}
				    else { error("expresion Num Stack full", argv); }
                                    if (temp1 == 0 || temp2 == 0) {
				      divBy0++;
                                    }
                                    else { //(temp1 < tooSmall ) 
                                      overSize++;
                                    }
				  }
				  else {
//  cout << temp2 << "/" << temp1 << "\n";
//                                    temp3 = (temp2 / temp1);
//  cout << temp3 << " = " << temp2 << "/" temp1 << \n";
				    if (num.push(temp2 / temp1)) {}
				    else { error("expresion Num Stack full", argv); }
				  }
				  break;
//			case '+': if (num.push(temp2 + temp1)) {}
			case '+': //cout << temp2 << "+" << temp1 << "\n";
                                  if (num.push(temp2 + temp1)) {}
				  else { error("expresion Num Stack full", argv); }
				  break;
			case '-': if (num.push(temp2 - temp1)) {}
				  else { error("expresion Num Stack full", argv); }
				  break;
		      }
		      i++; // go past the ')'
		      break;
	}
//cout << " end sub\n";
}

//-------------------------------------------------------------

void gp_srand( unsigned long long seed)
{
    unsigned int  j;

// seed the first routine........................
    RandIx1 = (IC1 + seed) % M1;
//... which is then used to seed the second.....
    RandIx1 = (IA1 * RandIx1 + IC1) % M1;
    RandIx2 = RandIx1 % M2;
// .. and third routines....
    RandIx1 = (IA1 * RandIx1 + IC1) % M1;
    RandIx3 = RandIx1 % M3;
// Fill the random variable table with sequential uniform values
    for (j = 1; j <= 97; j++)
    {
// Deviates generated by the first two routines.............................
	RandIx1 = (IA1 * RandIx1 + IC1) % M1;
	RandIx2 = (IA2 * RandIx2 + IC2) % M2;
// Low and high order pieces combined here to create value.....
	TrandArray[j] = (RandIx1 + RandIx2 * RM2) * RM1;
    }
}

// returns an unsigned long long
unsigned long long gp_rand(void)
{
    double temp;
    unsigned int j;

//  Except when initializing, this is where we start.  Generate the next number
// for each sequence.
    RandIx1 = (IA1 * RandIx1 + IC1) % M1;
    RandIx2 = (IA2 * RandIx2 + IC2) % M2;
    RandIx3 = (IA3 * RandIx3 + IC3) % M3;

// Use the third sequence to get an integer between 1 and 97
    j = 1 + ((97 * RandIx3) / M3);
    if (j > 97 || j < 1) cout <<"gplgp_rand() ??";

// Return that table entry, and refill it.
    temp = TrandArray[j] * M3;
    TrandArray[j] = (RandIx1 + RandIx2 * RM2) * RM1;

   return (unsigned long long)temp;
}

//-------------------------------------------------------------

void error(char *msg, char *argv[])
{
  //move(10, 20);
  writeOut(argv); // dump data to file
  cout << "ERROR: " << msg << "." << "\n";
  exit(9);
}
//------------------------------------------------------------

void exitOut(char *argv[], int fit)
{
  FILE *stream;
  char fileName[30];

//  ptr1 = strrchr(argv[0],'//'); // find last slash in path string
//cout << "here is the file name ->" << ptr1 << endl;
//cout << "here is argv[0] ->" << argv[0] << endl;

  fileName[0] = 0;
  strcpy(fileName, argv[23]);
  strcat(fileName, ".results.dat");

  stream = fopen(fileName, "a" ); // append to file
  fprintf(stream, "%3d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",fit, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12], argv[13], argv[14], argv[15], argv[16], argv[17], argv[18], argv[19], argv[20], argv[21], argv[22], argv[23], argv[24]);

  fclose(stream);
}
//-------------------------------------------------------------

void writeOut(char *argv[])
{
  FILE *stream;
  char fileName[30];

  fileName[0] = 0;
  strcpy(fileName, argv[23]);
  strcat(fileName, ".pop.log");
  
  stream = fopen(fileName, "a" ); // append to file
  pop.first(); // set to first in list
  do {
    fprintf(stream, "%3d,%s\n",pop.getFit(), pop.giveData());
  } while (pop.next() == 0); // till end of list

  fclose(stream);
}

//---------------------------------------------------------
//#################################################################################################
void buildString(char side, char boolein)
{
  char function[60];
  int x;
//  char *copyPtr;
  char opArray[2];
  char numberStr[7];

  function[0] = 0;    // reset string to clear it
  strcpy(function, "("); // start out with open "("
//  strcat(function, p.operators()); // add the op
//  p.operators(copyPtr);
  strcat(function, p.operators(opArray)); // add the op

//  x = (p.gp_rand() % 4); // makes 0 to 3
  x = (gp_rand() % 4); // makes 0 to 3
  //printf(" %d",x);
  if (x == 0) {  // if 0 then 2 leaves
    leaf(boolein);        // make new random leaf and put in leafArray string
    strcat(function, leafArray);  // cat the leaf into function array
    leaf(boolein);        // make new random leaf and put in leafArray string
    strcat(function, leafArray);  // cat the leaf into function array
  }
  if (x == 1) {   // if 1 then single on left, leaf on right
    strcat(function, p.numberStr(boolein, numberStr));
    leaf(boolein);        // make new random leaf and put in leafArray string
    strcat(function, leafArray);  // cat the leaf into function array
  }
  if (x == 2) {   // if 2 then  leaf left single on right
    leaf(boolein);        // make new random leaf and put in leafArray string
    strcat(function, leafArray);  // cat the leaf into function array
    strcat(function, p.numberStr(boolein, numberStr));
  }
  if (x == 3) {   // if 3 then  only one leaf
    strcat(function, p.numberStr(boolein, numberStr));
    strcat(function, p.numberStr(boolein, numberStr));
  }
  strcat(function, ")");
  //printf("Function = %s\n", function);
  if (side == 'l')
    strcpy(ArrayLeft,function);
  else
    strcpy(ArrayRight,function);

}

//#################################################################################################
//-----------------------------------------------------------------
void leaf(char boolein)
{
//  char *copyPtr;
  char opArray[2];
  char numberStr[7];

  leafArray[0] = 0;       // clear the array
//  if (p.gp_rand() % 3 == 0 && boolein == 'y') // y = yes to F( no to k
  if (gp_rand() % 3 == 0 && boolein == 'y') // y = yes to F( no to k
    strcpy(leafArray, " F(");
  else
    strcpy(leafArray, " (");

//  p.operators(copyPtr); // add the op
  strcat(leafArray, p.operators(opArray));
//  strcat(leafArray, copyPtr);
//  cout << "should only be F(* --->"<< leafArray << endl;
  strcat(leafArray, p.numberStr(boolein, numberStr));
  strcat(leafArray, p.numberStr(boolein, numberStr));
  strcat(leafArray, ")");

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#######################################################################################################

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

/*      extra tools
  while (!kbhit()) {}

*/
