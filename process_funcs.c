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


#include "Pcommands.h"

void proc1 (void)
{
    int size;
    pbind(1);
    psleep();
    psend(4,"HELLO",6);
    while(1);

}

void proc2 (void)
{
    pdisplay_str(1,2,"proc2");
    while (1);

}

void proc3 (void)
{
    pdisplay_str(1,3,"proc3");
    while (1);

}


void proc4 (void)
{
    char h[10];
    int size;
    int src_id;
    pbind(4);
    psleep();
    size = precv(&src_id,h,10);
    pdisplay_str(1,4,h);
    while (1);

}

void idle (void)
{
    while (1);
}
