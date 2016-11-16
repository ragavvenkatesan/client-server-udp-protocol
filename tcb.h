#ifndef TCB_H
#define TCB_H

#include <ucontext.h>
#include <string.h>

struct TCB_t 
{
     struct TCB_t     *next;
     struct TCB_t     *prev;
     ucontext_t      context;
     int id;
};

void init_TCB(struct TCB_t *tcb, void *function, void *stackP, int stack_size)
{
    memset(tcb, '\0', sizeof(struct TCB_t));       // wash, rinse
    getcontext(&tcb->context);              
    tcb->context.uc_stack.ss_sp = stackP;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    makecontext(&tcb->context, function, 0); // context is now cooked
}

#endif
