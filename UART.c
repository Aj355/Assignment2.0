/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   29  Oct  2017
 * Submission date : 15 Nov 2017
 * File name : UART.c
 * Purpose: This file contains the initialization routines for the UART module.
 *          It also has the UART ISR
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */
#include "UART.h"
#include "Queue.h"
#include "Kcommands.h"
#include "kernel.h"
#include "Trainsetprotocol.h"
#include "processes.h"

#define MAX_LEN     32

/* Globals */
volatile int UART_state;                /* BUSY|IDLE */
volatile int UART1_state;               /* BUSY|IDLE */
struct UART_entry current_msg;          /* message currently being printed */

void UART0_Init(void)
{
    volatile int wait;

    UART_state = IDLE;  // initialize UART_state to be in IDLE mode

    /* Initialize UART0 */
    // Enable Clock Gating for UART0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;
    // Enable Clock Gating for PORTA
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_IBRD_R = 8;
    // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556
    UART0_FBRD_R = 44;

    // WLEN: 8, no parity, one stop bit, without FIFOs)
    UART0_LCRH_R = (UART_LCRH_WLEN_8);
    // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_AFSEL_R = 0x3;
    // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);
    // Enable Digital I/O on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;

    UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself.
}

void UART1_Init(void)
{
    volatile int wait;
    
    UART1_state = IDLE;  // initialize UART_state to be in IDLE mode
    
    /* Initialize UART1 */
    // Enable Clock Gating for UART0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOB;
    // Enable Clock Gating for PORTA
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART1;
    wait = 0; // give time for the clocks to activate
    
    UART1_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs
    
    // Setup the BAUD rate
    // IBRD = int(16,000,000 / (16 * 9600)) = 104 + 1/6
    UART1_IBRD_R = 104;
    // FBRD = int(1/6 * 64 + 0.5) = 11 + 1/6
    UART1_FBRD_R = 11;
    
    // WLEN: 8, no parity, one stop bit, without FIFOs)
    UART1_LCRH_R = (UART_LCRH_WLEN_8);
    // Enable Receive and Transmit on PA1-0
    GPIO_PORTB_AFSEL_R = 0x3;
    // Enable UART RX/TX pins on PA1-0
    GPIO_PORTB_PCTL_R = (0x01) | ((0x01) << 4);
    // Enable Digital I/O on PA1-0
    GPIO_PORTB_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;
    
    UART1_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself.
}

void InterruptEnable(unsigned long InterruptIndex)
{
/* Indicate to CPU which device is to interrupt */
if(InterruptIndex < 32)
    // Enable the interrupt in the EN0 Register
    NVIC_EN0_R |= 1 << InterruptIndex;
else
    // Enable the interrupt in the EN1 Register
    NVIC_EN1_R |= 1 << (InterruptIndex - 32);
}

void UART_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
    UART1_IM_R |= flags;
}


void UART0_IntHandler(void)
{
/*
 * Simplified UART ISR - handles receive and xmit interrupts
 * Application signalled when data received
 */
    struct msg_request tmp;
    char in_char;

    if (UART0_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt and make char available to application */
        UART0_ICR_R |= UART_INT_RX;

        // fill the entry with the incoming information
        in_char = UART0_DR_R;
        tmp.dst_id = VERTUAL_TRN; /* Destination */
        tmp.sz = sizeof(char);    /* Size of msg */
        tmp.src_id = UART;        /* Source is UART (unique ID) */
        tmp.msg = &in_char;       /* input character */
        save_registers();         /* incase send wakes up higher priority proc*/
        ksend(&tmp);              /* send input character */
        restore_registers();      /* restore registers for same reason of sv  */
    }

    if (UART0_MIS_R & UART_INT_TX)
    {

        /* XMIT done - clear interrupt */
        UART0_ICR_R |= UART_INT_TX;
        // if the current message is not done
        if (*(current_msg.dsp_msg))
        {
            UART0_DR_R = *(current_msg.dsp_msg);
            current_msg.dsp_msg++;
        }
        else // if the current message is done xmit
        {
            save_registers();  /* needed incase process is has higher priority*/
            running[current_priority]->sp = get_PSP(); /* save current psp    */
            
            enqueue_pcb(current_msg.proc);             /* Unblock the process */
            
            set_PSP(running[current_priority]->sp);    /* load new psp, if any*/
            restore_registers();/* needed incase proc is has higher priority  */
            
            // if there is an entry in the UART list
            if (dequeue_UART(&current_msg))
            {
                UART0_DR_R = *(current_msg.dsp_msg);
                current_msg.dsp_msg++;
            }
            else // if the list is empty
                UART_state = IDLE;
        }
    }
}

struct transmit recv;
int escaped;
int length;
int chksum;

struct frame send;
int counter;


void UART1_IntHandler(void)
{
    struct msg_request tmp;
    /*
     * Simplified UART ISR - handles receive and xmit interrupts
     * Application signalled when data received
     */

    if (UART1_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt and make char available to application */
        UART1_ICR_R |= UART_INT_RX;

        if ( !length && UART1_DR_R == STX)
        {
            chksum = 0;
            length++;
        }
        else if (length)
        {
            if (escaped)
            {
                recv.xmit[length++] = UART1_DR_R;
                chksum +=UART1_DR_R;
                escaped = 0;
            }
            else
            {
                switch (UART1_DR_R)
                {
                case STX:
                    length = 0;
                    break;
                case DLE:
                    escaped = 1;
                    break;
                case ETX:
                    if (chksum == 0xff)
                    {
                        tmp.dst_id = DLL; /* Destination */
                        tmp.sz = length -2;               /* Size of msg */
                        recv.length = length -2;
                        tmp.src_id = UART;     /* Source is systick (unique ID) */
                        tmp.msg = &recv.xmit[1];
                        save_registers();         /* incase send wakes up higher priority proc*/
                        ksend(&tmp);              /* send systick notification of 1/10 second */
                        restore_registers();      /* restore registers for same reason of sv  */
                    }
                    length = 0;
                    break;
                default:
                    recv.xmit[length++] = UART1_DR_R;
                    chksum +=UART1_DR_R;
                }
            }
            if (length > 16)
                length = 0;
        }
    }
    
    if (UART1_MIS_R & UART_INT_TX)
    {
        
        /* XMIT done - clear interrupt */

        UART1_ICR_R |= UART_INT_TX;
        /*  if  current frame is not done sending*/
        if (counter <= send.length)
            UART1_DR_R = send.bytes[counter++];  // Load character into data reg.

        else
        {
            counter = 0;
            if (FQ.cnt > 0)
            {
                dequeue_frame(&send);
                UART1_DR_R = send.bytes[counter++];  // Load character into data reg.
            }
            else
                UART1_state = IDLE;
        }
    }
}
void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}

void InterruptMasterDisable (void)
{
    /* Disable CPU Interrupts */
    __asm(" cpsid   i");
}


void init_UART (void)
{
    /* Initialize UART */
    UART0_Init();                                // Initialize UART0
    UART1_Init();                                // Initialize UART1
    NVIC_EN0_R = 0;
    InterruptEnable(INT_VEC_UART0);              // Enable UART0 interrupts
    InterruptEnable(INT_VEC_UART1);              // Enable UART0 interrupts
    UART_IntEnable(UART_INT_RX | UART_INT_TX);   // Enable RCV & Xmit interrupts
}


