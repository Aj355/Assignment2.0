
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
#ifndef PROCESS_SUPPORT_H_
#define PROCESS_SUPPORT_H_

#define SUCCESS            1
#define FAIL           -1
#define PRIORITY_NUM    4   /* idle, low, medium and high priorities. */
#define STACKSIZE       1024
#define MAX_PROCESS     32
#define PRIVATE static

#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP


enum PR_TYPE {IDLE_, LOW_, MED_, HIGH_};

void volatile save_registers();
void volatile restore_registers();


/* Cortex default stack frame */
/* Registers saved by s/w (explicit) */
/* There is no actual need to reserve space for R4-R11, other than
 * for initialization purposes.  Note that r0 is the h/w top-of-stack.
 */
struct stack_frame
{
    unsigned long r4;
    unsigned long r5;
    unsigned long r6;
    unsigned long r7;
    unsigned long r8;
    unsigned long r9;
    unsigned long r10;
    unsigned long r11;
    /* Stacked by hardware (implicit)*/
    unsigned long r0;
    unsigned long r1;
    unsigned long r2;
    unsigned long r3;
    unsigned long r12;
    unsigned long lr;
    unsigned long pc;
    unsigned long psr;
};

/* Process control block */
struct pcb
{
    unsigned long sp;          /* Stack pointer - r13 (PSP) */
    unsigned id;               /* process identification */
    unsigned short priority;   /* priority level of process (0, 1, 2, 3) */
    unsigned long *stack_addr; /* address given by malloc (used to free the stack) */
    int mailbox_num;  /* mailbox number of process. -1 if unbound */
    /* Links to adjacent PCBs */
    struct pcb *next;
    struct pcb *prev;
};

void set_LR  (volatile unsigned long);
void set_PSP (volatile unsigned long);
void set_MSP (volatile unsigned long);
void assignR7(volatile unsigned long);

unsigned long get_PSP(void);
unsigned long get_MSP(void);
unsigned long get_SP (void);

int reg_proc( void (*func)(void), unsigned id, unsigned short priority);
int enqueue_pcb(struct pcb*);
void dequeue_running_pcb(void);

/* externals */
extern struct pcb  *running[];
extern int current_priority;             /* Current priority level */
extern int pcb_counter;                  /* keeps track of the number of WTR processes */

#endif /* PROCESS_SUPPORT_H_ */
