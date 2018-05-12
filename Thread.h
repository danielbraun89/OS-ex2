//
// Created by Daniel on 12/05/2018.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include "uthreads.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <setjmp.h>


class Thread
{

private:

    int id;
    sigjmp_buf env;
    int quantum_number;
    char allocated_stack[STACK_SIZE];  //example stack, does not really do anything

public:
    unsigned long long line_number;
    /**
     * constructor
     */
    Thread(int id) : id(id) , quantum_number(1)
    {

    };

    /**
     * destructor
     */
    ~Thread(){};

    /**
     * a comparison operator.
     * This is neccesary in order to find it (inside a vector for example)
     * @param lhs left thread
     * @param rhs right thread
     * @return true if equal
     */
    friend bool operator== (Thread const& lhs, Thread const& rhs)
    {
        return lhs.get_id() == rhs.get_id();
    };


    int get_id() const
    {
        return id;
    };
};

#endif //EX2_THREAD_H
