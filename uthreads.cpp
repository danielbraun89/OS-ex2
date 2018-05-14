//
// Created by Daniel on 12/05/2018.
//
#include "Thread.h"
#include "uthreads.h"
#include <queue>
#include <functional>
#include <list>
#include <iostream>
#include <unordered_map>
#include <map>

#define MILLION 1000000
#define EXIT_FAILURE (-1)
#define EXIT_SUCCESS 0

std::priority_queue<int, std::vector<int>, std::greater<int> > thread_id_queue; // from the example here: http://en.cppreference.com/w/cpp/container/priority_queue

std::map<int, Thread*> thread_map;


std::list<int> ready_threads;
//std::priority_queue<Thread, std::vector<Thread>, std::greater<Thread>> ready_threads; //will pull the thread with smallest line number

std::vector<Thread*> blocked_threads; // no queue required for blocked

sigset_t singnal_set;

int total_num_of_quantums;
int length_of_quantum;

struct itimerval timer;

struct sigaction action;




/**
 * add or remove the SIGVTALRM signal
 * @param status  int: whether to block or unblock the signal
 * @return success :0  error: -1
 */
int block_SIGVTALRM(int status){
    //create an empty signal set
    int ret = sigemptyset(&singnal_set);
    if(ret == -1)
    {
        std::cerr<<"problem with creating empty set"<<std::endl;
        return -1;
    }

    ret = sigaddset(&singnal_set,SIGVTALRM);

    if(ret == -1)
    {
        std::cerr<<"problem with adding SIGVTALRM to the signal set"<<std::endl;
        return -1;
    }

    ret = sigprocmask(status, &singnal_set, NULL);
    if(ret == -1)
    {
        std::cerr<<"problem with blocking or unblocking the signal"<<std::endl;
        return -1;
    }
    return 0;
}

//just so set_timer would recognize it
void delete_all();

/**
 * set the timer so it will signal SIGVTALRM after usecs micro seconds
 * @param usecs in micro seconds
 */
void set_timer(int usecs) {

    timer.it_value.tv_sec = usecs/MILLION;
    timer.it_value.tv_usec = usecs%MILLION;

    timer.it_interval.tv_sec = usecs/MILLION;
    timer.it_interval.tv_usec = usecs%MILLION;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
        delete_all();
        std::cerr<<"thread library error: timer set error."<<std::endl;
        exit(1);
    }
}



/**
 * @brief releasing the assigned library memory
 */
void delete_all()
{
    set_timer(0);
    for (int i = 1; i < MAX_THREAD_NUM; i++)
    {
        delete thread_map[i];
    }
}


/**
 * change dependency of the threads dependent on tid, to not dependent
 * (do not change their block status if exists)
 */
void release_dependency(int tid)
{
    for ( auto it = thread_map.begin(); it != thread_map.end(); ++it  )
    {
        //release the dependency
        if (it->second->get_depend_on() == tid)
        {
            it->second->set_depend_on(-1);
        }
        // if it was not blocked beforehand, we can return it to the ready list
        if (it->second->get_state() == "ready")
        {
            ready_threads.push_back(it->second->get_id());
        }
    }
}


/**TODO change the documentation
 * swtich between the current running thread and the next thread that should run, update quantoms
 * calculation and release threads that depend on the current running thread.
 */
void switch_threads(){
    set_timer(0);
    // running thread has been already been handled (either pushed back or blocked or whatever)
    if (uthread_get_tid() != EXIT_FAILURE)
    {
        int ret_val = sigsetjmp(*(thread_map[uthread_get_tid()]->get_env_p()), 1);
        if (ret_val == 7)
        {
            return;
        }
        //TODO  check if need to release dependency on here
        release_dependency(uthread_get_tid());
    }

    // jump to the next thread on the ready list
    total_num_of_quantums++;
    thread_map[ready_threads.front()]->set_state("running");
    ready_threads.pop_front();
    set_timer(length_of_quantum);
    siglongjmp(*(thread_map[uthread_get_tid()]->get_env_p()),7);
}




/**
 * in case of reciving SIGVTALRM switch the thread
 * @param signal the signal SIGVTALRM
 */
