#ifndef _GP_LIST_H
#define _GP_LIST_H

#include <string.h>

class gp_node {
    friend class gp_list;
public:  //formula,   fitness,     spiner,        next pinter
    //gp_node (char *d=0, int f=0, unsigned long long s=0, gp_node *n=0)
    gp_node (char *d, int f)
    {   //if (d != 0) {
        //data = new char[strlen(d)+1];
        //if (!(data = new char[strlen(d)+1])) {
        //printf("\nInsufficient memory for Data node\n  of length %d\n", strlen(d));
        //exit (1);
        //}
        //strcpy(data,d);
        fitness = f;
        //spiner = 0;
        //} else data = d;
        data = d;
        //next = n;
        next = 0;
    };
    char *getdata() {
        return data;
    };
    int getfitness() {
        return fitness;
    };
    //unsigned long long getspiner() { return spiner; };
    gp_node *getnext() {
        return next;
    };
private:
    char *data;
    int fitness;
    //unsigned long long spiner;
    gp_node *next;
};

class gp_list {
    //friend ostream &operator<<(ostream &o, gp_list &l);
public:
    gp_list() {
        head = current = 0;
    };
    ~gp_list();
    void sort();    // sort the list high first
    int remove();       // remove the node at current
    int changeStr(char *in, char *out);  // replace old string pointer with new one
    int getMin();  // return lowest fitness value
    int getMinCount(int min);  // return lowest fitness count
    int add(char *d, int f);// insert d after current
    unsigned long long getMaxSpin();   // load the spiner data
    char *SpinString(unsigned long long rnd);  // return the spin string
    int getFit();              // return the fitness value of current
    char *giveData();    // give the current data string pointer
    int first();        // set current to first
    int last();         // set current to last node
    int next();         // set curent to current->next
    int length();       // return length of list
    int isempty();      // return 1 if list is empty
private:
    gp_node *head;
    gp_node *current;
};
#endif

//######################################################################################
//######################################################################################
//######################################################################################
