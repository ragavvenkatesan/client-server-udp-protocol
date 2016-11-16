#ifndef SEM_H
#define SEM_H

#include "threads.h"

#ifndef VERBOSE
#define VERBOSE 2
#endif

struct Semaphore_t
{
	int sem;
	struct TCB_t *q;
};

void InitSem(struct Semaphore_t *semA, int value)
{
	semA->sem=value;
	NewQueue(&(semA->q));	
}

void P(struct Semaphore_t *semA) // similar to run
{
	struct TCB_t *temp;
	if (VERBOSE >=2)
	{
		printf("Ready Queue: Before P by %d :", Curr_Thread->id);
		PrintQueue(&ReadyQ);
		printf("Sem Queue: Before P by %d :", Curr_Thread->id);
		PrintQueue(&(semA->q));		
	}		
	semA->sem = semA->sem- 1;	
   	if(semA->sem < 0)
	{	
		if(VERBOSE >=2 )
		{
				printf(" %d is blocked \n", Curr_Thread->id);
		}			
   		struct TCB_t *Prev_Thread;
		Prev_Thread = Curr_Thread;		
		Curr_Thread=DelQueue(&ReadyQ); // Deletes a thread				
		AddQueue(&(semA->q),Prev_Thread);
		if (VERBOSE >=2)
		{
			printf("Ready Queue: During P by %d :", Curr_Thread->id);
			PrintQueue(&ReadyQ);
			printf("Sem Queue: During P by %d :", Curr_Thread->id);
			PrintQueue(&(semA->q));		
		}		
   		swapcontext(&(Prev_Thread->context),&(Curr_Thread->context));
	}
	// some other new thread is printing this. 
	if (VERBOSE >=2)
	{
		printf("Ready Queue: After P by %d :", Curr_Thread->id);
		PrintQueue(&ReadyQ);
		printf("Sem Queue: After P by %d :", Curr_Thread->id);
		PrintQueue(&(semA->q));		
	}
}

void V(struct Semaphore_t *semA)
{
	struct TCB_t *temp;
	if (VERBOSE >=2)
	{
		printf("Ready Queue: Before V by %d :", Curr_Thread->id);
		PrintQueue(&ReadyQ);
		printf("Sem Queue: Before P by %d :", Curr_Thread->id);
		PrintQueue(&(semA->q));		
	}	
	(semA->sem)++;
	if(semA->sem < 1)
	{	
		temp=DelQueue(&(semA->q)); // Deletes a thread
		if(temp!=NULL)
		{
			if(VERBOSE >=2 )
			{
					printf(" %d Unblocking thread %d \n ", Curr_Thread->id, temp->id);
			}
			AddQueue(&ReadyQ,temp);
		}
	}
	// some other new thread is printing this. 	
	if (VERBOSE >=2)
	{
		printf("Ready Queue: After V by %d :", Curr_Thread->id);
		PrintQueue(&ReadyQ);
		printf("Sem Queue: After V by %d :", Curr_Thread->id);
		PrintQueue(&(semA->q));		
	}	
	yield();	
}

#endif