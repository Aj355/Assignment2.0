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
 * ------------------------------------------------------------------------- */

#include "UART.h"
#include "process_support.h"
#include "process_funcs.h"
#include "kernel_calls.h"
#include "Pcommands.h"

#define SVC()   __asm(" SVC #0")

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
    reg_proc(idle, 0, 0);
    reg_proc(proc1, 1, 3);
    reg_proc(proc2, 2, 2);
    reg_proc(proc3, 3, 1);
    reg_proc(proc4, 4, 3);


    /* run first process */
    SVC();

	return ;
}
