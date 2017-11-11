/*
 * Pcommands.c
 *
 *  Created on: Nov 8, 2017
 *      Author: AbdullahAlhadlaq
 */
#include <stdio.h>
#include "Pcommands.h"
#include "process_support.h"

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
int psend(int dst_id,char *msg, int sz)
{
    struct msg_request req;

    if (sz > MAX_MSG_SZ)
        return FAIL;

    req.dst_id = dst_id;
    req.msg = msg;
    req.sz = sz;

    return pkcall(SEND,&req);
}
