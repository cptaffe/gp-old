#ifndef _TSTACK2_H
#define _TSTACK2_H
#define DEFSIZE             100

#include <iostream>

using namespace std;
using std::cout;

template< class T >
class Tstack2 {

private:
    int size;
    int top;
    T *contents;
public:
    Tstack2(int = 10);
    ~Tstack2();
    int push(const T&);
    int pop(T&);
    int isEmpty() const  {
        return top == -1;    // return 1 if empty
    }
    int isFull()  const {
        return top == size-1;    // return 1 if full
    }
};


template< class T > Tstack2< T >::Tstack2(int s) {
    size = s > 0 && s < 1000 ? s : 10;
    top = -1;
    if (!(contents = new T[size])) {
        std::cout << "\nOut of memory for stack";
    }
}

template<class T>
Tstack2<T>::~Tstack2() { // the destructor
    delete [] contents;
}

template< class T >
int Tstack2< T >::push(const  T &item) {
    if (!isFull())  {
        contents[++top] = item;
        return 1;
    }
    return 0;  // else return 0 if fail from full
}

template< class T >
int Tstack2< T >::pop(T &popValue) {
    if (!isEmpty()) {
        popValue = contents[top--];
        return 1;
    }
    return 0;  // else return 0
}

#endif
