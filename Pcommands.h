/*
 * Pcommands.h
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef PCOMMANDS_H_
#define PCOMMANDS_H_

#define MAX_MSG_QUEUE   32  /* Maximum number of mailboxes */
#define MAX_MSG_SZ      64  /* Done for memory constraints */
#define MAX_UART_REQ    32          // # UART requests in the queue




/* */
enum CODE_TYPE {TERMINATE, GETID, NICE, BIND, SEND, RECV, DISPLAY};



/* Message control block */
struct mcb
{
    int  src_id;                 /* */
    char msg[MAX_MSG_SZ];        /* */
    int  sz;                     /* */
};


struct msg_request
{
    int  id;                 /* dst or src */
    int  sz;
    char *msg;
};

/* */
struct krequest
{
int code;      /* Unique (and defined) kernel code */
int rtnvalue;  /* Result of operation (specific to each code) */
void *pkmsg;   /* Address (32-bit value) of process message */
};

struct UART_request
{
    int   proc_id;
    char *dsp_msg;
};

/* Function entry points */
void pterm(void);
int pgetid(void);
int pnice (int);
int pbind(int);
int psend(int dst_id,char *msg, int sz);


#endif /* PCOMMANDS_H_ */
