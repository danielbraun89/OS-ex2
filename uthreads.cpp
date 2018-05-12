//
// Created by Daniel on 12/05/2018.
//
#include "Thread.h"
#include "uthreads.h"
#include <queue>
#include <functional>
#include <queue>
#include <vector>
#include <iostream>

std::priority_queue<int, std::vector<int>, std::greater<int> > thread_id_queue; // from the example here: http://en.cppreference.com/w/cpp/container/priority_queue

Thread* base_thread; //pointer to hold the base thread (thread with id 0)

Thread* running_thread; //pointer to the current running thread (there can only be one)

std::deque<Thread*> ready_threads;
//std::priority_queue<Thread, std::vector<Thread>, std::greater<Thread>> ready_threads; //will pull the thread with smallest line number

std::vector<Thread*> blocked_threads; // no queue required for blocked

int length_of_quantum;

/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    length_of_quantum = quantum_usecs;
    for (int i = 1; i <= MAX_THREAD_NUM; ++i)
    {
        thread_id_queue.push(i);
    }
    base_thread = new Thread(0);
    running_thread = base_thread;

}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void))
{
    Thread* new_thread;
    int new_id = thread_id_queue.top();
    new_thread = new Thread(new_id);
    ready_threads.push_back(new_thread);

    thread_id_queue.pop();
    return new_id;
}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    Thread* thread_to_kill_p;
    //look for it in the running thread
    if(running_thread->get_id() == tid)
    {
        thread_to_kill_p = running_thread;
    }

    //look for it in the blocked threads vector
    for(std::size_t i=0; i<blocked_threads.size(); ++i)
    {
        if (blocked_threads[i]->get_id() == tid)
        {
            thread_to_kill_p = blocked_threads[i];
            blocked_threads.erase(blocked_threads.begin() + i);
            break;
        }
    }

    //look for it in the ready threads vector
    for(std::size_t i=0; i<ready_threads.size(); ++i)
    {
        if (ready_threads[i]->get_id() == tid)
        {
            thread_to_kill_p = ready_threads[i];
            ready_threads.erase(ready_threads.begin() + i);

            break;
        }
    }
    thread_id_queue.push(tid);
    delete thread_to_kill_p;
    return 0;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid){}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state if it's not synced. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid){}


/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will terminate. It is considered an error if no thread with ID tid
 * exists, if thread tid calls this function or if the main thread (tid==0) calls this function.
 * Immediately after the RUNNING thread transitions to the BLOCKED state a scheduling decision
 * should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid){}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid(){}


/*
 * Description: This function returns the total number of quantums since
 * the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums(){}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered an error.
 * Return value: On success, return the number of quantums of the thread with ID tid.
 * 			     On failure, return -1.
*/
int uthread_get_quantums(int tid){}
