/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct   2017
 * Editing       :   25  Oct - Used code from course website to context switch
 *                   28  Oct - written nice command and get ID
 *                    1  Nov - bind command is done
 *                    9  Nov - messaging system is complete
 * Submission date : 15  Nov   2017
 * File name : Kcommands.c
 * Purpose: Implementation of kernel calls and kernel commands.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "kernel.h"
#include "Kcommands.h"
#include "Pcommands.h"
#include "processes.h"
#include "Queue.h"
#include "UART.h"
#include "SYSTICK.h"
#include "Kprocesses.h"
#include "Trainsetprotocol.h"
 
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
    
    /* IF a process is bound to a queue */
    if (temp -> mailbox_num != UNBOUND_Q)
    {
        /* free its message queue and unbind */
        mailboxes[temp -> mailbox_num].head = 0;
        mailboxes[temp -> mailbox_num].tail = 0;
        mailboxes[temp -> mailbox_num].cnt  = 0;
        mailboxes[temp -> mailbox_num].process = NULL;
    }
    
	dequeue_running_pcb();			 /* Remove calling process from WTR queue */
    free (temp -> stack_addr);	     /* Free its allocated stack memory       */
    free (temp);					 /* Free allocated memory of PCB          */
    set_PSP(running[current_priority] -> sp); /* Set PSP to next process's SP */
}

/*******************************************************************************
* Purpose:
*             get the currently running process's id from the its PCB.
* Arguments:
*             NONE
* Return :
*             running process id is returned
*******************************************************************************/
int kgetid(void)
{
    return running[current_priority] -> id;	
}

