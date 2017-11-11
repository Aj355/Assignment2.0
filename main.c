

/**
 * main.c
 */

#include "UART.h"
#include "process_support.h"
#include "process_funcs.h"
#include "kernel_calls.h"
#include "Pcommands.h"

void main(void)
 {
    init_kernel();

    /* reg_proc(process name, id, priority) */
    reg_proc(idle, 0, 0);
    reg_proc(proc1, 1, 3);
    reg_proc(proc2, 2, 1);

    /* run first process */
    SVC();

	return ;
}
