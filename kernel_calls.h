
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
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */
#ifndef KERNEL_CALLS_H_
#define KERNEL_CALLS_H_



/* function prototypes  */
void init_kernel (void);
//void SVCHandler(struct stack_frame *argptr);
void SVCall(void);
void PendSV (void);

#endif /* KERNEL_CALLS_H_ */
