
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
#include "Kprocesses.h"
#include "processes.h"


#define TIME_SERVER_PID_QID		0	/* Queue ID and Process ID of time server */

struct sleeping_proc *sleep_list;   /*contains sleeping PCBs and duration*/

/*******************************************************************************
* Purpose:
*             This function enqueues a sleep request into the sleeping list
* Arguments:
*             req:      sleep request
* Return :
*             NONE
*******************************************************************************/
void enqueue_sleep(struct sleeping_proc *req)
{
    struct sleeping_proc *entry;
    struct sleeping_proc *ptr;

    entry = req;
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

/*******************************************************************************
* Purpose:
*             This is a process and its purpose is to maintain the time by
*             incrementing the global counter every one-tenth of a second
*             every time it receives a message from SYSTICK. It also gives
*             the time to requesting processes and is able to satisfy
*             sleep requests for certain duration of time.
* Arguments:
*             NONE
* Return :
              NONE
*******************************************************************************/
void time_server (void)
{
    int src_id;
    unsigned long global_counter=17;
    struct time_req tmp;
    struct sleeping_proc *tmp_entry;

    pbind(TIME_SERVER);

    while (1)
    {
        // get a message from the mailbox
        precv(&src_id, &tmp, sizeof(struct time_req));

        // if it is from systick, then increment the counter
        if (src_id == SYSTICK)
        {
            global_counter++;
            while (sleep_list && sleep_list->counter == global_counter)
            {
                // send a wake up message to the process
                psend(sleep_list->mailbox_id, NULL, 0);

                // remove the entry from the sleeping list
                tmp_entry = sleep_list;
                sleep_list = sleep_list->next;
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

/*******************************************************************************
* Purpose:
*             This is the IDLE process and it has the lowest priority. This
*             process must will work when either all other processes are
*             blocked or terminated.
* Arguments:
*             NONE
* Return :
*             NONE
*******************************************************************************/
void idle (void)
{
    while (1);
}



