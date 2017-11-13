/*
 * Pcommands.h
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */
/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct  2017
 * Editing       :   15  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 15 Nov 2017
 * File name : Queue.c
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
 * ------------------------------------------------------------------------- */
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
#define TIME_SERVER     0



/* */
enum CODE_TYPE {TERMINATE, GETID, NICE, BIND, SEND, RECV, DISPLAY, SLEEP};

enum TIME_SERVER_REQ {_TIME, _SLEEP};

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

struct time_req
{
    int code;  /* TIME | SLEEP */
    unsigned long counter; /* duration of sleep in systicks */
};

/* Function entry points */
void pterm(void);
int pgetid(void);
int pnice (int);
int pbind(int);
int psend(int dst_id, void *msg, unsigned short sz);
int precv(int *src_id,void *msg, unsigned short maxsz);
void psleep(void);
int pdisplay_str(unsigned int col, unsigned int row, char *str);


#endif /* PCOMMANDS_H_ */
