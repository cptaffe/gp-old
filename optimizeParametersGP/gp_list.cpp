//#include <ASSERT.H>
#include  <stdio.h>
#include <iostream>
using namespace std;

#include "gp_list.h"
#include <cstdlib>

#include <cstdio>
#include <ios>
//#include <sunmath.h>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
//#include <curses.h>
#include<iomanip>


gp_list::~gp_list()
{
    char *data;
    current = head;
    while (current != 0) {
        //cout << current->getdata() << endl;
        head = current->next;
        data = current->data;
        delete data;
        delete current;
        current = head;
    }
    //cout << "delete data" << endl;
}

void gp_list::sort()
{
    int length = 0;
    gp_node *n1;
    gp_node *n2;
    gp_node *n3;
    int pass;
    //n = head
    for(n1=head; n1!=0; n1=n1->next) {
        ++length;
    }
    for (pass = 1; pass < length; pass++) {
        if (head->fitness < head->next->fitness) {
            n1 = head;   // if the first 2 need swaping take care of head
            n2 = head->next;
            n1->next = n2->next;
            head = n2;
            head->next = n1;
        }
        //current = head;
        //cout << endl;
        for(current=head; current->next->next!=0; current=current->next) {
            //while (current->next->next != 0) {
            if (current->next->fitness < current->next->next->fitness) {
                //cout << current->next->fitness << " < " << current->next->next->fitness << ", ";
                n1 = current;
                n2 = current->next;        // setup
                n3 = current->next->next;
                n2->next = n3->next;
                n1->next = n3;             // and swap
                n3->next = n2;
                //cout << "swap " << "n2 = " << n2->fitness << " and " << "n3 = "<< n3->fitness << endl;
            }
            //current = current->next;   // increment
        }
    }
    //n1 = 0;
    //n2 = 0;        // setup
    //n3 = 0;
    // list should now be highest fitness first
    //current = head;
    //cout << endl;
    //for(n1=head; n1!=0; n1=n1->next) {
    //cout << "  " << n1->fitness;
    //current = current->next;
    //}
    //cout << endl << length << endl;
}

int gp_list::add(char *d, int f)
{
    gp_node *n;
    char *dataPtr;
    //for(n=head; n!=0; current=n,n=n->next);
    // empty just run current to end of list
    // always add at end of list
    if (!(dataPtr =  new char[strlen(d)+1])) {
        //    move(1, 16);
        cout << "Insufficient memory for Data to node";
        return 1;
    }
    strcpy(dataPtr, d);
    if (!(n = new gp_node(dataPtr, f))) {
        delete dataPtr;  // delete because out of memory for node
        dataPtr = 0;
        //    move(1, 19);
        cout << "Insufficient memory for node";
        return 1;
        //exit (1);
    }
    if (head == 0)  // if first one
        //head = current = n;
        head = n;
    else {
        n->next = head;
        head = n;
        //current = head;
        //current->next = n; // put the new node at the end
        //current = n;
    }
    return 0;
}

int gp_list::getMin()
{
    gp_node *n;
    int min = 200;
    n = head;      // set to first node
    if (n == 0) {
        //move(1,6);
        printf("    null\n");
        exit(1);
    }
    while (n !=0 && min > 1) {
        //move(1,1);
        //printf(" Min %3d Fitness %3d", min, n->fitness);
        if (n->fitness < min) min = n->fitness; // make min the lowest fitness
        n=n->next;
    }
    return min;  // return the lowest fitness value
}

int gp_list::getMinCount(int min)
{
    gp_node *n;
    int count = 0; // count number of min values
    n = head;      // set to first node
    while (n !=0) {
        //move(1,2);
        //printf(" Fitness %3d  count %3d", n->fitness, count);
        if (n->fitness == min) count++; // count number of min fitness values
        n=n->next;
    }
    //cout << " end getMinCount" << endl;
    return count;  // return the count of the lowest fit
}

