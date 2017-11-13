/*
 * system_procs.h
 *
 *  Created on: Nov 12, 2017
 *      Author: AbdullahAlhadlaq
 */

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
