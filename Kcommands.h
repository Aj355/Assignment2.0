/*
 * Kcommands.h
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef KCOMMANDS_H_
#define KCOMMANDS_H_

#include "Pcommands.h"
#include "process_support.h"

#define MSG_PER_Q       8
#define UNBOUND_Q       -1  /* signals that a process does not have a queue */
#define MAX_MSG_SZ      64  /* done for memory constraints */




/* Message queue */
struct mailbox
{
    struct pcb *process;                /* */
    struct mcb msg_queue[MSG_PER_Q];    /* */
    int head;                           /* */
    int tail;                           /* */
    int cnt;                            /* */
    /* these elements are message info in event of process blocking */
    int  *src_id;
    int  sz;
    char *buffer_addr;
};

struct UART_entry
{
    struct pcb  *proc;
    char        *dsp_msg;
};
struct UART_reqs
{
    int head;
    int tail;
    struct UART_entry queue[MAX_UART_REQ];
};

extern struct mailbox mailboxes[];

/* function prototypes */
int kgetid(void);
void kterm(void);
void knice(int*);
int kbind(int);
int ksend(struct msg_request *req);
int krecv(struct msg_request *req);
#endif /* KCOMMANDS_H_ */
