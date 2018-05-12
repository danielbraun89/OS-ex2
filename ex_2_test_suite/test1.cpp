/**********************************************
 * Test 1: correct threads ids
 * changed by daniel in 12.5.2018
 **********************************************/

#include <cstdio>
#include <cstdlib>
#include "uthreads.h"

#define GRN "\e[32m"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

void halt()
{
    while (true)
    {}
}

void wait_next_quantum()
{
    int quantum = uthread_get_quantums(uthread_get_tid());
    while (uthread_get_quantums(uthread_get_tid()) == quantum)
    {}
    return;
}

void thread1()
{
    uthread_block(uthread_get_tid());
}

void thread2()
{
    halt();
}

void error()
{
    printf(RED "ERROR - wrong id returned?\n" RESET);
    exit(1);
}

int main()
{
    printf(GRN "Test 1:    \n" RESET);
    fflush(stdout);

    uthread_init(10);
//    int a = uthread_spawn(thread1);
//    printf(GRN "id: %d   ", a);
//    fflush(stdout);
//    a = uthread_spawn(thread1);
//    printf(GRN "id: %d   ", a);
//    fflush(stdout);
    if (int a = uthread_spawn(thread1) != 1)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    if (int a = uthread_spawn(thread2) != 2)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    if (int a = uthread_spawn(thread2) != 3)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    if (int a = uthread_spawn(thread1) != 4)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    if (int a = uthread_spawn(thread2) != 5)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    if (int a = uthread_spawn(thread1) != 6)
    {
        printf(GRN "id: %d   \n", a);
        error();
    }

    printf(GRN "sup\n");

    uthread_terminate(5);
    printf(GRN "sup1.5\n");

    if (uthread_spawn(thread1) != 5)
        error();

    printf(GRN "sup2\n");

    wait_next_quantum();
    wait_next_quantum();

    printf(GRN "sup3\n");
    fflush(stdout);
    uthread_terminate(5);
    if (uthread_spawn(thread1) != 5)
        error();

    uthread_terminate(2);
    if (uthread_spawn(thread2) != 2)
        error();

    uthread_terminate(3);
    uthread_terminate(4);
    if (uthread_spawn(thread2) != 3)
        error();
    if (uthread_spawn(thread1) != 4)
        error();

    printf(GRN "SUCCESS\n" RESET);
    uthread_terminate(0);

}
