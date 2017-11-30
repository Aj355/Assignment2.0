/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date assigned :   26  Sept  2017
 * Date created  :   12  Oct  2017
 * Editing       :   28  Oct  - Added enqueue/dequeue messages
 *                   01  Nov  - Added enqueue/dequeue UART
 * Submission date : 15  Nov 2017
 * File name : Queue.h
 * Purpose: Implement a static circular queue in order to manage UART and 
 *              messaging system queues.
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#ifndef TRAINSET_H_
#define TRAINSET_H_

#include "process_support.h"
#include "Pcommands.h"
#include "Kcommands.h"

/* Trainset message codes */
#define HALL_TRGR_MSG   0xA0        /* Train triggering hall sensor event */
#define HALL_TRGR_ACK   0xA2        /* Ack of Hall sensor trigger event   */
#define HALL_REST_MSG   0xA8        /* reset Queue of Hall trigger event  */
#define HALL_REST_ACK   0xAA        /* Ack of Hall sensor queue resetting */
#define CHNG_SPDR_MSG   0xC0        /* Change train speed and direction   */
#define CHNG_SPDR_ACK   0xC2        /* Ack of Speed/Direction change msg  */
#define CHNG_SWTC_MSG   0xE0        /* Change switch state message        */
#define CHNG_SWTC_ACK   0xE2        /* Ack of change switch state message */

#define STX             0x02        /* ASCII start-of-xmit control byte   */
#define ETX             0x03        /* ASCII end-of-xmit   control byte   */
#define DLE             0x10        /* ASCII Data Link esc control byte   */


enum PktType {DATA, ACK, NACK};
enum Direction {CW,CCW};
enum Switch {STRAIGHT,DIVERTED};

struct message
{
    unsigned char code; /* Message code (described below) */
    unsigned char arg1; /* First argument (optional) */
    unsigned char arg2; /* Second argument (optional) */
};


struct mag_dir
{
    union {
        struct{
            unsigned magnitude : 3;   /* 0 – stop through 7 – maximum */
            unsigned ignored : 4;     /* Zero */
            unsigned direction : 1;   /* 1 for CCW and 0 for CW */
        };
        unsigned char mag_dir;
    };

};


struct control
{
    union {
        struct{
            unsigned nr : 3;
            unsigned ns : 3;
            enum PktType type : 2;
        };
        unsigned char cntrl;
    };
};

struct packet
{
    struct control ctr;
    unsigned char  len;
    struct message msg;
};

struct frame
{
    unsigned char start_of_xmit;
    struct packet pkt;
    unsigned char Chksum;
    unsigned char end_of_xmit;
};

/* External variables*/
extern unsigned NR;
extern unsigned NS;
extern struct packet temp_pkt;
#endif /* TRAINSET_H_ */