/*******************************************************************************
* Purpose:
*             bind the calling process to the specified mailbox number. IF the
*						the process is already bound to a mailbox or if the 
*						queue is already bound to a process, this command fails.
* Arguments:
*             num:      queue number for binding
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL (-1) if invalid number is given by process or binding failed
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
*             req:      message request containing (msg, sz, dst_id, src_id)
* Return :
*             max_sz    if successful sending of message
*             FAIL      if sending failed
*******************************************************************************/
int ksend(struct msg_request *req)
{
    int i;
    struct mailbox *dst_mail = &mailboxes[req->dst_id];
    int max_sz = req->sz;       /*maximum size to be copied*/
    /*IF sender does not own a mailbox OR destination mailbox not found*/
    if (req ->src_id == 0 &&
		(running[current_priority]->mailbox_num == UNBOUND_Q ||
			dst_mail->process == NULL))
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
        *(dst_mail->src_id) = (req->src_id == 0)? running[current_priority]->mailbox_num : req->src_id;
        /*copy message into receiver buffer until it's full or message is complete*/
        for (i=0; i<max_sz; i++)
            dst_mail->buffer_addr[i] = req->msg[i];
        //dst_mail->buffer_addr[--i] = '\0';

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
*             This function checks if the calling process is bound to a queue. 
*					Then, the mailbox is checked for messages. If there are no
*					messages, the calling process is blocked while it waits to 
*					for a message to arrive. When a process is blocked, it will
*					will have left the address of its buffer to store the next 
*					arriving message right before it blocks. Otherwise, this 
*					function will get the next message from its mailbox and 
*					returns it by address to the calling process.
* Arguments:
*             req:      message request containing (msg, sz, dst_id, src_id)
* Return :
*             TRUE      if successful reception of message
*             FALSE     if reception of message is unsuccessful
*******************************************************************************/
int krecv(struct msg_request *req)
{
	/*if receiving process doe not have a mailbox*/
    if (running[current_priority]->mailbox_num == UNBOUND_Q) 
        return FALSE;

	/*if it has a mailbox*/
    else 
    {
		/*if the mailbox is empty*/
        if (mailboxes[running[current_priority]->mailbox_num].cnt == 0)
        {
            /*store important information*/
            mailboxes[running[current_priority]->mailbox_num].src_id = &(req->src_id);
            mailboxes[running[current_priority]->mailbox_num].sz = req->sz;
            mailboxes[running[current_priority]->mailbox_num].buffer_size = &(req->sz);
            mailboxes[running[current_priority]->mailbox_num].buffer_addr = req->msg;

            /*block the process*/
            running[current_priority]->sp = get_PSP();
            dequeue_running_pcb();
            set_PSP(running[current_priority] -> sp);
            (*req).sz = mailboxes[running[current_priority]->mailbox_num].sz;
        }

		/*if the mailbox is not empty*/
        else
            dequeue_msg(req);
    }

    return TRUE;
}

/*******************************************************************************
* Purpose:
*             This function prints a string by putting a display request into
*             the UART list if the UART is busy or putting it directly into
*             the data register if it is idle.
* Arguments:
*             dsp_msg:  message to be displayed (string)
* Return :
*             SUCCESS   if enqueuing of the message is successful
*             FAIL      if enqueuing is not successful
*******************************************************************************/
void kdisplay(char *dsp)
{
    struct UART_entry new_entry;
    new_entry.dsp_msg = dsp;
    new_entry.proc = running[current_priority];

    if (UART_state == BUSY)
    {
        enqueue_UART(&new_entry);
    }
    else /*if UART is idle*/
    {
        UART_state = BUSY;
        current_msg.dsp_msg = new_entry.dsp_msg;
        current_msg.proc    = new_entry.proc;
        UART0_DR_R = *dsp;
        current_msg.dsp_msg++;
    }

    //block the process
    running[current_priority]->sp = get_PSP();
    dequeue_running_pcb();
    set_PSP(running[current_priority]->sp);

}
/*******************************************************************************
* Purpose:
*             This function changes the processes priority by removing the
*             PCB and placing it in the desired priority WTR queue.
* Arguments:
*             pr:       desired priority supplied by process
* Return :
*             NONE
*******************************************************************************/
void knice(int* pr)
{
	struct pcb *temp = running[current_priority];   /* running PCB */

    /* the id should be returned by pkcall */
    if (*pr > HIGH_ || *pr < LOW_ || *pr == current_priority)
        return ;

    running[current_priority]->sp = get_PSP();      /*store current PSP*/
    running[current_priority] ->priority = *pr;     /*change priority in PCB*/


    /*if the requested priority is different the current one*/

    dequeue_running_pcb();                          /*remove PCB from WTR*/
    enqueue_pcb(temp);                              /*Place PCB in new WTR*/
    set_PSP(running[current_priority] -> sp);       /*restore PSP*/
}




/*******************************************************************************
* Purpose:
*             This function changes the processes priority by removing the
*             PCB and placing it in the desired priority WTR queue.
* Arguments:
*             pr:       desired priority supplied by process
* Return :
*             NONE
*******************************************************************************/
void send_frame (struct frame * frame)
{
    if ( UART1_state == BUSY )
        enqueue_frame(frame);
        /* IF UART is idle - not transmitting any characters */
    else
    {
        UART1_state = BUSY;          // Signal UART is busy
        send.low   = frame->low;
        send.high  = frame->high;
        UART1_DR_R = frame->bytes[counter++];  // Load character into data reg.
    }
}

#define SHOULD_ESC(x)  x == DLE || x == STX || x == ETX

void xmit_frame (struct transmit *packet )
{
    struct frame frame;
    int checksum = 0;
    int i=0,j=0;
    if (packet->length > PKT_BYTE)
        return;
    frame.bytes[i++] = STX;
    for ( ; i <= packet->length ; i++)
    {
        if (SHOULD_ESC(frame.bytes[i]))
        {
            frame.bytes[i] = DLE;
            i++;
        }
        frame.bytes[i] = packet->xmit[j++];
        checksum += frame.bytes[i];
    }
    frame.bytes[i++] = ~checksum;
    frame.bytes[i] = ETX;
    frame.length = i;
    send_frame(&frame);

}
