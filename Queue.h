/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   12  Oct  2017
 * Editing       :   28  Oct  - Added enqueue/dequeue messages
 *                   01  Nov  - Added enqueue/dequeue UART
 * Submission date : 15  Nov 2017
 * File name : Queue.h
 * Purpose: Implement a static circular queue in order to manage UART and 
 *              messaging system queues.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "process_support.h"
#include "Pcommands.h"
#include "Kcommands.h"
#include "Trainsetprotocol.h"

/* maximum number of queue entries for both input and output */
#define MAX_ENTRIES 128
/* number of queues: Input and output */
#define QUEUE_NUM   2
/* to signal successful or unsuccessful enqueuing and dequeuing */
#define FALSE   0
#define TRUE    1


/* function prototypes */
int enqueue_msg(struct msg_request * req);
int dequeue_msg(struct msg_request * req);
int enqueue_UART(struct UART_entry * req);
int dequeue_UART(struct UART_entry * req);
int enqueue_frame (struct frame  * req);
int dequeue_frame (struct frame  * req);
int enqueue_packet(struct transmit * req);
int dequeue_packet(struct transmit * req);

#endif /* QUEUE_H_ */
