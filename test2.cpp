/**********************************************
 * Test 2: very simple sync check
 *
 **********************************************/

#include <cstdio>
#include <cstdlib>
#include "uthreads.h"

#define GRN "\e[32m"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

bool thread2Spawned = false;
bool thread2Executed = false;

void halt()
{
    while (true)
    {}
}

void thread1()
{
    while (!thread2Spawned)
    {}
    printf(GRN "thread 1 sync to 2    \n" RESET);
    fflush(stdout);
    uthread_sync(2);

    if (!thread2Executed)
    {
        printf(RED "ERROR - thread did not wait to sync\n" RESET);
        exit(1);
    }
    printf(GRN "SUCCESS\n" RESET);
    uthread_terminate(0);
}

void thread2()
{
    printf(GRN "thread started2    \n" RESET);
    fflush(stdout);
    thread2Executed = true;
    halt();
}

int main()
{
    printf(GRN "Test 2:    \n" RESET);
    fflush(stdout);

    uthread_init(20);
    printf(GRN "post init    \n" RESET);
    uthread_spawn(thread1);
    printf(GRN "uthread_spawn(thread1)    \n" RESET);
    uthread_spawn(thread2);
    printf(GRN "uthread_spawn(thread2)    \n" RESET);
    thread2Spawned = true;
    halt();
}
