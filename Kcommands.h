/*
 * Kcommands.h
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef KCOMMANDS_H_
#define KCOMMANDS_H_

#define MAX_MSG_SZ      64  /* done for memory constraints */
#define MSG_PER_Q       8

/* Message control block */
struct mcb
{
    int  src_id;                 /* */
    char msg[MAX_MSG_SZ];        /* */
    int  sz;                     /* */
};

/* Message queue */
struct mailbox
{
    struct pcb *process;                /* */
    struct mcb msg_queue[MSG_PER_Q];    /* */
    /* these elements are message info in event of process blocking */
    int  dst_id;
    int  sz;
    char *buffer_addr;
};

extern struct mailbox mailboxes[];

/* function prototypes */
int kgetid(void);
void kterm(void);
void knice(int*);
int kbind(int);
int ksend();
#endif /* KCOMMANDS_H_ */
