/******************************************************************************/
/* Filename: SYSTICK.c
 * Author: Abdullah Alhadlaq
 * Date: 25 Sep 2017
 * Purpose: This file contains the initialization routines, as well as the
 *          IST for the SYSTICK module.
 * Disclaimer: A large portion of this file is taken from the ECED4402 course
 *             website.
 * Edits:
 * 27 Sep 2017: -Added support for the queue module
 *
 * 29 sep 2017: -Added support for time drifting
 */
/******************************************************************************/
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
#include "SYSTICK.h"
#include "Queue.h"
#include "Pcommands.h"

#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV 0x10000000

/* This an array used to adjust for time drifting
 * an average of 102.4 ticks is needed to generate
 * one-tenth of a second. Therefore, it is used to increment
 * the time of day and stop watch counters.
 */
int t_adj [] = {102, 102, 103, 102, 103};
volatile int t_adj_cntr=0; // counter for the t_adj array
volatile int t_adj_indx=0;   // index for the t_adj_array


void SysTickStart(void)
{
// Set the clock source to internal and enable the counter to interrupt
ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
// Clear the enable bit to stop the counter
ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
/*
 For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
*/
ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
// Set the interrupt bit in STCTRL
ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
// Clear the interrupt bit in STCTRL
ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

void IntMasterEnable(void)
{
// enable CPU interrupts
__asm(" cpsie   i");
}

void SysTickHandler(void)
{
    struct msg_request tmp;

    // increment the time adjust counter
    t_adj_cntr ++;

    // if it equals the current number in the t_adj array
    if (t_adj_cntr == t_adj[t_adj_indx])
    {
        // increment the time adjust index
        t_adj_indx = (t_adj_indx+1) % T_ADJ_SZ;
        // reset the counter
        t_adj_cntr = 0;

        // enqueue the entry in the input queue
    }

    tmp.id = TIME_SERVER;
    tmp.sz = 0;

    ksend(&tmp);

    /* Signal that the PendSV handler is to be called on exit */
    NVIC_INT_CTRL_R |= TRIGGER_PENDSV;
}

void init_systick (void)
{
    /* initialize SYSTICK */
    SysTickPeriod(USED_PERIOD);
    SysTickIntEnable();
    SysTickStart();
}

