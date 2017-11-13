/*
 * process_funcs.h
 *
 *  Created on: Oct 24, 2017
 *      Author: AbdullahAlhadlaq
 */

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
#ifndef PROCESS_FUNCS_H_
#define PROCESS_FUNCS_H_

/* function prototypes */
void idle (void);
void process (void);
void proc1 (void);
void proc2 (void);
void proc3 (void);
void proc4 (void);
#endif /* PROCESS_FUNCS_H_ */
