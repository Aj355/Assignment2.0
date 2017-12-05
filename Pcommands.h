
/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct  2017
 * Submission date : 15 Nov 2017
 * File name : Pcommands.h
 * Purpose: Define command parameters in process space and necessary data 
 *              structures. Function entry points are given to user as a guide.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */
#ifndef PCOMMANDS_H_
#define PCOMMANDS_H_

#define MAX_MSG_QUEUE   32    /* Maximum number of mailboxes                  */
#define MAX_MSG_SZ      64    /* Done for memory constraints				  */
#define MAX_UART_REQ    64    /* Number of UART requests in the queue		  */
#define UART_MAX_MSG    128   /* Maximum number of bytes in a UART message    */
#define MAX_ROW         24    /* Maximum number of rows in a window		      */
#define MAX_COLUMN      80    /* Maximum number of columns in a window        */
#define ESC_SEQ_SZ      8	  /* Number of bytes of cursor position sequence  */
#define NUL             '\0'  /* NUL character to terminate strings			  */
#define ESC             '\e'  /* Escape character to begin an escape sequence */
#define DEL             127   /* delete character                             */
#define CARRIAGE_RTN    '\r'  /* carriage return character                    */
#define TIME_SERVER     0     /* Time server process ID and mailbox number    */
#define IDLE_PROC       1     /* Idle process ID            */
#define VERTUAL_TRN     2     /* vertual train proc id and mailbox number   */
#define SYSTICK         -2
#define UART            -3
#define SVC()   __asm(" SVC #0")

/* Kernel command codes */
enum CODE_TYPE {TERMINATE, GETID, NICE, BIND, SEND, RECV, DISPLAY};

/* Time server requests */
enum TIME_SERVER_REQ {_TIME, _SLEEP};

/* Message control block as stored in mailbox */
struct mcb
{
    int  src_id;                 /* Source id */
    char msg[MAX_MSG_SZ];        /* Store a message if dest is not recieving */
    int  sz;                     /* Size of the message inside the buffer    */
};

/* Message request block used by both send and recieve */
struct msg_request
{
    int  src_id;                  /* Source mailbox ID */
	int  dst_id;				  /* Destination mailbox ID */
    int  sz;					  /* Message byte size */
    char *msg;					  /* Pointer to the message */
};

/* arguments to pass to the kernel during a kernel request */
struct krequest
{
enum CODE_TYPE code;    /* Unique (and defined) kernel code		       */
int rtnvalue;			/* Result of operation (specific to each code) */
void *pkmsg;		    /* Address (32-bit value) of process message   */
};

/* Time server requests structure */
struct time_req
{
	enum TIME_SERVER_REQ code;  /* TIME | SLEEP */
    unsigned long counter;		/* duration of sleep in systicks */
};


#endif /* PCOMMANDS_H_ */
