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
 * File name : main.c
 * Purpose: Implement a static circular queue in order to organize interrupts
 *              According to their type (UART or SYSTICK)
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#include<stdio.h>
#include "UART.h"
#include "process_support.h"
#include "process_funcs.h"
#include "kernel_calls.h"
#include "Pcommands.h"
#include "system_procs.h"
#include "system_requests.h"


#define SVC()   __asm(" SVC #0")
#define SYS_PROC_NUM    5


void init_display ()
{
    //print_str("\e[36m"); // Enable the Cyan font colour
    //print_str("\e[3J");  // Clear to top of terminal
    //print_str("\e[;H");  // Cursor goes to top left
}

void main(void)
 {
    init_kernel();
    //init_display ();
    /* reg_proc(process name, id, priority) */
    int i;
    for (i = 0; i < NUM_OF_PROCESSES/* sizeof(processes)/sizeof(processes[0]) */; i++)
    {
        reg_proc(processes[i].func, i+SYS_PROC_NUM, processes[i].priority);
    }
    //reg_proc(proc1, 2, 3);
    //reg_proc(proc2, 3, 3);
    //reg_proc(proc3, 4, 1);
    //reg_proc(proc4, 5, 3);


    /* run first process */
    SVC();

	return ;
}