void my_timer_handler(int signal)
{
    //move the running thread the end of the ready list
    ready_threads.push_back(uthread_get_tid());
    switch_threads(); //call switch threads
}

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
    if (quantum_usecs < 1)
    {
        std::cerr<<" invalid quantum length"<<std::endl;
        return EXIT_FAILURE;
    }

    length_of_quantum = quantum_usecs;
    for (int i = 0; i <= MAX_THREAD_NUM; i++)
    {
        thread_id_queue.push(i);
    }
    sigaddset(&singnal_set, SIGVTALRM);
    uthread_spawn(NULL);
    total_num_of_quantums = 1;

    // make my_timer_handler the signal handler for SIGVTALRM.
    action.sa_handler = &my_timer_handler;
    if (sigaction(SIGVTALRM, &action,NULL) < 0)
    {
        std::cerr<<"thread library error: sigaction error."<<std::endl;
    }
    // set timer
    set_timer(length_of_quantum);
    return EXIT_SUCCESS;
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
    block_SIGVTALRM(0);
    if (thread_id_queue.empty())
    {
        std::cerr << "maximum threads exceeded\n";
        return -1;
    }
    int new_id = thread_id_queue.top();
    thread_id_queue.pop();
    Thread* new_thread;
    try
    {
        new_thread = new Thread(new_id, f);
    }
    catch (std::bad_alloc&)
    {
        std::cerr << "could not create a thread object\n";
        return -1;
    }
    thread_map[new_id] = new_thread;

    if (new_id != 0)
    {
        ready_threads.push_back(new_id);
        thread_map[new_id]->set_state("ready");

    }
    else
    {
        thread_map[new_id]->set_state("running");
    }
    block_SIGVTALRM(1);
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
    block_SIGVTALRM(0);

    if (thread_map[tid] == NULL)
    {
        std::cerr << " invalid thread id in terminate\n";
        return EXIT_FAILURE;
    }
    //look for it in the running thread

    if(0 == tid)
    {
        delete_all();
        exit(0);
    }

    thread_id_queue.push(tid);

    //look for it in the ready threads vector, if its there remove it from there
    if (thread_map[tid]->get_state()=="ready")
    {
        ready_threads.remove(tid);
    }

    int running_id = thread_map[tid]->get_state() == "running";

    release_dependency(tid);
    delete thread_map[tid];
    thread_map[tid] = NULL;


    if(tid == running_id){
        block_SIGVTALRM(1);
        switch_threads();
    }
    block_SIGVTALRM(1);
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
int uthread_block(int tid)
{
    block_SIGVTALRM(0);
    if (thread_map[tid] == NULL)
    {
        std::cerr << " invalid thread to block\n";
        return EXIT_FAILURE;
    }
    if (0 == tid)
    {
        std::cerr << "cant block the main thread\n";
        return -1;
    }

    std::string prev_state = thread_map[tid]->get_state();
    thread_map[tid]->set_state("blocked");

    if(prev_state == "running")
    {
        block_SIGVTALRM(1);
        switch_threads();
        return EXIT_SUCCESS;
    }

    if(prev_state == "ready")
    {
        ready_threads.remove(tid);
        block_SIGVTALRM(1);
        return EXIT_SUCCESS;
    }

    block_SIGVTALRM(1);
    return EXIT_SUCCESS;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state if it's not synced. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    block_SIGVTALRM(0);
    if (thread_map[tid] == NULL)
    {
        std::cerr << " invalid thread to resume\n";
        return EXIT_FAILURE;
    }

    if(thread_map[tid]->get_state() == "blocked")
    {
        thread_map[tid]->set_state("ready");
        if (thread_map[tid]->get_depend_on() == -1)
        {
            ready_threads.push_back(tid);
        }

    }
    block_SIGVTALRM(1);
    return EXIT_SUCCESS;
}


/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will terminate. It is considered an error if no thread with ID tid
 * exists, if thread tid calls this function or if the main thread (tid==0) calls this function.
 * Immediately after the RUNNING thread transitions to the BLOCKED state a scheduling decision
 * should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid)
{
    block_SIGVTALRM(0);
    if (thread_map[tid] == NULL)
    {
        std::cerr << " invalid thread to sync\n";
        return EXIT_FAILURE;
    }

    if (uthread_get_tid() == tid)
    {
        std::cerr << " a thread cant sync itself\n";
        return EXIT_FAILURE;
    }

    if (uthread_get_tid() == 0)
    {
        std::cerr << " the main thread cant sync to other threads\n";
        return EXIT_FAILURE;
    }

    //remove the thread from running and change its status to ready
    //not to blocked because we need to diffrenciate from real blocked and
    //we are not allowed to add new states...
    ready_threads.remove(uthread_get_tid());
    thread_map[tid]->set_state("ready");
    thread_map[tid]->set_depend_on(tid);

    block_SIGVTALRM(1);
    switch_threads();
    return EXIT_SUCCESS;
}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    for ( auto it = thread_map.begin(); it != thread_map.end(); ++it  )
    {
        if (it->second->get_state() == "running")
        {
            return it->second->get_id();
        }
    }

    return EXIT_FAILURE;
}


/*
 * Description: This function returns the total number of quantums since
 * the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    return total_num_of_quantums;
}


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
int uthread_get_quantums(int tid)
{
    if (thread_map[tid] == NULL)
    {
        std::cerr << " invalid thread to sync\n";
        return EXIT_FAILURE;
    }
    return thread_map[tid]->get_quantum_number();
}



