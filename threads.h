#ifndef THREADS_H
#define THREADS_H

#include "q.h"
#include "tcb.h"

int exitCounter=0;

struct TCB_t *ReadyQ;
struct TCB_t *Curr_Thread;
struct TCB_t *exit_thread;

int threadCounter = 0;

#ifndef VERBOSE
#define VERBOSE 2
#endif

void start_thread(void (*function)(void))
{
	int *stack;	
	stack=(int *) malloc(8192);
	threadCounter ++; 					
	if(VERBOSE >= 1)
	{	
		printf("Starting a new thread %d .. \n", threadCounter);
	}
	struct TCB_t *tcb;
	tcb=NewItem();
	init_TCB(tcb,function,stack,8192);
	tcb->id = threadCounter;
	AddQueue(&ReadyQ,tcb);	
}

void exit_function()
{
	printf("All threads are killed. Exiting \n");		
	exit(0);
}
void arm_exit()
{
	start_thread(exit_function);
}

// will kill the current thread
void thread_exit()
{
	printf("Thread %d is exiting\n",Curr_Thread->id);
	threadCounter --;
	if(threadCounter == 0)
	{
		arm_exit();
	}
	Curr_Thread = DelQueue(&ReadyQ);
	ucontext_t dump;     // get a place to store the dump context, for faking
	getcontext(&dump);   // magic sauce :)
	swapcontext(&dump, &(Curr_Thread->context));  // start the next thread on readyQ	
}
 
void run()
{   // real code
	if(VERBOSE >= 1)
	{	
		printf("User scheduler is taking over .. \n");
	}
	if (VERBOSE >=2)
	{
		printf("Ready Queue:");
		PrintQueue(&ReadyQ);	
	}			
	Curr_Thread = DelQueue(&ReadyQ);
	ucontext_t parent;     // get a place to store the main context, for faking
	getcontext(&parent);   // magic sauce
	swapcontext(&parent, &(Curr_Thread->context));  // start the first thread	
	// Comes here only when main in reloaded, which never happes
}

void yield()
{
	if (VERBOSE >=2)
	{
		printf("Context switch .. \n");		
		printf("Ready Queue Before:");
		PrintQueue(&ReadyQ);	
	}	
		
	if(VERBOSE >= 2)
	{	

	}	
   	struct TCB_t *Prev_Thread;
   	AddQueue(&ReadyQ, Curr_Thread);   
   	Prev_Thread = Curr_Thread;
   	Curr_Thread = DelQueue(&ReadyQ);
   	swapcontext(&(Prev_Thread->context),&(Curr_Thread->context));
	if (VERBOSE >=2)
	{
		printf("Ready Queue After:");
		PrintQueue(&ReadyQ);	
	}	   
}
#endif
