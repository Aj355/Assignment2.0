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
#define ALL             0xFF        /* all switches or all trains as trgt */

#define STX             0x02        /* ASCII start-of-xmit control byte   */
#define ETX             0x03        /* ASCII end-of-xmit   control byte   */
#define DLE             0x10        /* ASCII Data Link esc control byte   */

#define WINDOW_SIZE     2           /* Maximum packets xmitted before ack */
#define SWITCH_NUM      6           /* Number of switches within trainset */
#define TRAIN_NUM       2           /* Number of trains moving within set */


enum PktType {DATA, ACK, NACK};     /* Packet type */
enum Direction {CW,CCW};            /* Locomotive direction */
enum Switch {STRAIGHT,DIVERTED};    /* Switch direction */


struct message
{
    union {
        struct{
            unsigned char code; /* Message code (described below) */
            unsigned char arg1; /* First argument (optional) */
            unsigned char arg2; /* Second argument (optional) */
        };
        unsigned long message;
    };

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
    union {
        struct{
            struct control ctr;
            unsigned char  len;
            struct message msg;
        };
        unsigned long pkt;
    };
};



struct frame
{
    union {
        struct{
            unsigned char start_of_xmit;
            struct packet pkt;
            unsigned char Chksum;
            unsigned char end_of_xmit;
        };
        unsigned long frame;
    };

};

/* List of UART display commands */
struct frame_queue
{
    struct frame queue[WINDOW_SIZE];            /* UART queue of requests */
    int head;                                   /* Head of circular queue */
    int tail;                                   /* Tail of circular queue */
    volatile int cnt;                           /* Number of rqs in queue */
};

/* External variables*/
extern unsigned NR;
extern unsigned NS;
extern struct packet temp_pkt;
extern struct frame_queue FQ;


void reset_hall_queue(void);
void send_sw(unsigned char switch_num, enum Switch dir);
void send_md(unsigned char train_num, unsigned mag, enum Direction dir);
void hall_sensor_ack(unsigned char sensor_num);


#endif /* TRAINSET_H_ */
