/*
 * Pcommands.h
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef PCOMMANDS_H_
#define PCOMMANDS_H_

#define MAX_MSG_QUEUE   128
#define MAX_MSG_SZ      64  /* done for memory constraints */

/* */
#define SVC()   __asm(" SVC #0")


/* */
enum CODE_TYPE {TERMINATE, GETID, NICE, BIND, SEND, RECV, DISPLAY};


struct msg_request
{
    int  dst_id;                 /* */
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


#endif /* PCOMMANDS_H_ */
