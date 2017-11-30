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
#include <stdlib.h>
#include "Queue.h"
#include "UART.h"
#include "Kcommands.h"
#include "kernel.h"
#include "Trainsetprotocol.h"
#include "processes.h"

#define Window_size         2

struct packet temp_pkt;
struct frame  temp_frm;
unsigned nr;
unsigned ns;
/*******************************************************************************
* Purpose:
*             This process inserts a PCB into its corresponding priority
*             WTR queue and adjusts the current_priority global variable
*             if needed.
* Arguments:
*             in_pcb:   process control block to be inserted
* Return :
*             SUCCESS   if successful enqueuing of the PCB occurs
*             FAIL      if the WTR queue is full
*******************************************************************************/
int construct_packet(struct message *msg, enum PktType type)
{
    switch (type)
    {
    case DATA:
        temp_pkt.ctr.nr = nr;
        temp_pkt.ctr.ns = ns;
        ns = (ns + 1) % 8;
        temp_pkt.ctr.type = type;
        temp_pkt.len = sizeof(*msg);
        temp_pkt.msg = msg;
        break;

    case ACK: /* FALL-THROUGH */
    case NACK:
        temp_pkt.ctr.nr = nr;
        temp_pkt.ctr.type = type;
        break;

    default:
        return FALSE;
    }
    return SUCESS;
}

/*******************************************************************************
* Purpose:
*             This process removes a PCB from its corresponding priority
*             WTR queue and adjusts the current_priority global variable
*             if needed. This function does not free the PCB.
* Arguments:
*             NONE (only dequeues the running processes pcb)
* Return :
*             NONE
*******************************************************************************/
int construct_frame(struct packet pkt)
{
    temp_frm.start_of_xmit = STX;
    temp_frm.pkt = pkt;
    temp_frm.Chksum  = pkt.ctr;
    temp_frm.Chksum += pkt.len;
    temp_frm.Chksum += pkt.msg.code;
    temp_frm.Chksum += pkt.msg.arg1;
    temp_frm.Chksum += pkt.ctr.arg2;
    temp_frm.end_of_xmit = ETX;

}


/* -------------------------------------------------------------------------- *
 * Purpose:       remove an element from the UART queue
 * Arguments:
 *                req: UART entry to be dequeued
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
int UART1(struct UART_entry *req)
{

}

/* -------------------------------------------------------------------------- *
 * Purpose:       Insert a message request into a mailbox
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
int er(struct msg_request * msg)
{

}

/* -------------------------------------------------------------------------- *
 * Purpose:       send speed and direction
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
void send_md(unsigned char train_num, unsigned mag, enum Direction dir)
{
    struct message msg;
    struct mag_dir md;
    md.direction = dir;
    md.ignored   = 0;
    md.magnitude = mag;
    msg.code = CHNG_SPDR_MSG;
    msg.arg1 = train_num;
    msg.arg2 = md.mag_dir;
    psend(12,&msg,sizeof(struct message));
}

/* -------------------------------------------------------------------------- *
 * Purpose:       send speed and direction
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
void send_sw(unsigned char switch_num, enum Switch dir)
{
    struct message msg;
    msg.code = CHNG_SWTC_MSG;
    msg.arg1 = switch_num;
    msg.arg2 = (unsigned char) dir;
    psend(12,&msg,sizeof(struct message));
}


/* -------------------------------------------------------------------------- *
 * Purpose:       send speed and direction
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
void reset_hall_queue(void)
{
    struct message msg;
    msg.code = HALL_REST_MSG;
    msg.arg1 = 0;
    msg.arg2 = 0;
    psend(12,&msg,sizeof(struct message));
}

/* -------------------------------------------------------------------------- *
 * Purpose:       send speed and direction
 * Arguments:
 *                msg:  message request
 * Returns:
 *                TRUE  if dequeuing is successful
 *                FALSE if dequeuing is not successful
 * -------------------------------------------------------------------------- */
void hall_sensor_ack(unsigned char sensor_num)
{
    struct message msg;
    msg.code = HALL_TRGR_ACK;
    msg.arg1 = sensor_num;
    msg.arg2 = 0;
    psend(12,&msg,sizeof(struct message));
}


/* -------------------------------------------------------------------------- *
 * Purpose:       Insert an entry into the UART queue
 *
 * Arguments:
 *                req:  uart entry to be enqueued
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
void DLL(void)
{
    pbind(12);
}
