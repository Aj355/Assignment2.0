/*
 * process_funcs.c
 *
 *  Created on: Oct 24, 2017
 *      Author: AbdullahAlhadlaq
 */


#include "UART.h"
#include "Pcommands.h"

void process (void)
{
    print_str("I am a process\n\r");
    while(1);
}


void proc1 (void)
{
    pbind(1);
    psend(1, "hello", 6);
    while (1);
}

void proc2 (void)
{
    pbind(2);
    while(1); //
}

void idle (void)
{
    while (1);
}
