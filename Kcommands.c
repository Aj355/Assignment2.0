/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   24  Oct   2017
 * Editing       :   25  Oct - Used code from course website to context switch
 *                   28  Oct - written nice command and get ID
 *                    1  Nov - bind command is done
 *                    9  Nov - messaging system is complete
 * Submission date : 15  Nov   2017
 * File name : Kcommands.c
 * Purpose: Implementation of kernel calls and kernel commands.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "kernel_calls.h"
#include "Kcommands.h"
#include "Pcommands.h"
#include "process_support.h"
#include "Queue.h"
#include "UART.h"
#include "SYSTICK.h"
#include "system_procs.h"

#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000

 
struct mailbox mailboxes[MAX_MSG_QUEUE]; /* List of message queues */


/*******************************************************************************
* Purpose:
*             Supervisory call issued when pkcall uses SVC(); macro. This is the
*             Supervisor call (trap) entry point. From here, all registers are
*             saved, SVCHandler is called, and when it returns, all registers
*             are loaded again.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void SVCall(void)
{
	/*
	* Using MSP - trapping process either MSP or PSP (specified in LR)
	* Source is specified in LR: F9 (MSP) or FD (PSP)
	* Save r4-r11 on trapping process stack (MSP or PSP)
	* Restore r4-r11 from trapping process stack to CPU
	* SVCHandler is called with r0 equal to MSP or PSP to access any arguments
	*/

	/* Save LR for return via MSP or PSP */
	__asm("     PUSH    {LR}");

	/* Trapping source: MSP or PSP? */
	__asm("     TST     LR,#4");      /* Bit #4 indicates MSP (0) or PSP (1) */
	__asm("     BNE     RtnViaPSP");

	/* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
	__asm("     PUSH    {r4-r11}");
	__asm("     MRS r0,msp");
	__asm("     BL  SVCHandler");       /* r0 is MSP */
	__asm("     POP {r4-r11}");
	__asm("     POP     {PC}");

	/* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
	__asm("RtnViaPSP:");
	__asm("     mrs     r0,psp");
	__asm("     stmdb   r0!,{r4-r11}"); /* Store multiple, decrement before */
	__asm("     msr psp,r0");
	__asm("     BL  SVCHandler");       /* r0 Is PSP */

	/* Restore r4..r11 from trapping process stack  */
	__asm("     mrs     r0,psp");
	__asm("     ldmia   r0!,{r4-r11}"); /* Load multiple, increment after */
	__asm("     msr psp,r0");
	__asm("     POP     {PC}");

}

/*******************************************************************************
* Purpose:
*             Initialize essential kernel structures such as registration of
*             system processes (IDLE, time server, etc) and the initialization
*             of the UART module as well as enabling CPU interrupts and setting
*             PENDSV priority to lowest possible.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void init_kernel(void)
{

	/* Initialize UART */
	init_UART();

	/* Enable Master (CPU) Interrupts */
	InterruptMasterEnable();

	/* give PendSV the lowest priority */
	NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY;

	/* Register system processes (name,ID,Priority) */
    reg_proc(time_server, 0, 3);
    reg_proc(idle, 1, 0);
    //reg_proc(reserved1, 2, 3);
    //reg_proc(reserved2, 3, 3);
    //reg_proc(reserved3, 4, 3);


}

