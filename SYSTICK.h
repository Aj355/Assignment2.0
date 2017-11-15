
/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   07  Sept  2017
 * Date created  :   12  Sept  2017
 * Editing       :   15  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 15 Nov 2017
 * File name : SYSTICK.h
 * Purpose: Implement the systick module and necessary data structures.
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
#define USED_PERIOD    16384       /* 2^14 this will give 1 sec per 1024 ticks*/
#define T_ADJ_SZ       5          // size of the t_adj array

#define TRUE    1
#define FALSE   0

/* Global variables */
extern int t_adj [];                // array used for adjusting time drift
extern volatile int t_adj_cntr;     // counter for the t_adj array
extern volatile int t_adj_indx;     // index for the t_adj_array

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
