/******************************************************************************/
/* Filename: Queue.c
 * Author: Abdullah Alhadlaq
 * Date: 25 Sep 2017
 * Purpose: This file contains the initialization routine as well as enqueuing
 *          and dequeuing routines for the Queue module.
 * Edits:
 * 30 sep 2017: -Added protection in critical sections
 */
/******************************************************************************/

#include "Queue.h"
#include "UART.h"

/* an array of the queues: input and output */
struct queue queues[QUEUE_NUM];

/* This function initializes the queues by assigning initial
 * values to the head, the tail, and the counter of each
 * queue.
 * Argument:
 *      NONE
 * Returns:
 *      NONE
 */
void init_queues(void)
{
    int i;  // used in the for loop

    /* initialize all queues: both input and output */
    for (i=0; i<QUEUE_NUM; i++)
    {
        queues[i].head = 0;
        queues[i].tail = 0;
        queues[i].counter = 0;
    }

    return;
}

/* This function enqueues an entry to the head
 * of a queue.
 * Arguments:
 *      queue_type: input or output queue
 *      added_entry: entry to be added to queue
 * Return:
 *      TRUE: if enqueuing is successful
 *      FALSE: if enqueuing is unsuccessful
 */
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
