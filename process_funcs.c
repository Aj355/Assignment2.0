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
    //print_str("I ");
    int x = pbind(1);
    print_char(x+'0');
    print_char(pbind(0)+'1');
    pnice(1);
    //print_str("am");
    while (1);
}

void proc2 (void)
{
    print_str("and");
    while(1); //
}

void idle (void)
{
    while (1);
}
