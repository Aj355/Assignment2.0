/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   12  Nov  2017
 * Editing       :   15  Sept - Disable interrupt upon entry and enable upon
 *                                  leaving
 * Submission date : 15 Nov 2017
 * File name : System_procs.c
 * Purpose: Define data structures that aid in waking and putting processes to
 *              sleep. System processes entry points are also declared here.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */
#ifndef SYSTEM_PROCS_H_
#define SYSTEM_PROCS_H_


/* structure to manage sleep requesting processes */
struct sleeping_proc
{
    int mailbox_id;             // of the sleeping proc
    unsigned long counter;      // number of systicks asleep
    struct sleeping_proc *next;
};

/* System processes entry points */
void enqueue_sleep(struct sleeping_proc *req);
void time_server (void);
void idle (void);
void reserved1 (void);
void reserved2 (void);
void reserved3 (void);


#endif /* SYSTEM_PROCS_H_ */