/*******************************************************************************
* Purpose:
*             Supervisor call handler. Handles the startup of the first initial
*             process and all other SVCs such as geid, terminate, the messaging
*             system, etc. Assumes that the first call is to start up the first
*             process. Since this has been called as a trap (Cortex exception),
*             the code is in Handler mode and uses the MSP.
* Arguments:
*             argptr: Argptr points to the full stack consisting of both hardware
*              and software register pushes. it is R0 as setup in SVCall()
* Return :
*             NONE
*******************************************************************************/
void SVCHandler(struct stack_frame *argptr)
{
	/* Argptr points to (i.e., has the value of) either:
	-  the top of the MSP stack (startup initial process)
	-  the top of the PSP stack (all subsequent calls)
	*/
	static int firstSVCcall = TRUE;
	struct krequest *kcaptr;

	if (firstSVCcall)
	{
		/*
		* Force a return using PSP
		* This will be the first process to run, so the eight "soft pulled" registers
		(R4..R11) must be ignored otherwise PSP will be pointing to the wrong
		location; the PSP should be pointing to the registers R0..xPSR, which will
		be "hard pulled"by the BX LR instruction.
		* To do this, it is necessary to ensure that the PSP points to (i.e., has) the
		address of R0; at this moment, it points to R4.
		* Since there are eight registers (R4..R11) to skip, the value of the sp
		should be increased by 8 * sizeof(unsigned int).
		* sp is increased because the stack runs from low to high memory.
		*/
		set_PSP(running[current_priority]->sp + 8 * sizeof(unsigned int));

		firstSVCcall = FALSE;

		/* Start SysTick */
		init_systick();

		/*
		- Change the current LR to indicate return to Thread mode using the PSP
		- Assembler required to change LR to FFFF.FFFD (Thread/PSP)
		- BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
		*/
		__asm(" movw    LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
		__asm(" movt    LR,#0xFFFF");  /* Upper 16 only */
		__asm(" bx  LR");          /* Force return to PSP */
	}
	else /* Subsequent SVCs */
	{
		/*
		* kcaptr points to the arguments associated with this kernel call
		* argptr is the value of the PSP (passed in R0 and pointing to the TOS)
		* the TOS is the complete stack_frame (R4-R10, R0-xPSR)
		* in this example, R7 contains the address of the structure supplied by
		the process - the structure is assumed to hold the arguments to the
		kernel function.
		* to get the address and store it in kcaptr, it is simply a matter of
		assigning the value of R7 (arptr -> r7) to kcaptr
		*/

		kcaptr = (struct krequest *) argptr->r7;

		/* Switch on kernel command code message */
		switch (kcaptr->code)
		{

		/* Terminate a process */
		case TERMINATE:
			kterm();		   /* CALL ther kernel termination process */
			break;

		/* Inquire about process ID */
		case GETID:
			kcaptr->rtnvalue = kgetid(); 
			break;

		/* Update the priority of a process */
		case NICE:
			knice(kcaptr->pkmsg);
			break;

		/* Bind a process to a message mailbox */
		case BIND:
			kcaptr->rtnvalue = kbind(*((int *)kcaptr->pkmsg));
			break;

		/* Send a message to another process, or a server (system process) */
		case SEND:
			kcaptr->rtnvalue = ksend(kcaptr->pkmsg);
			break;

		/* Recieve a message from another process, or a server */
		case RECV:
			kcaptr->rtnvalue = krecv(kcaptr->pkmsg);
			break;

		/* Display a string to output terminal via UART */
		case DISPLAY:
			kdisplay(kcaptr->pkmsg);
			break;

		/* Undefined command */
		default:
			kcaptr->rtnvalue = -1;
		}
	}
}

/*******************************************************************************
* Purpose:
*             PENDSV handler. Registers r0..r3, xpsr,pc,lr are pushed implicitly
*					While registers R4..R11 are pushed explictly by 
*					save_registers() function. Stores the current SP into PCB 
*					and context switches the current process tothe next one in
*					the waiting to run (WTR) queue. R4..R11 are then restored 
*					manually to for the next process to run properly. Finally, 
*				the implicitly saved registers of the now running process
*				are implicitly popped. 
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void PendSV(void)
{
	//InterruptMasterDisable();
	save_registers();							/* Save registers R4..R11     */
	running[current_priority]->sp = get_PSP();	/* Store current SP in PCB    */
	running[current_priority] = running[current_priority]->next;/* Con-switch */
	set_PSP(running[current_priority]->sp);		/* restore process SP to PSP  */
	restore_registers();						/* Load registers R4..R11     */
	//InterruptMasterEnable();
}

