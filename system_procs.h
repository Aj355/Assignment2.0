/*
 * system_procs.h
 *
 *  Created on: Nov 12, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef SYSTEM_PROCS_H_
#define SYSTEM_PROCS_H_

#define SYSTICK_ID  100 // signals that incoming message is from systick

struct sleeping_proc
{
    int mailbox_id;             // of the sleeping proc
    unsigned long counter;      // number of systicks asleep
    struct sleeping_proc *next;
};


#endif /* SYSTEM_PROCS_H_ */
