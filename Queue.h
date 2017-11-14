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


/* function prototypes */
int enqueue_msg(struct msg_request *);
int dequeue_msg(struct msg_request *);
int enqueue_UART(struct UART_entry * req);
int dequeue_UART(struct UART_entry *req);


#endif /* QUEUE_H_ */
