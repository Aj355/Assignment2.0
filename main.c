

/**
 * main.c
 */

#include "UART.h"
#include "process_support.h"
#include "process_funcs.h"
#include "kernel_calls.h"
#include "Pcommands.h"

#define SVC()   __asm(" SVC #0")

void init_display ()
{
    print_str("\e[36m"); // Enable the Cyan font colour
    print_str("\e[3J");  // Clear to top of terminal
    print_str("\e[;H");  // Cursor goes to top left
}

void main(void)
 {
    init_kernel();
    //init_display ();
    /* reg_proc(process name, id, priority) */
    reg_proc(idle, 0, 0);
    reg_proc(proc1, 1, 3);
    reg_proc(proc2, 2, 1);

    /* run first process */
    SVC();

	return ;
}
