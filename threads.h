#ifndef THREADS_H
#define THREADS_H

#include "q.h"
#include "tcb.h"

int exitCounter=0;

struct TCB_t *ReadyQ;
struct TCB_t *Curr_Thread;

int threadCounter = 1;

#ifndef VERBOSE
#define VERBOSE 2
#endif

void start_thread(void (*function)(void))
{
	int *stack;	
	stack=(int *) malloc(8192);
	if(VERBOSE >= 1)
	{	
		printf("Starting a new thread %d .. \n", threadCounter);
	}
	struct TCB_t *tcb;
	tcb=NewItem();
	init_TCB(tcb,function,stack,8192);
	tcb->id = threadCounter;
	threadCounter ++; 			
	AddQueue(&ReadyQ,tcb);	
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
	// Doesn't come here at all. 
	while(1);
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
