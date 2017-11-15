/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   29  Oct   2017
 * Submission date : 15 Nov 2017
 * File name : Kcommands.h
 * Purpose: Define kernel data structures and function entry points.
 * Acknowledgment: This code is based on the source code provided in class
 * -------------------------------------------------------------------------  */
#ifndef KCOMMANDS_H_
#define KCOMMANDS_H_

#include "Pcommands.h"
#include "process_support.h"

#define MSG_PER_Q       8   /* Maximum number of messages per mailbox         */
#define UNBOUND_Q       -1  /* signals that a process does not have a queue   */
#define MAX_MSG_SZ      64  /* done for memory constraints                    */

#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000

/* Message queue */
struct mailbox
{
    struct pcb *process;                /* store PCB if a process is blocked  */
    struct mcb msg_queue[MSG_PER_Q];    /* circular queue of msgs in mailbox  */
    int head;                           /* first message in the queue         */
    int tail;                           /* last  message in the queue         */
    volatile int cnt;                   /* number of messages in the queue    */
    /* these elements are message info in event of process blocking           */
    char *buffer_addr;                  /* Address of process's buffer        */
    int  *buffer_size;                  /* Size of process buffer             */
    int  *src_id;                       /* slot to store sender ID            */
    int  sz;                            /* Actual size of the message         */
};

/* A single UART display command structure */
struct UART_entry
{
    struct pcb  *proc;                          /* requesting process's PCB   */
    char        *dsp_msg;                       /* ptr to string to display   */
};

/* List of UART display commands */
struct UART_queue
{
    struct UART_entry queue[MAX_UART_REQ];      /* UART queue of requests */
    int head;                                   /* Head of circular queue */
    int tail;                                   /* Tail of circular queue */
    volatile int cnt;                           /* Number of rqs in queue */
};

/* External variables */
extern struct mailbox mailboxes[];

/* function prototypes  */
void init_kernel(void);
void SVCall(void);
void PendSV(void);
int kgetid(void);
void kterm(void);
void knice(int*);
int kbind(int);
int ksend(struct msg_request *req);
int krecv(struct msg_request *req);
void kdisplay(char *dsp);

#endif /* KCOMMANDS_H_ */
