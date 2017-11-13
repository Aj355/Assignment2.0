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
    int size;
    int src_id;
    char h[10];
    pdisplay_str(1,1,"proc1");
    //pbind(1);
    //size = precv(&src_id,h,10);
    while(1);

}

void proc2 (void)
{
    int size;
    pdisplay_str(1,2,"proc2");
    //pbind(2);
    //size = psend(1,"hello",6);
    while (1);

}

void idle (void)
{
    while (1);
}
