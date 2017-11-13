/*
 * process_support.c
 *
 *  Created on: Oct 24, 2017
 *      Author: AbdullahAlhadlaq
 */


#include <stdio.h>
#include <stdlib.h>
#include "process_support.h"
#include "Pcommands.h"


struct pcb  *running[PRIORITY_NUM];          /* Pointers to running process in WTR queues while accounting for priority */
int current_priority;                        /* Current priority level */
int pcb_counter;                             /* keeps track of the number of WTR processes */

void assignR7(volatile unsigned long data)
{
/* Assign 'data' to R7; since the first argument is R0, this is
* simply a MOV from R0 to R7
*/
__asm(" mov r7,r0");
}

unsigned long get_PSP(void)
{
/* Returns contents of PSP (current process stack */
__asm(" mrs     r0, psp");
__asm(" bx  lr");
return 0;   /***** Not executed -- shuts compiler up */
        /***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
/* Returns contents of MSP (main stack) */
__asm(" mrs     r0, msp");
__asm(" bx  lr");
return 0;
}

void set_PSP(volatile unsigned long ProcessStack)
{
/* set PSP to ProcessStack */
__asm(" msr psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
/* Set MSP to MainStack */
__asm(" msr msp, r0");
}

void volatile save_registers()
{
/* Save r4..r11 on process stack */
__asm(" mrs     r0,psp");
/* Store multiple, decrement before; '!' - update R0 after each store */
__asm(" stmdb   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

void volatile restore_registers()
{
/* Restore r4..r11 from stack to CPU */
__asm(" mrs r0,psp");
/* Load multiple, increment after; '!' - update R0 */
__asm(" ldmia   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

unsigned long get_SP()
{
/**** Leading space required -- for label ****/
__asm("     mov     r0,SP");
__asm(" bx  lr");
return 0;
}


int reg_proc( void (*func)(void), unsigned id, unsigned short priority)
{
    unsigned long *sp;
    struct stack_frame *regs;
    struct pcb *ptr;

    if (priority > HIGH_ )
    {
        return FAIL;
    }
    /* allocate stack memory*/
    sp = (unsigned long *)malloc(STACKSIZE);
    /* account for space for the stack frame*/
    regs = (struct stack_frame *)(sp + STACKSIZE - sizeof(struct stack_frame));
    /* allocate memory for PCB*/
    ptr = (struct pcb *)malloc(sizeof(struct pcb));

    /* Set up initial values for the registers*/
    regs->r0    = 0x00000000;
    regs->r1    = 0x11111111;
    regs->r2    = 0x22222222;
    regs->r3    = 0x33333333;
    regs->r4    = 0x44444444;
    regs->r5    = 0x55555555;
    regs->r6    = 0x66666666;
    regs->r7    = 0x77777777;
    regs->r8    = 0x88888888;
    regs->r9    = 0x99999999;
    regs->r10   = 0xAAAAAAAA;
    regs->r11   = 0xBBBBBBBB;
    regs->r12   = 0xCCCCCCCC;
    regs->lr    = (unsigned long)pterm;  /* the address of the terminate function */
    regs->pc    = (unsigned long)func;
    regs->psr  = 0x01000000;

    /* assign initial values to PCB*/
    ptr-> id    = id;
    ptr-> sp    = (unsigned long) regs;
    ptr-> priority = priority;
    ptr-> stack_addr = sp;
    ptr-> mailbox_num = -1;

    /* Connect PCB to the end of the priority queue*/
    if (running[priority] == NULL)
    {
        /* if the queue is empty, let the priority queue pointer point at this pcb.
         * Make the next and previous pointer point to the same pcb (for circular queue)
         */
        running[priority] = ptr;
        ptr -> next         = ptr;
        ptr -> prev         = ptr;
    }else {
        /* if the queue is not empty, add the current pcb to the end of the queue*/
        ptr -> next                 = running[priority];
        ptr -> prev                 = running[priority]->prev;
        running[priority] -> prev   = ptr;
        ptr -> prev-> next          = ptr;
    }

    if (current_priority < priority )
        current_priority = priority;

    return SUCCESS;

}

/*******************************************************************************
* Purpose:
*             This process inserts a PCB into its corresponding priority
*             WTR queue and adjusts the current_priority global variable
*             if needed.
* Arguments:
*             in_pcb:   process control block to be inserted
* Return :
*             SUCCESS   if successful enqueuing of the PCB occurs
*             FAIL      if the WTR queue is full
*******************************************************************************/
int enqueue_pcb (struct pcb *in_pcb)
{
    /*if the number of processes exceeds the allowable amount*/
    if (pcb_counter == MAX_PROCESS)
        /*exit with error code*/
        return FAIL;
    /*else if the number of messages does not exceed the maximum*/
    /*Connect PCB to the end of the priority queue*/
    if (running[in_pcb->priority] == NULL)
    {
        /* if the queue is empty, let the priority queue pointer point at this pcb.
         * Make the next and previous pointer point to the same pcb (for circular queue)
         */
        running[in_pcb->priority] = in_pcb;
        in_pcb -> next            = in_pcb;
        in_pcb -> prev            = in_pcb;
    }else
    {
        /* if the queue is not empty, add the current pcb to the end of the queue*/
        in_pcb -> next                      = running[in_pcb->priority];
        in_pcb -> prev                      = running[in_pcb->priority]->prev;
        running[in_pcb->priority] -> prev   = in_pcb;
        in_pcb -> prev-> next               = in_pcb;
    }
    /*if the inserted process has higher priority, then change the current priority*/
    if (current_priority < in_pcb->priority )
    {
        current_priority = in_pcb->priority;
    }

    pcb_counter++;

    return SUCCESS;
}

/*******************************************************************************
* Purpose:
*             This process removes a PCB from its corresponding priority
*             WTR queue and adjusts the current_priority global variable
*             if needed. This function does not free the PCB.
* Arguments:
*             NONE (only dequeues the running processes pcb)
* Return :
*             NONE
*******************************************************************************/
void dequeue_pcb(void)
{
    /* if the running process is not the last one in the current priority WTR queue */
    if (running[current_priority] != running[current_priority]->next)
    {
        /* remove the running process PCB from the WTR queue */
        running[current_priority]->prev->next = running[current_priority]->next;
        running[current_priority]->next->prev = running[current_priority]->prev;
        /* make the next process the running process */
        running[current_priority] = running[current_priority] -> next;
    }
    else /* if it is the last process */
    {
        running[current_priority] = NULL;
        /* decrease the priority until a process PCB is present */
        while (!running[--current_priority]);
    }
}
