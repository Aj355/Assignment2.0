
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
 * -------------------------------------------------------------------------- */
#include "Queue.h"
#include "UART.h"
#include "Kcommands.h"
/* an array of the queues: input and output */
struct queue queues[QUEUE_NUM];

/* Globals */
volatile int cnt = 0;     // Initialize both queue counters

unsigned short enqueue (unsigned short queue_type, struct entry added_entry)
{
    /* identify what queue to use (input or output) */
    struct queue *used_queue = &queues[queue_type];
    int state;  // signals successful or unsuccessful enqueuing

    /* disable interrupts before entering critical section */
    InterruptMasterDisable();

    /*IF queue_counter = MAX_QUEUE_ENTRIES meaning that it is full*/
    if ((*used_queue).counter == MAX_ENTRIES)
        /* Exit and return an error code (specified by a value of 0) */
        state = FALSE;

    else /* if not full */
    {
        /* Add entry to the head of the queue */
        (*used_queue).entries[(*used_queue).head].character =
                                                        added_entry.character;
        (*used_queue).entries[(*used_queue).head].type =
                                                        added_entry.type;
        /* Update the position of the head of the queue
         * (by incrementing the head integer) */
        (*used_queue).head = ((*used_queue).head +1) % MAX_ENTRIES;
        /* Increment the queue_counter */
        (*used_queue).counter ++;
        /* Signal successful enqueuing by sending 1 to the calling func */
        state = TRUE;
    }

    /* enable interrupts since we exited critical section */
    InterruptMasterEnable();

    return state;   /* 1 successful | 0 unseccuessful */

}

/* This function dequeues an entry from the tail
 * of a queue and sends it to the calling function by
 * copying the information from removed entry to the entry
 * variable given by the calling function.
 * Arguments:
 *      queue_type: input or output queue
 *      removed_entry: reference to the entry to be removed from a queue
 * Return:
 *      TRUE: if dequeuing is successful
 *      FALSE: if dequeuing is unsuccessful
 */
unsigned short dequeue (unsigned short queue_type, struct entry *removed_entry)
{
    /* identify what queue to use (input or output) */
    struct queue *used_queue = &queues[queue_type];
    int state; /* signals successful or unsuccessful dequeuing */

    /* disable interrupts before entering critical section */
    InterruptMasterDisable();

    /*IF queue_counter = 0 meaning that it is empty*/
    if ((*used_queue).counter == 0)
        /* Exit and return an error code (specified by a value of 0) */
        state = FALSE;

    else /* if not empty */
    {
        /* Supply entry info (TYPE, and CHAR) to the calling subroutine */
        (*removed_entry).character =
                (*used_queue).entries[(*used_queue).tail].character;
        (*removed_entry).type =
                (*used_queue).entries[(*used_queue).tail].type;
        /* Update pos of tail of queue (by decrementing the tail integer) */
        (*used_queue).tail = ((*used_queue).tail + 1) % MAX_ENTRIES;
        /* decrement the queue_counter */
        (*used_queue).counter --;
        /* Signal successful dequeuing by sending 1 to the calling subroutine */
        state = TRUE;
    }

    /* enable interrupts since we exited critical section */
    InterruptMasterEnable();

    return state;   /* 1 successful | 0 unsuccessful */
}


/* -------------------------------------------------------------------------- *
 * Purpose: Insert an entry into the input or output queue.
 * Parameters:
 *                c = Queue type (OUTPUT  or INPUT)
 *                s = Entry type (SYSTICK or UART )
 *                data = Character to be stored in Queue (matters in UART)
 * Returns: none
 * -------------------------------------------------------------------------- */
int enqueue_msg(struct mcb * msg, int dst_id)
{
    InterruptMasterDisable();               // Disable all interrupt
    int state = TRUE;
    int i;
    if (mailboxes[dst_id].cnt == MSG_PER_Q) // IF queue is full
        state = FALSE;
    else
    {
        mailboxes[dst_id].msg_queue.sz = msg->sz;
        mailboxes[dst_id].msg_queue.src_id = msg->src_id;
        for(i = 0; i<msg->sz ; i++)
            mailboxes[dst_id].msg_queue.msg[i] = msg->msg[i];

        // Increment head to next entry
        mailboxes[dst_id].head = (mailboxes[dst_id].head + 1) % QSIZE;
        // Increment Queue entry counter
        mailboxes[dst_id].cnt++;
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
int dequeue(struct mcb *msg, int dst_id)
{
    InterruptMasterDisable();                    // Disable all interrupt
    int state = TRUE;
    if (mailboxes[dst_id].cnt > 0)                              // IF the queue is not empty
    {
        msg->sz = mailboxes[dst_id].msg_queue.sz;
        msg->src_id = mailboxes[dst_id].msg_queue.src_id;
        for(i = 0; i<msg->sz ; i++)
            msg->msg[i] = mailboxes[dst_id].msg_queue.msg[i] ;
        mailboxes[dst_id].tail = (mailboxes[dst_id].tail + 1) % QSIZE;         // Increment tail to entry
        mailboxes[dst_id].cnt--;                                // Decrement queue counter
    }
    else
        state = FALSE;
    InterruptMasterEnable();                     // Enable all interrupts
    return state;
}
