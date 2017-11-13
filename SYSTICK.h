/******************************************************************************/
/* Filename: SYSTICK.h
 * Author: Abdullah Alhadlaq
 * Date: 25 Sep 2017
 * Purpose: This file contains the definitions, external global variables and
 *          function prototypes for the SYSTICK module.
 * Disclaimer: A large portion of this file is taken from the ECED4402 course
 *             website.
 * Edits:
 * 29 Sep 2017: -Added the USED_PERIOD definition
 *              -Added support for time drifting
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
#ifndef SYSTICK_H_
#define SYSTICK_H_

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

// Maximum period
#define MAX_WAIT       0x1000000   /* 2^24 */
#define USED_PERIOD    16384       /* 2^14 this will give 1 sec per 1024 ticks */
#define T_ADJ_SZ       5          // size of the t_adj array

#define TRUE    1
#define FALSE   0

/* Global variables */
extern int t_adj [];    // array used for adjusting time drift
extern volatile int t_adj_cntr; // counter for the t_adj array
extern volatile int t_adj_indx;   // index for the t_adj_array

/* function prototypes */
void SysTickStart(void);
void SysTickStop(void);
void SysTickPeriod(unsigned long Period);
void SysTickIntEnable(void);
void IntMasterEnable(void);
void SysTickHandler(void);
void SysTickIntDisable(void);
void init_systick(void);

#endif /* SYSTICK_H_ */
