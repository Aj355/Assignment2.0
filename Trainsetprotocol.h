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
#define ETR             0x01        /* Number of the express locomotive   */
#define LTR             0X02        /* Number of the local locomote       */

#define STX             0x02        /* ASCII start-of-xmit control byte   */
#define ETX             0x03        /* ASCII end-of-xmit   control byte   */
#define DLE             0x10        /* ASCII Data Link esc control byte   */

#define WINDOW_SIZE     2           /* Maximum packets xmitted before ack */
#define SWITCH_NUM      6           /* Number of switches within trainset */
#define TRAIN_NUM       2           /* Number of trains moving within set */
#define MSG_BYTE        3           /* Number of bytes within one message */
#define PKT_BYTE        5           /* Number of bytes within one packet  */
#define FRM_BYTE        8           /* Number of bytes within one frame   */


enum PktType {DATA, ACK, NACK};         /* Packet type */
enum Direction {CW,CCW};                /* Locomotive direction */
enum Switch {STRAIGHT,DIVERTED};        /* Switch direction */



/* Message structure */
struct __attribute__((packed)) m
{
    unsigned char code;         /* Message code (described below) */
    unsigned char arg1;         /* First argument (optional) */
    unsigned char arg2;         /* Second argument (optional) */

};

struct message
{
    union
    {
        unsigned long message;          /* message viewed as a single unit */
        struct __attribute__((packed))
        {
            unsigned char code;         /* Message code (described below) */
            unsigned char arg1;         /* First argument (optional) */
            unsigned char arg2;         /* Second argument (optional) */

        };

    };
};


/* magnitude/direction structure */
struct mag_dir
{
    union {
        struct __attribute__((packed)){
            unsigned magnitude : 3;     /* 0 – stop through 7 – maximum */
            unsigned ignored : 4;       /* Zero */
            unsigned direction : 1;     /* 1 for CCW and 0 for CW */
        };
        unsigned char mag_dir;          /* argument viewed as one byte */
    };

};


/* */
struct control
{
    union {
        struct __attribute__((packed)){
            unsigned nr : 3;            /* Response number */
            unsigned ns : 3;            /* sequence number */
            enum PktType type : 2;      /* Packet type */
        };
        unsigned char cntrl;            /* control viewed as a byte */
    };
};



/* */
struct __attribute__((packed)) pk
{
    struct control ctr;         /* Control field */
    unsigned char  len;         /* Length  field */
    struct m msg;               /* Message field */
};
struct packet
{
    union {
        struct __attribute__((packed))
        {
            struct control ctr;         /* Control field */
            unsigned char  len;         /* Length  field */
            struct m msg;               /* Message field */
        };
        unsigned long long pkt;         /* packet viewed as a single unit */
        char packets[5];                /* packet viewed as a series of bytes */
    };
};



/* */
struct frame
{
    union {
        struct __attribute__((packed)){
            unsigned char start_xmit;   /* start of transmission*/
            struct pk pkt;              /* packet field */
            unsigned char Chksum;       /* checksum field */
            unsigned char end_xmit;     /* end of transmission */
        };
        unsigned long long frame;            /* frame viewed as a single unit */
        char frames[8];                 /* frame viewed as a series of bytes */
    };
    //int counter;
    //int escaped;
};


/* List of frames to be send by the physical layer */
struct frame_queue
{
    struct frame queue[12];             /* frame queue */
    int head;                           /* Head of circular queue */
    int tail;                           /* Tail of circular queue */
    volatile int cnt;                   /* Number of rqs in queue */
};

/* List of frames to be send by the physical layer */
struct packet_queue
{
    struct packet queue[12];             /* frame queue */
    int head;                           /* Head of circular queue */
    int tail;                           /* Tail of circular queue */
    volatile int cnt;                   /* Number of rqs in queue */
};


/* External variables*/
extern unsigned NR;
extern unsigned NS;
extern int counter;
extern struct current_frame_send;
extern struct packet temp_pkt;
extern struct frame_queue FQ;


void reset_hall_queue(void);
void send_sw(unsigned char switch_num, enum Switch dir);
void send_md(unsigned char train_num, unsigned mag, enum Direction dir);
void hall_sensor_ack(unsigned char sensor_num);
void send_frame (struct frame );
void DLL (void);
#endif /* TRAINSET_H_ */