/*******************************************************************************
* Purpose:
*             Terminate a process by removing it from the waiting to run queue
*             and deallocate all its memory.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void kterm(void)
{
    struct pcb *temp = running[current_priority];
    
    /* IF a process is bound to a queue */
    if (temp -> mailbox_num != UNBOUND_Q)
    {
        /* free its message queue and unbind */
        mailboxes[temp -> mailbox_num].head = 0;
        mailboxes[temp -> mailbox_num].tail = 0;
        mailboxes[temp -> mailbox_num].cnt  = 0;
        mailboxes[temp -> mailbox_num].process = NULL;
    }
    
	dequeue_running_pcb();			 /* Remove calling process from WTR queue */
    free (temp -> stack_addr);	     /* Free its allocated stack memory       */
    free (temp);					 /* Free allocated memory of PCB          */
    set_PSP(running[current_priority] -> sp); /* Set PSP to next process's SP */
}

/*******************************************************************************
* Purpose:
*             get the currently running process's id from the its PCB.
* Arguments:
*             NONE
* Return :
*             running process id is returned
*******************************************************************************/
int kgetid(void)
{
    return running[current_priority] -> id;	
}

/*******************************************************************************
* Purpose:
*             bind the calling process to the specified mailbox number. IF the
*						the process is already bound to a mailbox or if the 
*						queue is already bound to a process, this command fails.
* Arguments:
*             num:      queue number for binding
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL (-1) if invalid number is given by process or binding failed
*******************************************************************************/
int kbind(int num)
{
    /* IF Process is already bound to mailbox OR mailbox is already bound */
    if (running[current_priority] -> mailbox_num != UNBOUND_Q ||
            mailboxes[num].process != NULL )
        /* THEN EXIT with error */
        return FAIL;
    else
    {
        /* store desired mailbox ID in PCB */
        running[current_priority] -> mailbox_num = num;
        /* connect PCB to desired mailbox */
        mailboxes[num].process = running[current_priority];
        /* EXIT with mailbox number */
        return num;
    }
}

/*******************************************************************************
* Purpose:
*             This function takes a message request and either gives it
*             directly to the receiving process of stores it in the receiving
*             process's mailbox.
* Arguments:
*             req:      message request containing (msg, sz, dst_id, src_id)
* Return :
*             QUEUE NUM if successful binding is done
*             FAIL      if invalid number is given by process or binding failed
*******************************************************************************/
int ksend(struct msg_request *req)
{
    int i;
    struct mailbox *dst_mail = &mailboxes[req->dst_id];
    int max_sz = req->sz;       /*maximum size to be copied*/
    /*IF sender does not own a mailbox OR destination mailbox not found*/
    if (req ->src_id != SYSTICK &&
		(running[current_priority]->mailbox_num == UNBOUND_Q ||
			dst_mail->process == NULL))
        /*THEN EXIT with error code*/
        return FAIL;

    /*IF receiving process is blocked*/
    if (dst_mail->buffer_addr != NULL)
    {
        /* max_sz is either size of sender's msg or receiver's buffer
         * whichever is smaller */
        max_sz = (dst_mail->sz > req->sz) ? req->sz : dst_mail->sz;
        /*so that the receiver knows the number of bytes copied*/
        *(dst_mail ->buffer_size) = max_sz;
        /*THEN give source id to receiver*/
        *(dst_mail->src_id) = (req->src_id == SYSTICK)? SYSTICK:running[current_priority]->mailbox_num;
        /*copy message into receiver buffer until it's full or message is complete*/
        for (i=0; i<max_sz; i++)
            dst_mail->buffer_addr[i] = req->msg[i];

        dst_mail->buffer_addr[--i] = '\0';
        /*Unblock receiver by inserting its PCB into WTR queue*/

        running[current_priority]->sp = get_PSP();
        enqueue_pcb(dst_mail->process);

        set_PSP(running[current_priority] -> sp);

        /*put a null in the buffer_addr in mailbox to signify that process is not blocked*/
        dst_mail->buffer_addr = NULL;
    }
    else /* if process not blocked */
    {
        /*put message, its size, and source id into the receive mailbox*/
        enqueue_msg(req);
    }

    /*EXIT with the number of copied bytes*/
    return max_sz;
}



