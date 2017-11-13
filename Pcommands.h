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
#define UART_MAX_MSG    128
#define MAX_ROW         24
#define MAX_COLUMN      80
#define ESC_SEQ_SZ      8
#define NUL             0x00
#define ESC             0x1b



/* */
enum CODE_TYPE {TERMINATE, GETID, NICE, BIND, SEND, RECV, DISPLAY, SLEEP};



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


/* Function entry points */
void pterm(void);
int pgetid(void);
int pnice (int);
int pbind(int);
int psend(int dst_id,char *msg, int sz);
int precv(int *src_id,char *msg, unsigned short maxsz);
void psleep(void);
int pdisplay_str(unsigned int col, unsigned int row, char *str);


#endif /* PCOMMANDS_H_ */
