/******************************************************************************/
/* Filename: UART.c
 * Author: Abdullah Alhadlaq
 * Date: 25 Sep 2017
 * Purpose: This file contains the initialization routines for the UART module.
 *          It also has the UART ISR
 * Disclaimer: A large portion of this file is taken from the ECED4402 course
 *             website.
 * Edits:
 * 27 Sep 2017: -Added InterruptMasterEnable function
 *              -Allowed ISR to support Queue module
 *
 * 28 Sep 2017: -Support for BUSY|ISLE UART states
 * 24 Oct 2017: -Added print_char and print_str funcs
 */
/******************************************************************************/

#include "UART.h"
#include "Queue.h"

/* Globals */
volatile int UART_state;    /* BUSY|IDLE */

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

void InterruptEnable(unsigned long InterruptIndex)
{
/* Indicate to CPU which device is to interrupt */
if(InterruptIndex < 32)
    // Enable the interrupt in the EN0 Register
    NVIC_EN0_R = 1 << InterruptIndex;
else
    // Enable the interrupt in the EN1 Register
    NVIC_EN1_R = 1 << (InterruptIndex - 32);
}

void UART0_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
}

void UART0_IntHandler(void)
{
/*
 * Simplified UART ISR - handles receive and xmit interrupts
 * Application signalled when data received
 */
    struct entry x; // temporary entry to hold information

    if (UART0_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt and make char available to application */
        UART0_ICR_R |= UART_INT_RX;
        // fill the entry with the incoming information
        x.type = UART;
        x.character = UART0_DR_R;
        //enqueue the entry in the input queue for the control module
        enqueue(INPUT, x);
    }

    if (UART0_MIS_R & UART_INT_TX)
    {
        /* XMIT done - clear interrupt */
        UART0_ICR_R |= UART_INT_TX;

        // if there is an entry in the output queue, dequeue it
        if (dequeue(OUTPUT, &x))
            UART0_DR_R = x.character;   // output the character
        else // if the output queue is empty, then set the state to IDLE
            UART_state = IDLE;
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

/* This function takes a character and prints it
 * using the UART module
 * Arguments:
 *      prnt: The character to be printed
 * Returns:
 *      NONE
 */
void print_char (char prnt)
{
    struct entry temp;  // temporary entry to hold information

    if (UART_state == BUSY) //IF the UART_state is busy THEN
    {
        //Enqueue the character to be sent on the output queue
        temp.character = prnt;
        enqueue(OUTPUT, temp);
        // no need to fill the type because it can only be UART
        // (there is no SYSTICK for output)
    }
    else    // if the UART is IDLE
    {
        // set state to be busy and put the character to be sent in UART0 data register
        UART_state = BUSY;
        UART0_DR_R = prnt;
    }
}

/* This function prints a string to the user using the UART
 * module by making use of the print_char function.
 * Arguments:
 *      str: string to be printed to the user
 * Returns:
 *      NONE
 */
void print_str (char *str)
{
    while (*str)    // while it is not null
    {
        print_char(*str);   // print
        str++;  // go to the next character
    }
}

void init_UART (void)
{
    /* Initialize UART */
    UART0_Init();                               // Initialize UART0
    InterruptEnable(INT_VEC_UART0);             // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable RCV & Xmit interrupts
}
