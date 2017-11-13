
/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman Aljedaibi and Abdullah alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   07  Sept  2017
 * Date created  :   21  Sept  2017
 * Editing       :   29  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 02 Oct 2017
 * File name : Queue.c
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
#include "Queue.h"
#include "UART.h"
#include "Kcommands.h"
#include "process_support.h"

/* an array of the queues: input and output */


#include "Queue.h"
struct queue queues[QUEUE_NUM];


struct UART_queue UQ;
struct UART_entry queue[MAX_UART_REQ];
int head;
int tail;
volatile int cnt;     // Initialize both queue counters





/* -------------------------------------------------------------------------- *
 * Purpose: Insert an entry into the input or output queue.
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                s = Entry type (SYSTICK or UART )
 *                data = Character to be stored in Queue (matters in UART)
 * Returns: none
 * -------------------------------------------------------------------------- */
int enqueue_TS(struct UART_entry * req)
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
 * Purpose: remove an element from an Input or Output queue
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                element = pointer desired entry to be removed from the queue
 * Returns: none
 * -------------------------------------------------------------------------- */
int dequeue_TS(struct UART_entry *req)
{
    int state;
    InterruptMasterDisable();                    // Disable all interrupt
    if (UQ.cnt > 0)                            // IF the queue is not empty
    {
        state = TRUE;
        req->dsp_msg = queue[UQ.tail].dsp_msg;
        req->proc = queue[UQ.tail].proc;
        UQ.tail = (UQ.tail + 1) % MAX_UART_REQ;        // Increment tail to entry
        UQ.cnt--;                                   // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}











/* -------------------------------------------------------------------------- *
 * Purpose: Insert an entry into the input or output queue.
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                s = Entry type (SYSTICK or UART )
 *                data = Character to be stored in Queue (matters in UART)
 * Returns: none
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
 * Purpose: remove an element from an Input or Output queue
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                element = pointer desired entry to be removed from the queue
 * Returns: none
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
 * Purpose: Insert an entry into the input or output queue.
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                s = Entry type (SYSTICK or UART )
 *                data = Character to be stored in Queue (matters in UART)
 * Returns: none
 * -------------------------------------------------------------------------- */
int enqueue_msg(struct msg_request * msg)
{
    int state;
    int i;
    struct mailbox *ptr;
    InterruptMasterDisable();               // Disable all interrupt
    ptr = &mailboxes[msg->id];
    if (ptr->cnt == MSG_PER_Q) // IF queue is full
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
 * Purpose: remove an element from an Input or Output queue
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                element = pointer desired entry to be removed from the queue
 * Returns: none
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
        msg->id = ptr->msg_queue[ptr->tail].src_id;
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
