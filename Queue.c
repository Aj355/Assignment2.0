/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct  2017
 * Editing       :   15  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 15 Nov 2017
 * File name : Queue.c
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
 * ------------------------------------------------------------------------- */
#include <stdlib.h>
#include "Queue.h"
#include "UART.h"
#include "Kcommands.h"
#include "kernel.h"
#include "Trainsetprotocol.h"

struct UART_queue UQ;   /* contains list of string pointers to be printed */
struct frame_queue FQ;  /* contains list of frames to be sent */
struct packet_queue PQ; /* contains list of packets to be sent */
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
int enqueue_pcb(struct pcb *in_pcb)
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
		in_pcb->next = in_pcb;
		in_pcb->prev = in_pcb;
	}
	else
	{
		/* if the queue is not empty, add the current pcb to the end of the queue*/
		in_pcb->next = running[in_pcb->priority];
		in_pcb->prev = running[in_pcb->priority]->prev;
		running[in_pcb->priority]->prev = in_pcb;
		in_pcb->prev->next = in_pcb;
	}
	/*if the inserted process has higher priority, then change the current priority*/
	if (current_priority < in_pcb->priority)
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
void dequeue_running_pcb(void)
{
	/* IF the running process is not alone in the current priority WTR queue  */
	if (running[current_priority] != running[current_priority]->next)
	{
		/* remove the running process PCB from the WTR queue */
		running[current_priority]->prev->next = running[current_priority]->next;
		running[current_priority]->next->prev = running[current_priority]->prev;
		/* make the next process the running process */
		running[current_priority] = running[current_priority]->next;
	}
	else /* if it is the last process */
	{
		running[current_priority] = NULL;
		/* decrease the priority until a process PCB is present */
		while (!running[--current_priority]);
	}
	pcb_counter--;
}
/* -------------------------------------------------------------------------- *
 * Purpose:       Insert an entry into the UART queue
 *
 * Arguments:
 *                req:  uart entry to be enqueued
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
int enqueue_UART(struct UART_entry * req)
{
    int state;
    InterruptMasterDisable();               // Disable all interrupt
    if (UQ.cnt == MAX_UART_REQ)              // IF queue is full
        state = FALSE;
    else
    {
        state = TRUE;
        UQ.queue[UQ.head].dsp_msg = req->dsp_msg;
        UQ.queue[UQ.head].proc = req->proc;
        // Increment head to next entry
        UQ.head = (UQ.head + 1) % MAX_UART_REQ;
        // Increment Queue entry counter
        UQ.cnt++;
    }
    InterruptMasterEnable();                // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       remove an element from the UART queue
 * Arguments:
 *                req: UART entry to be dequeued
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
int dequeue_UART(struct UART_entry *req)
{
    int state;
    InterruptMasterDisable();                    // Disable all interrupt
    if (UQ.cnt > 0)                            // IF the queue is not empty
    {
        state = TRUE;
        req->dsp_msg = UQ.queue[UQ.tail].dsp_msg;
        req->proc = UQ.queue[UQ.tail].proc;
        UQ.tail = (UQ.tail + 1) % MAX_UART_REQ;        // Increment tail to entry
        UQ.cnt--;                                   // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       Insert an entry into the UART queue
 *
 * Arguments:
 *                req:  uart entry to be enqueued
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
int enqueue_packet(struct transmit * req)
{
    int state;
    InterruptMasterDisable();               // Disable all interrupt
    if (PQ.cnt == 25)              // IF queue is full
        state = FALSE;
    else
    {
        state = TRUE;
        PQ.queue[PQ.head].whole = req->whole;
        // Increment head to next entry
        PQ.head = (PQ.head + 1) % 25;
        // Increment Queue entry counter
        PQ.cnt++;
    }
    InterruptMasterEnable();                // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       remove an element from the UART queue
 * Arguments:
 *                req: UART entry to be dequeued
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
int dequeue_packet(struct transmit * req)
{
    int state;
    InterruptMasterDisable();                    // Disable all interrupt
    if (PQ.cnt > 0)                              // IF the queue is not empty
    {
        state = TRUE;
        req->whole = PQ.queue[PQ.tail].whole;
        PQ.tail = (PQ.tail + 1) % 25;   // Increment tail to entry
        PQ.cnt--;                                // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       Insert an entry into the UART queue
 *
 * Arguments:
 *                req:  uart entry to be enqueued
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
int enqueue_frame(struct frame * req)
{
    int state;
    InterruptMasterDisable();               // Disable all interrupt
    if (FQ.cnt == 25)              // IF queue is full
        state = FALSE;
    else
    {
        state = TRUE;
        FQ.queue[FQ.head].low    = req->low;
        FQ.queue[FQ.head].high   = req->high;
        FQ.queue[FQ.head].length = req->length;
        // Increment head to next entry
        FQ.head = (FQ.head + 1) % 25;
        // Increment Queue entry counter
        FQ.cnt++;
    }
    InterruptMasterEnable();                // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       remove an element from the UART queue
 * Arguments:
 *                req: UART entry to be dequeued
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
int dequeue_frame(struct frame * req)
{
    int state;
    InterruptMasterDisable();                    // Disable all interrupt
    if (FQ.cnt > 0)                              // IF the queue is not empty
    {
        state = TRUE;
        req->low = FQ.queue[FQ.tail].low;
        req->high = FQ.queue[FQ.tail].high;
        req->length = FQ.queue[FQ.tail].length;
        FQ.tail = (FQ.tail + 1) % 25;   // Increment tail to entry
        FQ.cnt--;                                // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       Insert a message request into a mailbox
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
int enqueue_msg(struct msg_request * msg)
{
    int state;
    int i;
    struct mailbox *ptr;
    InterruptMasterDisable();               // Disable all interrupt
    ptr = &mailboxes[msg->dst_id];
    if (ptr->cnt == MSG_PER_Q)              // IF queue is full
        state = FALSE;
    else
    {
        state = TRUE;
        ptr->msg_queue[ptr->head].sz = msg->sz;
        ptr->msg_queue[ptr->head].src_id = running[current_priority]->mailbox_num;
        for(i = 0; i<msg->sz ; i++)
            ptr->msg_queue[ptr->head].msg[i] = msg->msg[i];

        // Increment head to next entry
        ptr->head = (ptr->head + 1) % MSG_PER_Q;
        // Increment Queue entry counter
        ptr->cnt++;
    }
    InterruptMasterEnable();                // Enable all interrupts
    return state;
}

/* -------------------------------------------------------------------------- *
 * Purpose:       remove an element from a mailbox
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
int dequeue_msg(struct msg_request *msg)
{
    int state;
    int i;
    struct mailbox *ptr;
    InterruptMasterDisable();                    // Disable all interrupt
    state = TRUE;
    ptr = &mailboxes[running[current_priority]->mailbox_num];
    if (ptr->cnt > 0)                              // IF the queue is not empty
    {
        msg->sz = (ptr->msg_queue[ptr->tail].sz > msg->sz) ? msg->sz : ptr->msg_queue[ptr->tail].sz;
        msg->src_id = ptr->msg_queue[ptr->tail].src_id;
        for(i = 0; i<msg->sz ; i++)
            msg->msg[i] = ptr->msg_queue[ptr->tail].msg[i];
        msg->msg[--i] = '\0';
        ptr->tail = (ptr->tail + 1) % MSG_PER_Q;
                                                                // Increment tail to entry
        ptr->cnt--;                                // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}

