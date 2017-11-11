/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman Aljedaibi
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   07  Sept  2017
 * Date created  :   20  Sept  2017
 * Submission date : 02 Oct 2017
 * File name : Queue.h
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
 * -------------------------------------------------------------------------- */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "process_support.h"
#include "Pcommands.h"
#include "Kcommands.h"

/* maximum number of queue entries for both input and output */
#define MAX_ENTRIES 128
/* number of queues: Input and output */
#define QUEUE_NUM   2
/* to signal successful or unsuccessful enqueuing and dequeuing */
#define FALSE   0
#define TRUE    1

/* two types of entries in a queue: either a character for
 * UART, or an entry signaling that one-tenth a second has
 * elapsed.
 */
enum entry_type {UART, SYSTICK};

/* an index for the type of queue (input or output) */
enum Queue_Type {INPUT, OUTPUT};

/* the structure of a queue entry. It is the same
 * for both input and output queues.
 */
struct entry
{
    unsigned short type;        /* SYSTICK | UART */
    unsigned char character;    /* if it is of type UART */
};

/* structure for a queue */
struct queue
{
    unsigned short head;                // index to the top of the queue
    unsigned short tail;                // index to the bottom of the queue
    unsigned short counter;             // the current content of the queue
    struct entry entries[MAX_ENTRIES];  // the nodes of the queue
};

/* external definition of the input and output queues
 * (contained in the array called queues)
 */
extern struct queue queues[];   // contains input and output queues

/* function prototypes */
unsigned short enqueue (unsigned short queue_type, struct entry added_entry);
unsigned short dequeue (unsigned short queue_type, struct entry *removed_entry);
int enqueue_msg(struct msg_request *);
int dequeue_msg(struct mcb *);

#endif /* QUEUE_H_ */
