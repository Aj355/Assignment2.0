/*
 * process_funcs.c
 *
 *  Created on: Oct 24, 2017
 *      Author: AbdullahAlhadlaq
 */


#include "Pcommands.h"



void proc1 (void)
{
    //int size;
    //int src_id;
    //char h[10];
    pdisplay_str(79,1,"proc1");
    //pbind(1);
    //size = precv(&src_id,h,10);
    while(1);

}

void proc2 (void)
{
    //int size;
    pdisplay_str(5,2,"proc2");
    //pbind(2);
    //size = psend(1,"hello",6);
    while (1);

}

void proc3 (void)
{
    //int size;
    pdisplay_str(1,3,"proc3");
    //pbind(2);
    //size = psend(1,"hello",6);
    while (1);

}


void proc4 (void)
{
    //int size;
    pdisplay_str(1,4,"proc4");
    //pbind(2);
    //size = psend(1,"hello",6);
    while (1);

}

void idle (void)
{
    while (1);
}
