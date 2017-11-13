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
#include "Queue.h"

#define PENDSV_R        (*((volatile unsigned long *) 0xE000ED04))
#define PENDSV_INVOKE   0x10000000


struct mailbox mailboxes[MAX_MSG_QUEUE]; /* List of message queues */
struct UART_reqs UART_list;

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
*             get the process's id from the running PCB
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
    struct mailbox *dst_mail = &mailboxes[req->id];
    int max_sz = req->sz;       /*maximum size to be copied*/
    /*IF sender does not own a mailbox OR destination mailbox not found*/
    if (running[current_priority]->mailbox_num == UNBOUND_Q ||
            dst_mail->process == NULL)
        /*THEN EXIT with error code*/
        return FAIL;

    /*IF receiving process is blocked*/
    if (dst_mail->buffer_addr != NULL)
    {
        /* max_sz is either size of sender's msg or receiver's buffer
         * whichever is smaller */
        max_sz = (dst_mail->sz > req->sz) ? req->sz : dst_mail->sz;
        /*so that the receiver knows the number of bytes copied*/
        *(dst_mail ->buffer_size) = max_sz;
        /*THEN give source id to receiver*/
        *(dst_mail->src_id) = running[current_priority]->mailbox_num;
        /*copy message into receiver buffer until it's full or message is complete*/
        for (i=0; i<max_sz; i++)
            dst_mail->buffer_addr[i] = req->msg[i];

        dst_mail->buffer_addr[--i] = '\0';
        /*Unblock receiver by inserting its PCB into WTR queue*/

        running[current_priority]->sp = get_PSP();
        enqueue_pcb(dst_mail->process);

        set_PSP(running[current_priority] -> sp);

        /*put a null in the buffer_addr in mailbox to signify that process is not blocked*/
        dst_mail->buffer_addr = NULL;
    }
    else /* if process not blocked */
    {
        /*put message, its size, and source id into the receive mailbox*/
        enqueue_msg(req);
    }

    /*EXIT with the number of copied bytes*/
    return max_sz;
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
int krecv(struct msg_request *req)
{
    if (running[current_priority]->mailbox_num == UNBOUND_Q)
        return FALSE;
    else
    {
        if (mailboxes[running[current_priority]->mailbox_num].cnt == 0)
        {
            mailboxes[running[current_priority]->mailbox_num].src_id = &(req->id);
            mailboxes[running[current_priority]->mailbox_num].sz = req->sz;
            mailboxes[running[current_priority]->mailbox_num].buffer_size = &(req->sz);
            mailboxes[running[current_priority]->mailbox_num].buffer_addr = req->msg;
            running[current_priority]->sp = get_PSP();
            dequeue_pcb();
            set_PSP(running[current_priority] -> sp);

            (*req).sz = mailboxes[running[current_priority]->mailbox_num].sz;
        }
        else
        {
            dequeue_msg(req);
        }

    }
    return TRUE;
}



/*******************************************************************************
* Purpose:
*             This function context switches the current process out.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void ksleep(void)
{
    PENDSV_R |= PENDSV_INVOKE;
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


