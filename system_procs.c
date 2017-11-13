/*
 * system_procs.c
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
#include <stdio.h>
#include <stdlib.h>
#include "Pcommands.h"
#include "system_procs.h"

struct sleeping_proc *sleep_list;



void enqueue_sleep(struct sleeping_proc *req)
{
    struct sleeping_proc *entry;
    struct sleeping_proc *ptr;

    /* Construct entry */
//    InterruptMasterDisable();                    // Disable all interrupt
    entry = (struct sleeping_proc *) malloc(sizeof(struct sleeping_proc));
//    InterruptMasterEnable();                     // Enable all interrupts
    entry -> mailbox_id = req -> mailbox_id;
    entry -> counter = req -> counter;

    /* Add device to head of the queue */
    if (sleep_list == NULL || /* if the queue is empty */
        /* if it has the soonest status change time*/
        entry -> counter < sleep_list -> counter)
    {
        entry -> next = sleep_list;
        sleep_list = entry;
    }
    else
    {
        ptr = sleep_list;
        /* Traverse the list as long as list is not empty AND SC time is lower
         than next entry OR priority is higher if time is equal */
        while (ptr -> next != NULL && (entry -> counter > ptr -> next ->counter ))
        {
            ptr = ptr -> next;
        }
        /* add entry to after node ptr */
        entry -> next = ptr -> next;
        ptr -> next = entry;
    }
}

void time_server (void)
{
    int src_id;
    unsigned long global_counter=0;
    //char tmp[MAX_MSG_SZ];
    struct time_req tmp;
    struct sleeping_proc *tmp_entry;
    pbind(0);
    while (1)
    {
        // get a message from the mailbox
        precv(&src_id, &tmp, sizeof(struct time_req));

        // if it is from systick, then increment the counter
        if (src_id == TIME_SERVER)
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
            if (tmp.code == _TIME)
            {
                //send the global counter to the mailbox of the requesting process
                psend(src_id, &global_counter, sizeof(unsigned long));
            }

            else // if it is a sleep request
            {
                tmp_entry = malloc (sizeof(struct sleeping_proc));
                tmp_entry->mailbox_id = src_id;
                tmp_entry->counter = tmp.counter + global_counter;
                enqueue_sleep(tmp_entry);
            }
        }
    }



}