int gp_list::remove()
{   // deletes the node at current pointer
    gp_node *n;         //  and rejoins the list
    char *data;
    // precondition: current points to the node
    if (current == head) {      // for deletion
        head = current->next;    // rejoin head around current
        //data = current->data;    // postcondition: current points to head
        //current = head;
    }
    else {
        for (n=head; n->next!=current; n=n->next);
        // empty just run it up till node before current
        n->next = current->next; // rejoin around current
        //data = current->data;
        //delete data;
        //delete current;
        //current = head;
        //current = (n->next==0?n:n->next);
    }
    //current = head->next;
    data = current->getdata();
    delete data;
    delete current;
    //current = head;
    //current = 0;
    //cout <<"               Kill" << endl;
    return 1;
}

char *gp_list::giveData()
{
    return current->getdata();
}

char *gp_list::SpinString(unsigned long long rnd)
{
    gp_node *n;
    char *ptr;  // pointer to the string
    unsigned long long sum = 0;
    //move(1, 7);
    //cout << " Start SpinString...";
    n = head;      // set to first node
    //ptr = n->getdata(); // set to the first one
    while (n->next !=0) { // till end of list
        sum += n->fitness;
        if (sum >= rnd) {
            ptr = n->getdata(); // set to the current one
            break;
        }
        n=n->next;
    }
    current = n; // set current to this node
    return ptr;
}

int gp_list::getFit()
{   // returns the current spin pie for testing
    return current->fitness;
}

unsigned long long gp_list::getMaxSpin(void)
{   // precondition must set n to head to begain
    gp_node *n;
    unsigned long long maxSpin = 0;
    n = head;      // set to first node
    //while (n->next !=0 && n->next->fitness != 0) { // only up till zero spinner value
    while (n->next !=0) { // only up till zero spinner value
        maxSpin += n->fitness;
        //cout << maxSpin << " >= " << rnd << endl;
        n=n->next;
    }
    //cout << "maxSpin  ' "<< maxSpin << endl;
    return maxSpin;
}

int gp_list::first()
{
    current = head;
    return 1;
}

int gp_list::last()
{
    gp_node *n;
    for(n=head; n!=0; current=n,n=n->next);
    //empty
    return 1;
}

int gp_list::next()
{
    int status = 0; // if 0 then next
    //if (current != 0 && current->next != 0)
    if (current->next != 0)
        current = current->next;
    else status = 1; // if one then end of list, not moved
    return status;
}

int gp_list::length()
{
    int i=0;
    gp_node *n;
    for(n=head; n!=0; n=n->next)
        ++i;
    return i;
}
/*
int gp_list::isempty()
{
  return head==0;
}
*/

int gp_list::changeStr(char *add, char *del)
{
    gp_node *n;
    char *dataPtrIn;
    char *dataPtrOut;
    if (!(dataPtrIn =  new char[strlen(add)+1])) {
        //    move(1, 16);
        printf("Insufficient memory for Data to node");
        return 1;
        //exit (1);
    }
    strcpy(dataPtrIn, add);
    current = head;
    while (current != 0) {
        //clrscr();
        //printf("\n%s\ncompar\n%s\n", current->data, del);
        if (strcmp(current->data,del) == 0) { // find the match
            break;  // if the same string then break out of the loop
        }
        //current->data;
        current = current->next;
    }
    //printf("\n%s\n", add);
    //printf("\n%s\n", dataPtrIn);
    dataPtrOut = current->data;
    current->data = dataPtrIn;
    /*
      // find the old node by matching the string
      while (n !=0) {
        if (strcmp(n->getdata(),del) == 0) { // find the match
          break;  // if the same string then break out of the loop
        }
        n=n->next;
      }

      dataPtrOut = n->getdata();
      n->data = dataPtrIn;  // put in the new data
    */
    delete dataPtrOut;  // remove the old
    return 0;
}
//######################################################################################################
//######################################################################################################
//######################################################################################################
