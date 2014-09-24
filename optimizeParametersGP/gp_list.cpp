
// c++ libs
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <ios>
#include <fstream>
#include <iomanip>

// c libs
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

// local libs
#include "gp_list.h"

using namespace std;

gp_list::~gp_list() {
    char *data;
    current = head;
    while (current != 0) {
        head = current->next;
        data = current->data;
        delete data;
        delete current;
        current = head;
    }
}

void gp_list::sort() {
    int length = 0;
    gp_node *n1;
    gp_node *n2;
    gp_node *n3;
    int pass;

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

        for(current=head; current->next->next!=0; current=current->next) {

            if (current->next->fitness < current->next->next->fitness) {
                n1 = current;
                n2 = current->next;        // setup
                n3 = current->next->next;
                n2->next = n3->next;
                n1->next = n3;             // and swap
                n3->next = n2;
            }
        }
    }
}

int gp_list::add(char *d, int f) {
    gp_node *n;
    char *dataPtr;

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
        cout << "Insufficient memory for node";
        return 1;
    }

    // if first one
    if (head == 0) {
        head = n;
    } else {
        n->next = head;
        head = n;
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
        if (n->fitness < min) {
            min = n->fitness;
        }
        n=n->next;
    }

    return min;  // return the lowest fitness value
}

int gp_list::getMinCount(int min) {
    gp_node *n;
    int count = 0; // count number of min values
    n = head;      // set to first node
    while (n !=0) {
        if (n->fitness == min) count++; // count number of min fitness values
        n=n->next;
    }
    return count;  // return the count of the lowest fit
}

// deletes the node at current pointer
int gp_list::remove() {
    gp_node *n;         //  and rejoins the list
    char *data;
    // precondition: current points to the node
    if (current == head) {      // for deletion
        head = current->next;    // rejoin head around current
    }
    else {
        for (n=head; n->next!=current; n=n->next);
        // empty just run it up till node before current
        n->next = current->next; // rejoin around current
    }

    data = current->getdata();
    delete data;
    delete current;

    return 1;
}

char *gp_list::giveData() {
    return current->getdata();
}

char *gp_list::SpinString(unsigned long long rnd) {
    gp_node *n;
    char *ptr = NULL;  // pointer to the string
    unsigned long long sum = 0;

    n = head;      // set to first node

    while (n->next != NULL) { // till end of list
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

// returns the current spin pie for testing
int gp_list::getFit() {
    return current->fitness;
}

unsigned long long gp_list::getMaxSpin(void) {
    // precondition must set n to head to begain
    gp_node *n;
    unsigned long long maxSpin = 0;

    n = head; // set to first node

    // only up till zero spinner value
    while (n->next !=0) {
        maxSpin += n->fitness;
        n=n->next;
    }

    return maxSpin;
}

int gp_list::first() {
    current = head;
    return 1;
}

int gp_list::last() {
    gp_node *n;
    for(n=head; n!=0; current=n,n=n->next);
    return 1;
}

int gp_list::next() {
    int status = 0; // if 0 then next

    if (current->next != 0) {
        current = current->next;
    } else {
        status = 1; // if one then end of list, not moved
    }

    return status;
}

int gp_list::length()
{
    int i=0;
    gp_node *n;

    for(n=head; n!=0; n=n->next) {
        ++i;
    }

    return i;
}

int gp_list::changeStr(char *add, char *del) {
    char *dataPtrIn;
    char *dataPtrOut;

    if (!(dataPtrIn =  new char[strlen(add)+1])) {
        printf("Insufficient memory for Data to node");
        return 1;
    }

    strcpy(dataPtrIn, add);
    current = head;

    while (current != 0) {
        if (strcmp(current->data,del) == 0) { // find the match
            break;  // if the same string then break out of the loop
        }
        current = current->next;
    }

    dataPtrOut = current->data;
    current->data = dataPtrIn;

    delete dataPtrOut;  // remove the old
    return 0;
}
