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
                {
                    sleep_list = sleep_list->next;
                }
                free(tmp_entry);
            }
        }

        else // if message is from a process
        {
            // if it is a time request
            if (tmp[0] == _TIME)
            {
                //send the global counter to the mailbox of the requesting process
                sprintf(tmp, "%l", global_counter);
                psend(src_id, tmp, sizeof(tmp));
            }

            else // if it is a sleep request
            {
                tmp_entry = malloc (sizeof(struct sleeping_proc));
                tmp_entry->mailbox_id = (int)tmp[1];
                tmp_entry->counter = strtol((tmp+2), NULL, 10) + global_counter;

                /*
                //enqueue the entry in the correct position
                if (!sleep_list)            // if list is empty
                    sleep_list = tmp_entry; // become the first entry
                else                        // if list is not empty
                {
                    tmp_entry->next = sleep_list;                       // look at the first entry
                    while(1)
                    {
                        if (tmp_entry->counter < tmp_entry->next->counter)  // if it is smaller than the one it is currently looking at
                        {
                            if (tmp_entry->next->prev)                      // if it has a previous entry
                            {
                                tmp_entry->next->prev->next = tmp_entry;    // connect the previous entry
                                tmp_entry->prev = tmp_entry->next->prev;
                            }
                            tmp_entry->next->prev = tmp_entry;
                            break;
                        }
                        if (tmp_entry->next->next)
                            tmp_entry->next = tmp_entry->next->next;
                        else
                        {
                            tmp_entry->prev = tmp_entry->next;
                            tmp_entry->next->next = tmp_entry;
                            tmp_entry->next = NULL;
                            break;
                        }
                    }
                }
                */
            }
            // enqueue_slp(src_id, counter+global_counter);
        }
    }



}
