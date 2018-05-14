//
// Created by Daniel on 12/05/2018.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <iostream>
#include "uthreads.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <setjmp.h>
#include <string>

//header code taken from demo_jmp
#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required whe#include <iostream>n using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif


class Thread
{

private:

    int id;
    int depend_on;
    sigjmp_buf env;
    int quantum_number;
    std::string state;
    char* allocated_stack;
    //char allocated_stack[STACK_SIZE];  //example stack, does not really do anything

    address_t stack_pointer;
    address_t program_counter;

public:
    /**
     * constructor
     */
    Thread(int tid, void (*f)(void)) : id(tid) ,depend_on(-1) ,quantum_number(0)
    {
        try
        {
            allocated_stack = new char[STACK_SIZE];
            stack_pointer = (address_t)allocated_stack + STACK_SIZE - sizeof(address_t);
            program_counter = (address_t)f;

            sigsetjmp(env, 1);
            (env->__jmpbuf)[JB_SP] = translate_address(stack_pointer);
            (env->__jmpbuf)[JB_PC] = translate_address(program_counter);
            sigemptyset(&env->__saved_mask);
            state = "ready";
        }
        catch (std::bad_alloc&)
        {
            std::cerr << "system error: could not create a thread object\n";
        }

    };

    /**
     * destructor
     */
    ~Thread()
    {
        delete[] allocated_stack;
    };

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

    sigjmp_buf*  get_env_p()
    {
        return &env;
    };

    std::string get_state() const
    {
        return state;
    };

    void set_state(const std::string input)
    {
        if(input == "running")
        {
            quantum_number++;
        }
        state = input;
    };

    int get_depend_on() const
    {
        return depend_on;
    };

    void set_depend_on(int input)
    {
        depend_on = input;
    };

    int get_quantum_number() const
    {
        return quantum_number;
    };

};

#endif //EX2_THREAD_H
