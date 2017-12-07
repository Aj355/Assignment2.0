/* -------------------------------------------------------------------------- *
 * Author: Abdulrahman  Aljedaibi
 * Author: Abdullah     Alhadlaq
 * Course: Real time systems
 * ECED 4402
 * Date created  :   1  Dec   2017
 * Submission date : 7  Dec   2017
 * File name : Trainsetprotocol.h
 * Purpose: Define data structures and function entry points for train control
 * Acknowledgment: This code is based on the source code provided in class
 * ------------------------------------------------------------------------- */

#ifndef TRAINSET_H_
#define TRAINSET_H_

#include "process_support.h"
#include "Pcommands.h"
#include "Kcommands.h"

/* Trainset message codes */
#define HALL_TRGR_MSG   0xA0        /* Train triggering hall sensor event */
#define HALL_TRGR_ACK   0xAA        /* Ack of Hall sensor trigger event   */
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
#define HALL_SEN_NUM    24          /* Number of hall sensors in system   */
#define SPEC_SENSOR_NUM 6           /* Number of special case sensors     */
#define MAX_INPUT       16          /* allowed input for commands         */
#define MAX_FRAME       16          /* Maximum number of bytes in a frame */
#define MAX_PKT         8           /* Maximum number of bytes in a pkt   */
#define MAX_ELEMENTS    25          /* Max elements in FQ or PQ queues    */

#define CODE            0           /* CODE is the first byte in a message*/
#define ARG1            1           /* ARG1 is the secnd byte in a message*/
#define ARG2            2           /* ARG2 is the third byte in a message*/
#define CTRL            0           /* CTRL is the first byte in a packet */
#define LEN             1           /* LEN is the second byte in a packet */
#define PKT_MSG         2           /* msgs start after two byte in a pkt */

#define DLL             5           /* Data link layer is bound to mbox 5 */
#define APP             6           /* Train Manager   is bound to mbox 6 */
#define TIME_SERVER     0           /* Time server PID and mailbox number */

#define MAP_HEIGHT      13          /* height of the map in lines         */
#define MAP_WIDTH       70          /* width of the map in columns        */
#define MAX_NAME_SZ     3           /* Size of the max name               */
#define FIRST_LINE      1           /* number of first line in terminal   */
#define CMD_LINE_IN     20          /* command line input  position       */
#define CMD_LINE_OUT    23          /* command line output position       */
#define MAP_POS         3           /* starting row position of map       */
#define CHAR_INDEX      '0'         /* add to number to transform to char */
#define EXP_INIT_POS        22      /* initial position of the train      */
#define EXP_INIT_SPEED      7       /* initial speed of the train is max  */
#define EXP_INIT_DEST       24      /* initial destination of the train   */

#define REVERSE_SPD     15          /* speed at which  train is reversing */
#define REVERSE_TIME    3           /* time it takes to reverse the train */



enum PktType {DATA, ACK, NACK};      /* Packet type */
enum Direction {CW,CCW, AT_DST};     /* Locomotive direction */
enum Switch {DIV, STR, UNCH};        /* Switch direction */
enum TRAIN_NAME {LOCAL, EXPRESS};    /* names of the trains */
enum COMMANDS_ {DEST_CMD, HALL_CMD}; /* virtual machine commands */

/* location structure used in map */
struct location
{
    char name[MAX_NAME_SZ];
    unsigned x_pos;
    unsigned y_pos;
};

/* structure used in the table that determines the action to be taken */
struct action
{
    unsigned sw_num:    12;     /* number of the switch */
    unsigned sw_state:  2;      /* (straight | diverted | unchange) */
    unsigned dir:       2;      /* (CW | CCW | At_dst) */
    unsigned pad:       16;     /* padding */
};

/* train structure */
struct train
{
    unsigned head;          /* head position of the train */
    unsigned prev_h;        /* previous head position     */
    unsigned tail;          /* tail position of the train */
    unsigned speed;         /* speed of the train         */
    enum Direction dir;     /* CW | CCW                   */
};





/* magnitude/direction structure */
struct mag_dir
{
    union {
        struct __attribute__((packed)){
            unsigned magnitude : 4;     /* 0 for stop through 7 for maximum */
            unsigned ignored : 3;       /* Zero */
            unsigned direction : 1;     /* 1 for CCW and 0 for CW */
        };
        unsigned char mag_dir;          /* argument viewed as one byte */
    };

};


/* control structure */
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





/* Structure for transmission of messages/packets as sequence of bytes */
struct transmit
{
    union {

        char xmit[MAX_PKT];         /* array of bytes for easy population */
        long long whole;            /* easier copying of the object       */
    };
    char length;                    /* length of the transmitted object   */

};


/* Structure for tranmission of frames */
struct frame
{
    union
    {
        char bytes[MAX_FRAME];             /* series of bytes - MAX frame is */
        
        struct                             /* efficient enqueue and dequeue  */
        {
            long long low;                 /* Lower 8 bytes of the structure */
            long long high;                /* Higher 8 bytes of the structure*/
        };
    };
    int length;                            /* length of the frame */
};

/* List of frames to be send by the physical layer */
struct frame_queue
{
    struct frame queue[MAX_ELEMENTS];    /* frame queue */
    int head;                            /* Head of circular queue */
    int tail;                            /* Tail of circular queue */
    volatile int cnt;                    /* Number of rqs in queue */
};

/* List of frames to be send by the physical layer */
struct packet_queue
{
    struct transmit queue[MAX_ELEMENTS]; /* frame queue */
    int head;                            /* Head of circular queue */
    int tail;                            /* Tail of circular queue */
    volatile int cnt;                    /* Number of rqs in queue */
};


/* External variables*/
extern unsigned NR;
extern unsigned NS;
extern int counter;
extern struct current_frame_send;
extern struct packet temp_pkt;
extern struct frame_queue FQ;
extern int offset;


void reset_hall_queue(void);
void send_sw(unsigned char switch_num, enum Switch dir);
void send_md(unsigned char train_num, unsigned mag, enum Direction dir);
void hall_sensor_ack(unsigned char sensor_num);
void send_frame (struct frame *);
void DataLink (void);
void express_manager (void);
void virtual_train (void);
int special_sensor (int sensor);
void change_switch (int num, int state);
#endif /* TRAINSET_H_ */
