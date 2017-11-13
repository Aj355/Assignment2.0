/*
 * system_procs.h
 *
 *  Created on: Nov 12, 2017
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
#ifndef SYSTEM_PROCS_H_
#define SYSTEM_PROCS_H_


struct sleeping_proc
{
    int mailbox_id;             // of the sleeping proc
    unsigned long counter;      // number of systicks asleep
    struct sleeping_proc *next;
};

void enqueue_sleep(struct sleeping_proc *req);
void time_server (void);


#endif /* SYSTEM_PROCS_H_ */
