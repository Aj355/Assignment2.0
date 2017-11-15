


#ifndef SYSTEM_REQUESTS_H_
#define SYSTEM_REQUESTS_H_

/* Use this structure to register processes */
struct process_reg
{
    void (*func)(void);
    int priority;
};

/* */
extern struct process_reg processes[];

/* Process prototypes are written here */
void proc1 (void);
void proc2 (void);
void proc3 (void);



/*******************************************************************************
*                             SYSTEM COMMANDS                                  *
********************************************************************************
* Purpose:
*             Terminate a process automatically by having the process's return
*             address point to this function. Calls Pkcall to invoke kernel
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void pterm(void);

/*******************************************************************************
* Purpose:
*             invoke the kernel to get the process id from its process control
*             block. Then, it returns the id to the calling process.
* Arguments:
*             NONE
* Return :
*             process id (PID)
*******************************************************************************/
int pgetid(void);

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
int pnice (int pr);

/*******************************************************************************
* Purpose:
*             bind a process to a queue
* Arguments:
*             num:      queue number for binding
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL      if invalid number is given by process or binding failed
*******************************************************************************/
int pbind(int num);

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
int psend(int dst_id, void *msg, unsigned short sz);

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
int precv(int *src_id,void *msg, unsigned short maxsz);

/*******************************************************************************
* Purpose:
*             This function context switches the current process out.
* Arguments:
*             duration: multiples of a tenth of a second
* Return :
*             NONE
*******************************************************************************/
unsigned long get_time(void);

/*******************************************************************************
* Purpose:
*             This function context switches the current process out.
* Arguments:
*             duration: multiples of a tenth of a second
* Return :
*             NONE
*******************************************************************************/
void sleep(int duration);

/*******************************************************************************
* Purpose:
*             This function prepares a UART request by filling up the escape
*             sequence corresponding to the row and column number provided
*             by the calling process then calling SVC which will handle
*             printing the string. NUL termination is required (enforced)
* Arguments:
*             str:      pointer to the string to be printed
*             col:      column number for the cursor
*             row:      row number for the cursor
* Return :
*             SUCCESS   if the UART request has been sent successfully
*             FAIL      if the request is not successful
*******************************************************************************/
int pdisplay_str(unsigned int col, unsigned int row, char *str);


#endif /* SYSTEM_REQUESTS_H_ */
