/*
 * system_procs.c
 *
 *  Created on: Nov 12, 2017
 *      Author: AbdullahAlhadlaq
 */
#include <stdio.h>
#include <stdlib.h>
#include "Pcommands.h"
#include "system_procs.h"

struct sleeping_proc *sleep_list;

void time_server (void)
{
    int src_id;
    unsigned long global_counter=0;
    char tmp[MAX_MSG_SZ];
    struct sleeping_proc *tmp_entry;


    while (1)
    {
        // get a message from the mailbox
        precv(&src_id, tmp, MAX_MSG_SZ);

        // if it is from systick, then increment the counter
        if (src_id == SYSTICK_ID)
        {
            global_counter++;
            if (sleep_list && sleep_list->counter == global_counter)
            {
                // send a wake up message to the process
                psend(sleep_list->mailbox_id, NULL, 0);

                // remove the entry from the sleeping list
                tmp_entry = sleep_list;
                if (sleep_list->next)
                    sleep_list = sleep_list->next;
                free(tmp_entry);
            }
        }

        else // if message is from a process
        {
            // if it is a time request
            //if (tmp.code == TIME)

            // enqueue_slp(src_id, counter+global_counter);
        }
    }



}
