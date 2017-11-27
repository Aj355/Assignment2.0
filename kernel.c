/* --------------------------------------------------------------------------- *
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
 * -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "kernel.h"
#include "Kcommands.h"
#include "Pcommands.h"
#include "processes.h"
#include "Queue.h"
#include "UART.h"
#include "SYSTICK.h"
#include "Kprocesses.h"

#define SVC()   __asm(" SVC #0")
#define SYS_PROC_NUM    5


struct pcb  *running[PRIORITY_NUM];          /* Pointers to running process in WTR queues while accounting for priority */
int current_priority;                        /* Current priority level */
int pcb_counter;                             /* keeps track of the number of WTR processes */


void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
     * simply a MOV from R0 to R7
     */
    __asm(" mov r7,r0");
}


unsigned long get_PSP(void)
{
    /* Returns contents of PSP (current process stack */
    __asm(" mrs     r0, psp");
    __asm(" bx  lr");
    return 0;   /***** Not executed -- shuts compiler up */
    /***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
    /* Returns contents of MSP (main stack) */
    __asm(" mrs     r0, msp");
    __asm(" bx  lr");
    return 0;
}

void set_PSP(volatile unsigned long ProcessStack)
{
    /* set PSP to ProcessStack */
    __asm(" msr psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
    /* Set MSP to MainStack */
    __asm(" msr msp, r0");
}

void volatile save_registers()
{
    /* Save r4..r11 on process stack */
    __asm(" mrs     r0,psp");
    /* Store multiple, decrement before; '!' - update R0 after each store */
    __asm(" stmdb   r0!,{r4-r11}");
    __asm(" msr psp,r0");
}

void volatile restore_registers()
{
    /* Restore r4..r11 from stack to CPU */
    __asm(" mrs r0,psp");
    /* Load multiple, increment after; '!' - update R0 */
    __asm(" ldmia   r0!,{r4-r11}");
    __asm(" msr psp,r0");
}

unsigned long get_SP()
{
    /**** Leading space required -- for label ****/
    __asm("     mov     r0,SP");
    __asm(" bx  lr");
    return 0;
}


/*******************************************************************************
 * Purpose:
 *             This function registers a process by creating a PCB and
 *             allocating a stack for the process. Then it fills the PCB
 *             and the stack with the necessary initial values and finally
 *             inserts the PCB into the appropriate WTR queue.
 * Arguments:
 *             func:     entry point to the process
 *             id:       unique identification number for the process
 *             priority: priority level 0..3
 * Return :
 *             SUCCESS   if enqueuing of the PCB into the WTR is successful
 *             FAIL      if enqueuing is not successful
 *******************************************************************************/
int reg_proc( void (*func)(void), unsigned id, unsigned short priority)
{
    unsigned long *sp;
    struct stack_frame *regs;
    struct pcb *ptr;
    
    if (priority > HIGH_ )
    {
        return FAIL;
    }
    /* allocate stack memory*/
    sp = (unsigned long *)malloc(STACKSIZE);
    /* account for space for the stack frame*/
    regs = (struct stack_frame *)(sp + STACKSIZE - sizeof(struct stack_frame));
    /* allocate memory for PCB*/
    ptr = (struct pcb *)malloc(sizeof(struct pcb));
    
    /* Set up initial values for the registers*/
    regs->r0    = 0x00000000;
    regs->r1    = 0x11111111;
    regs->r2    = 0x22222222;
    regs->r3    = 0x33333333;
    regs->r4    = 0x44444444;
    regs->r5    = 0x55555555;
    regs->r6    = 0x66666666;
    regs->r7    = 0x77777777;
    regs->r8    = 0x88888888;
    regs->r9    = 0x99999999;
    regs->r10   = 0xAAAAAAAA;
    regs->r11   = 0xBBBBBBBB;
    regs->r12   = 0xCCCCCCCC;
    regs->lr    = (unsigned long)pterm;  /* the address of the terminate function */
    regs->pc    = (unsigned long)func;
    regs->psr  = 0x01000000;
    
    /* assign initial values to PCB*/
    ptr-> id    = id;
    ptr-> sp    = (unsigned long) regs;
    ptr-> priority = priority;
    ptr-> stack_addr = sp;
    ptr-> mailbox_num = -1;
    
    return enqueue_pcb(ptr);
}

/*******************************************************************************
 * Purpose:
 *             Supervisory call issued when pkcall uses SVC() macro. This is the
 *             Supervisor call (trap) entry point. From here, all registers are
 *             saved, SVCHandler is called, and when it returns, all registers
 *             are loaded again.
 * Arguments:
 *             NONE
 * Return :
 *             NONE
 ******************************************************************************/
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
    
    /* Trapping source is PSP - save r4-r11 on psp stack(MSP is active stack) */
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
 ******************************************************************************/
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
    
    /* here future system functions can be registered */
    
}

/*******************************************************************************
 * Purpose:
 *             Supervisor call handler. Handles the startup of the first initial
 *             process and all other SVCs such as geid, terminate, the messaging
 *             system, etc. Assumes that the first call is to start up the first
 *             process. Since this has been called as a trap (Cortex exception),
 *             the code is in Handler mode and uses the MSP.
 * Arguments:
 *             argptr: Argptr points to the full stack consisting of both
 *              hardware and software reg pushes. it is R0 as setup in SVCall()
 * Return :
 *             NONE
 ******************************************************************************/
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
         * This will be the first process to run, so the 8 "soft pulled" regs
         (R4..R11) must be ignored otherwise PSP will be pointing to the wrong
         location; the PSP should be pointing to the regs R0-xPSR, which will
         be "hard pulled"by the BX LR instruction.
         * To do this, it is necessary to ensure that the PSP points to the
         address of R0; at this moment, it points to R4.
         * Since there are eight registers (R4-R11) to skip, the value of the sp
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
                
                /* Send a message to another process, or a server (kprocess) */
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
 *            PENDSV handler. Registers r0..r3, xpsr,pc,lr are pushed implicitly
 *			  While registers R4..R11 are pushed explictly by
 *			  save_registers() function. Stores the current SP into PCB
 *			  and context switches the current process tothe next one in
 *			  the waiting to run (WTR) queue. R4..R11 are then restored
 *			  manually to for the next process to run properly. Finally,
 *			  the implicitly saved registers of the now running process
 *			  are implicitly popped.
 * Arguments:
 *             NONE
 * Return :
 *             NONE
 ******************************************************************************/
void PendSV(void)
{
    InterruptMasterDisable();
    save_registers();							/* Save registers R4..R11     */
    running[current_priority]->sp = get_PSP();	/* Store current SP in PCB    */
    running[current_priority] = running[current_priority]->next;/* Con-switch */
    set_PSP(running[current_priority]->sp);		/* restore process SP to PSP  */
    restore_registers();						/* Load registers R4..R11     */
    InterruptMasterEnable();
}






/*******************************************************************************
 * Purpose:
 *             Main function of the program.
 * Arguments:
 *             NONE
 * Return :
 *             NONE
 ******************************************************************************/
void main(void)
{
    int i;
    
    init_kernel();
    
    /* reg_proc(process name, id, priority) */
    for (i = 0; i < NUM_OF_PROCESSES; i++)
    {
        reg_proc(processes[i].func, i+SYS_PROC_NUM, processes[i].priority);
    }
    
    /* run first process */
    SVC();
}
