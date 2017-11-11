/*
 * Kcommands.c
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */


/* kterm */

#include <stdlib.h>
#include "Kcommands.h"
#include "Pcommands.h"
#include "process_support.h"

struct mailbox mailboxes[MAX_MSG_QUEUE]; /* List of message queues */

/*******************************************************************************
* Purpose:
*             Terminate a process by removing it from the waiting to run queue
*             and deallocate all its memory.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void kterm(void)
{

    struct pcb *temp = running[current_priority];

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
        /* decrease the priority until a process PCB is present */
        while (!running[--current_priority]);
    }

    free (temp -> stack_addr);
    free (temp);

    set_PSP(running[current_priority] -> sp);

}

/*******************************************************************************
* Purpose:
*             get the process's id from the running PCCB
* Arguments:
*             NONE
* Return :
*             process id
*******************************************************************************/
int kgetid(void)
{
    return running[current_priority] -> id;
}

/*******************************************************************************
* Purpose:
*             bind a process to a queue
* Arguments:
*             num:      queue number for binding
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL     if invalid number is given by process or binding failed
*******************************************************************************/
int kbind(int num)
{
    /* IF Process is already bound to mailbox OR mailbox is already bound */
    if (running[current_priority] -> mailbox_num != UNBOUND_Q ||
            mailboxes[num].process != NULL )
        /* THEN EXIT with error */
        return FAIL;
    else
    {
        /* store desired mailbox ID in PCB */
        running[current_priority] -> mailbox_num = num;
        /* connect PCB to desired mailbox */
        mailboxes[num].process = running[current_priority];
        /* EXIT with mailbox number */
        return num;
    }
}

/*******************************************************************************
* Purpose:
*             This function takes a message request and either gives it
*             directly to the receiving process of stores it in the receiving
*             process's mailbox.
* Arguments:
*             req:      message request containing (msg, sz, dst_id)
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL      if invalid number is given by process or binding failed
*******************************************************************************/
int ksend(struct msg_request *req)
{

    int i;
    struct mailbox *dst_mail = &mailboxes[req->dst_id];
    /*maximum size to be copied*/
    int max_sz = (dst_mail->sz > req->sz) ? req->sz : dst_mail->sz;
    /*IF sender does not own a mailbox OR destination mailbox not found*/
    if (running[current_priority]->mailbox_num == UNBOUND_Q ||
            dst_mail->process == NULL)
        /*THEN EXIT with error code*/
        return FAIL;

    /*IF receiving process is blocked*/
    if (mailboxes[req->dst_id].buffer_addr != NULL)
    {
        /*THEN give source id to receiver*/
        dst_mail->src_id = running[current_priority]->mailbox_num;
        /*copy message into receiver buffer until it's full or message is complete*/
        for (i=0; i<max_sz; i++)
            dst_mail->buffer_addr[i] = req->msg[i];
        /*Unblock receiver by inserting its PCB into WTR queue*/



    }
    else /* if process not blocked */
    {
        /*put message, its size, and source id into the receive mailbox*/

        /*
         * enqueue the message
         */

    }

    /*EXIT with the number of copied bytes*/
    return max_sz;
}

/*******************************************************************************
* Purpose:
*             get the process's id from the running PCCB
* Arguments:
*             pr:       desired priority supplied by process
* Return :
*             NONE
*******************************************************************************/
void knice(int* pr)
{
    struct pcb *temp = running[current_priority];

    running[current_priority]->sp = get_PSP();

    running[current_priority] ->priority = *pr;

    if (*pr == current_priority)
        return;

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
        /* decrease the priority until a process PCB is present */
        running[current_priority] = NULL;
        while (!running[--current_priority]);
    }


    /* Connect PCB to the end of the priority queue*/
    if (running[*pr] == NULL)
    {
        /* if the queue is empty, let the priority queue pointer point at this pcb.
         * Make the next and previous pointer point to the same pcb (for circular queue)
         */
        running[*pr] = temp;
        temp -> next         = temp;
        temp -> prev         = temp;
    }else {
        /* if the queue is not empty, add the current pcb to the end of the queue*/
        temp -> next                 = running[*pr];
        temp -> prev                 = running[*pr]->prev;
        running[*pr] -> prev   = temp;
        temp -> prev-> next          = temp;
    }

    if (current_priority < *pr )
        current_priority = *pr;

    set_PSP(running[current_priority] -> sp);

}