/*******************************************************************************
* Purpose:
*             This function checks if the calling process is bound to a queue. 
*					Then, the mailbox is checked for messages. If there are no
*					messages, the calling process is blocked while it waits to 
*					for a message to arrive. When a process is blocked, it will
*					will have left the address of its buffer to store the next 
*					arriving message right before it blocks. Otherwise, this 
*					function will get the next message from its mailbox and 
*					returns it by address to the calling process.
* Arguments:
*             req:      message request containing (msg, sz, dst_id, src_id)
* Return :
*             TRUE      if successful binding is done
*             FALSE     if invalid number is given by process or binding failed
*******************************************************************************/
int krecv(struct msg_request *req)
{
	/* */
    if (running[current_priority]->mailbox_num == UNBOUND_Q) 
        return FALSE;

	/* */
    else 
    {
		/* */
        if (mailboxes[running[current_priority]->mailbox_num].cnt == 0)
        {
            mailboxes[running[current_priority]->mailbox_num].src_id = &(req->src_id);
            mailboxes[running[current_priority]->mailbox_num].sz = req->sz;
            mailboxes[running[current_priority]->mailbox_num].buffer_size = &(req->sz);
            mailboxes[running[current_priority]->mailbox_num].buffer_addr = req->msg;
            running[current_priority]->sp = get_PSP();
            dequeue_running_pcb();
            set_PSP(running[current_priority] -> sp);
            (*req).sz = mailboxes[running[current_priority]->mailbox_num].sz;
        }

		/* */
        else
            dequeue_msg(req);

    }
    return TRUE;
}

/*******************************************************************************
* Purpose:
*             This function prints a string by putting a display request into
*             the UART list if the UART is busy or putting it directly into
*             the data register if it is idle.
* Arguments:
*             dsp_msg:  message to be displayed (string)
* Return :
*             SUCCESS   if enqueuing of the message is successful
*             FAIL      if enqueuing is not successful
*******************************************************************************/
void kdisplay(char *dsp)
{
    struct UART_entry new_entry;
    new_entry.dsp_msg = dsp;
    new_entry.proc = running[current_priority];
    if (UART_state == BUSY)
    {
        enqueue_UART(&new_entry);
    }
    else /*if UART is idle*/
    {
        UART_state = BUSY;
        current_msg.dsp_msg = new_entry.dsp_msg;
        current_msg.proc    = new_entry.proc;
        UART0_DR_R = *dsp;
        new_entry.dsp_msg++;
    }

    //block the process
    running[current_priority]->sp = get_PSP();
    dequeue_running_pcb();
    set_PSP(running[current_priority]->sp);

}
/*******************************************************************************
* Purpose:
*             get the process's id from the running PCCB
* Arguments:
*             pr:       desired priority supplied by process
* Return :
*             NONE
*******************************************************************************/
void knice(int* pr)
{
    struct pcb *temp = running[current_priority];

    running[current_priority]->sp = get_PSP();

    running[current_priority] ->priority = *pr;

    if (*pr == current_priority)
        return;

    dequeue_running_pcb();

    /* Connect PCB to the end of the priority queue*/
    if (running[*pr] == NULL)
    {
        /* if the queue is empty, let the priority queue pointer point at this pcb.
         * Make the next and previous pointer point to the same pcb (for circular queue)
         */
        running[*pr] = temp;
        temp -> next         = temp;
        temp -> prev         = temp;
    }else {
        /* if the queue is not empty, add the current pcb to the end of the queue*/
        temp -> next                 = running[*pr];
        temp -> prev                 = running[*pr]->prev;
        running[*pr] -> prev   = temp;
        temp -> prev-> next          = temp;
    }

    if (current_priority < *pr )
        current_priority = *pr;

    set_PSP(running[current_priority] -> sp);
}


