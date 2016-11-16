#ifndef Q_H
#define Q_H

#include<stdio.h>
#include<stdlib.h>

#include "tcb.h" 

#ifndef VERBOSE
#define VERBOSE 2
#endif

struct TCB_t *NewItem()
{
	struct TCB_t *item;	
	item=(struct TCB_t*) malloc(sizeof(struct TCB_t));
	return item;
}

void NewQueue(struct  TCB_t **headA)
{
	struct TCB_t *dummy;
	*headA=(struct TCB_t*) malloc(sizeof(struct TCB_t));	
	dummy = *headA;
	dummy->next = dummy;
	dummy->prev = dummy;
	dummy->id = 0;		
}

void AddQueue(struct TCB_t **headA, struct TCB_t *item)
{
	if(VERBOSE >=2)
	{
		printf("Adding thread %d to queue\n", item->id);
	}
	
	struct TCB_t *tempNode1;
	tempNode1 = *headA;
	 
	struct TCB_t *tempNode2;
	tempNode2=tempNode1->prev;

	item->next=tempNode1;
	item->prev=tempNode2;

	tempNode1->prev=item;
	tempNode2->next=item;
}	


void PrintQueue (struct TCB_t **headA)
{
	struct TCB_t *temp;
	temp = *headA;
	printf(" %d -> ", temp->id);
	temp = temp->next;
	while (temp->id > 0)
	{
		printf(" %d -> ", temp->id);	
		temp = temp->next;	
	}
	printf("\n");
}

struct TCB_t *DelQueue(struct TCB_t **headA)
{
	struct TCB_t *tempNode1;
	tempNode1 = *headA;
				
	if(tempNode1->next == tempNode1)
	{
		printf("Queue is Empty .. Retuning NULL\n");
		return NULL;
	}

	tempNode1 = tempNode1->next;	
	struct TCB_t *tempNode2;
	struct TCB_t *tempNode3;
	tempNode2=tempNode1->next;
	tempNode3=tempNode1->prev;
	tempNode2->prev=tempNode3;
	tempNode3->next=tempNode2;

	tempNode1->next=NULL;
	tempNode1->prev=NULL;
	if(tempNode1->id == 0)
	{
		printf("Exception: Returning dummy item \n");
		exit(0);
	}	
	return tempNode1;
}	


#endif

