/*
 * Pcommands.c
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
#include <stdio.h>
#include "Pcommands.h"
#include "process_support.h"

/* */
#define SVC()   __asm(" SVC #0")


/* pkcall */
int pkcall(int code, void *pkmsg)
{
/*
Process-kernel call function.  Supplies code and kernel message to the
kernel is a kernel request.  Returns the result (return code) to the caller.
*/
volatile struct krequest arglist;
/* Pass code and pkmsg to kernel in arglist structure */
arglist.code = code;
arglist.pkmsg = pkmsg;
/* R7 = address of arglist structure */
assignR7((unsigned long) &arglist);
/* Call kernel */
SVC();
/* Return result of request to caller */
return arglist . rtnvalue;
}

/*******************************************************************************
* Purpose:
*             Terminate a process automatically by having the process's return
*             address point to this function. Calls Pkcall to invoke kernel
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void pterm(void)
{
    pkcall(TERMINATE, NULL);
}

/*******************************************************************************
* Purpose:
*             invoke the kernel to get the process id from its process control
*             block. Then, it returns the id to the calling process.
* Arguments:
*             NONE
* Return :
*             process id
*******************************************************************************/
int pgetid(void)
{
    /* the id should be returned by pkcall */
    return pkcall(GETID, NULL);
}

/*******************************************************************************
* Purpose:
*             invoke the kernel to change the process priority to supplied
*             priority.
* Arguments:
*             pr:       desired priority given by process
* Return :
*             SUCCESS      priority is valid
*             FAIL     otherwise.
*******************************************************************************/
int pnice(int pr)
{
    /* the id should be returned by pkcall */
    if (pr > HIGH_ || pr < LOW_)
        return FAIL;

    pkcall(NICE, &pr);
    return SUCCESS;
}

/*******************************************************************************
* Purpose:
*             bind a process to a queue
* Arguments:
*             num:      queue number for binding
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL      if invalid number is given by process or binding failed
*******************************************************************************/
int pbind(int num)
{
    /* the id should be returned by pkcall */
    if (num >= MAX_MSG_QUEUE || num   < 0)
        return FAIL;
    else
        return pkcall(BIND, &num);;
}

/*******************************************************************************
* Purpose:
*             This function creates a message request
* Arguments:
*             dst_id:   the mailbox number of the receiver
*             msg:      the message to be sent
*             sz:       the size of the message in bytes
* Return :
*             BYTE_NUM  the number of bytes copied to the receiver
*             FAIL      if sending is unsuccessful
*******************************************************************************/
int psend(int dst_id,void *msg, unsigned short sz)
{
    struct msg_request req;                 /* */
    if (sz > MAX_MSG_SZ || msg == NULL)     /* */
        return FAIL;
    req.id = dst_id;                        /* */
    req.msg = (char *) msg;                 /* */
    req.sz = sz;                            /* */

    return pkcall(SEND,&req);
}



/*******************************************************************************
* Purpose:
*             This function receives a message
* Arguments:
*             dst_id:   the mailbox number of the receiver
*             msg:      the message to be sent
*             sz:       the size of the message in bytes
* Return :
*             BYTE_NUM  the number of bytes copied to the receiver
*             FAIL      if sending is unsuccessful
*******************************************************************************/
int precv(int *src_id,void *msg, unsigned short maxsz)
{
    struct msg_request recv_msg;            /* */
    if (msg == NULL || maxsz == 0)          /* */
        return FAIL;
    recv_msg.msg = (char *)msg;             /* */
    recv_msg.sz = maxsz;                    /* */
    if (pkcall(RECV,&recv_msg) == FAIL)     /* */
        return FAIL;
    *src_id = recv_msg.id;                  /* src ID stored in req after pkcall */
    return recv_msg.sz;                     /* msg sz stored in req after pkcall */
}

/*******************************************************************************
* Purpose:
*             This function context switches the current process out.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void psleep(void)
{
    pkcall(SLEEP,NULL);
}



/*******************************************************************************
* Purpose:
*             This function prepares a UART request by filling up the escape
*             sequence corresponding to the row and column number provided
*             by the calling process then calling SVC which will handle
*             printing the string.
* Arguments:
*             str:      pointer to the string to be printed
*             col:      column number for the cursor
*             row:      row number for the cursor
* Return :
*             SUCCESS   if the UART request has been sent successfully
*             FAIL      if the request is not successful
*******************************************************************************/
int pdisplay_str(unsigned int col, unsigned int row, char *str)
{
    char  dsp_msg[UART_MAX_MSG];
    int allowed_size = UART_MAX_MSG-1; // last char is NULL;
    int i=0;

    /*the column number and the row number must be within the acceptable range*/
    if (col>MAX_COLUMN || row>MAX_ROW || !str)
        return FAIL;
    /*if column and row are acceptable, then do the following*/
    /*fill the beginning of the string with the escape sequence*/
    dsp_msg[0] = '\e';
    dsp_msg[1] = '[';
    dsp_msg[2] = row/10 + '0';
    dsp_msg[3] = row%10 + '0';
    dsp_msg[4] = ';';
    dsp_msg[5] = col/10 + '0';
    dsp_msg[6] = col%10 + '0';
    dsp_msg[7] = 'H';
    /*copy the message directly after*/
    while(str[i] && i < allowed_size)
    {
        dsp_msg[ESC_SEQ_SZ+i] = str[i];
        i++;
    }
    dsp_msg[ESC_SEQ_SZ+i] = NUL;

    pkcall(DISPLAY,dsp_msg);

    return SUCCESS;
}
