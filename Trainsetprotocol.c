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
#include "Trainsetprotocol.h"
#include "processes.h"
#include "Pcommands.h"

struct action routing_tbl[HALL_SEN_NUM][HALL_SEN_NUM] =
{
              /*1*/            /*2*/         /*3*/         /*4*/
 /*1*/{{0, UNCH, AT_DST}, {6, STR, CCW}, {6, STR, CCW}, {6, STR, CCW},
              /*5*/           /*6*/         /*7*/         /*8*/
       {6, STR, CCW}, {6, STR, CCW}, {6, STR, CCW}, {6, STR, CCW},
             /*9*/          /*10*/         /*11*/        /*12*/
       {6, STR, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
            /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {6, DIV, CCW}, {6, DIV, CCW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {6, STR, CCW}, {6, STR, CCW}, {6, STR, CCW}, {6, STR, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*2*/{{6, STR, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {6, STR, CW}, {6, STR, CW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {6, STR, CW}, {6, STR, CW}, {6, STR, CW}, {6, STR, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {6, STR, CW}, {6, STR, CW}, {0, UNCH, CCW}, {6, STR, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*3*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST}, {5, STR, CCW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {5, STR, CCW}, {5, STR, CCW}, {5, STR, CCW}, {5, STR, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {5, STR, CCW}, {5, STR, CCW}, {5, STR, CCW}, {0, UNCH, CW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {0, UNCH, CW}, {5, STR, CCW}, {5, STR, CCW}, {0, UNCH, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {5, DIV, CCW}, {5, DIV, CCW}, {5, STR, CCW}, {5, STR, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*4*/{{5, STR, CW}, {5, STR, CW}, {5, STR, CW}, {0, UNCH, AT_DST},
          /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {5, STR, CW}, {5, STR, CW}, {5, STR, CW}, {5, STR, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {5, STR, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {5, STR, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {5, STR, CW}, {5, STR, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*5*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, AT_DST}, {4, STR, CCW}, {4, STR, CCW}, {4, STR, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {4, STR, CCW}, {4, STR, CCW}, {4, STR, CCW}, {4, STR, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {4, STR, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {4, STR, CCW}, {4, STR, CCW}, {4, STR, CCW}, {0, UNCH, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {0, UNCH, CW}, {0, UNCH, CW}, {4, STR, CCW}, {4, STR, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*6*/{{4, STR, CW}, {4, STR, CW}, {4, STR, CW}, {4, STR, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {4, STR, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {4, STR, CW}, {4, STR, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {4, DIV, CW}, {4, DIV, CW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {4, STR, CW}, {4, STR, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*7*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*8*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*9*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {0, UNCH, AT_DST}, {3, STR, CCW}, {3, STR, CCW}, {3, STR, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {3, STR, CCW}, {3, STR, CCW}, {3, STR, CCW}, {3, STR, CCW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {0, UNCH, CW}, {0, UNCH, CW}, {3, DIV, CCW}, {3, DIV, CCW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {3, STR, CCW}, {3, STR, CCW}, {3, STR, CCW}, {3, STR, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*10*/{{0, UNCH, CCW}, {0, UNCH, CCW}, {3, STR, CW}, {3, STR, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {3, STR, CW}, {3, STR, CW}, {3, STR, CW}, {3, STR, CW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {3, STR, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {0, UNCH, CCW}, {3, STR, CW}, {3, STR, CW}, {3, STR, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

          /*1*/            /*2*/         /*3*/         /*4*/
/*11*/{{2, STR, CCW}, {2, STR, CCW}, {2, STR, CCW}, {0, UNCH, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
          /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST}, {2, STR, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
       {2, STR, CCW}, {2, STR, CCW}, {2, STR, CCW}, {2, STR, CCW},
          /*17*/         /*18*/         /*19*/         /*20*/
       {2, STR, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {2, STR, CCW},
          /*21*/         /*22*/         /*23*/         /*24*/
       {2, STR, CCW}, {2, STR, CCW}, {2, DIV, CCW}, {2, DIV, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*12*/{{0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {2, STR, CW}, {2, STR, CW}, {2, STR, CW}, {2, STR, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {2, STR, CW}, {2, STR, CW}, {2, STR, CW}, {0, UNCH, AT_DST},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CCW}, {2, STR, CW}, {2, STR, CW}, {0, UNCH, CCW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {2, STR, CW}, {2, STR, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*13*/{{1, STR, CCW}, {1, STR, CCW}, {1, STR, CCW}, {1, STR, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {1, STR, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, AT_DST}, {1, STR, CCW}, {1, STR, CCW}, {1, STR, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {1, STR, CCW}, {0, UNCH, CW}, {1, STR, CCW}, {1, STR, CCW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {1, STR, CCW}, {1, STR, CCW}, {0, UNCH, CW}, {0, UNCH, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*14*/{{0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {1, STR, CW}, {1, STR, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {1, STR, CW}, {1, STR, CW}, {1, STR, CW}, {1, STR, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {1, STR, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {1, STR, CW}, {1, STR, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {1, STR, CW}, {1, STR, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*15*/{{0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW}, {0, UNCH, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*16*/{{0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CW}, {0, UNCH, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*17*/{{6, DIV, CW}, {6, DIV, CW}, {6, DIV, CW}, {6, DIV, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {6, DIV, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CCW}, {6, DIV, CW}, {6, DIV, CW}, {6, DIV, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {6, DIV, CW}, {6, DIV, CW}, {6, DIV, CW}, {6, DIV, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, AT_DST}, {0, UNCH, CCW}, {0, UNCH, CCW}, {6, DIV, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {6, DIV, CW}, {6, DIV, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*18*/{{0, UNCH, CW}, {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {4, DIV, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CW}, {0, UNCH, AT_DST}, {4, DIV, CCW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW}, {4, DIV, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*19*/{{0, UNCH, CCW}, {3, DIV, CW}, {3, DIV, CW}, {3, DIV, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {3, DIV, CW}, {3, DIV, CW}, {3, DIV, CW}, {3, DIV, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {3, DIV, CW}, {3, DIV, CW}, {3, DIV, CW}, {3, DIV, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {3, DIV, CW}, {0, UNCH, CCW}, {0, UNCH, CCW}, {0, UNCH, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CCW}, {3, DIV, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CCW}, {0, UNCH, CCW}, {3, DIV, CW}, {3, DIV, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*20*/{{1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {1, DIV, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {1, DIV, CCW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST},
           /*21*/         /*22*/         /*23*/         /*24*/
       {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW}, {1, DIV, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*21*/{{5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW}, {5, DIV, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, AT_DST}, {0, UNCH, CCW}, {5, DIV, CW}, {5, DIV, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*22*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CW}, {0, UNCH, AT_DST}, {0, UNCH, CW}, {0, UNCH, CW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*23*/{{2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW}, {2, DIV, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {2, DIV, CW}, {2, DIV, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}},

           /*1*/            /*2*/         /*3*/         /*4*/
/*24*/{{0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*5*/           /*6*/         /*7*/         /*8*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*9*/          /*10*/         /*11*/        /*12*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*13*/         /*14*/         /*15*/         /*16*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*17*/         /*18*/         /*19*/         /*20*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW},
           /*21*/         /*22*/         /*23*/         /*24*/
       {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, CW}, {0, UNCH, AT_DST}}
};



struct packet temp_pkt;
enum Switch switch_state[SWITCH_NUM];
struct train trains[TRAIN_NUM];
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
void express_manager(void)
{
    pbind(6);
    /* Send message to DLL with all switches straight   */
    send_sw(ALL, STR);
    
    /* Send message to DLL with hall sensor queue reset */
    reset_hall_queue();

    /* Ask display to enter train sections and speed */
    //pdisplay_str(1,5,"Enter starting sections and speeds:\n");
    /* RECV message from display */
    
    /* Load trains locations and speeds */

    trains[EXPRESS].head = 7;
    trains[EXPRESS].tail = 8;
    trains[EXPRESS].speed = 4;
    trains[EXPRESS].dir = CW;
    send_md(EXPRESS, trains[EXPRESS].speed, trains[EXPRESS].dir);

     
    /* Get next direction */
     
    /* Get next switch state */
     
    /* WHILE destination not reached */
    while(1);
    /* IF switch state needs to change */
     
    /* SEND change SWITCH message to DLL */
    
    /* SEND next direction and speed to specified train */
    
    /* wait to receive message back from DLL with new train location */
    
    /* Get next direction */
    
    /* Get next switch state */
    
    /* Send stop train message to DLL */
    
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
void encapsulate(struct packet packet)
{
    struct frame  temp_frm;
    temp_frm.start_xmit = STX;
    temp_frm.pkt.msg.code = packet.msg.code;
    temp_frm.pkt.msg.arg1 = packet.msg.arg1;
    temp_frm.pkt.msg.arg2 = packet.msg.arg2;
    temp_frm.pkt.len = packet.len;
    temp_frm.pkt.ctr.cntrl= packet.ctr.cntrl;
    temp_frm.Chksum  = packet.ctr.cntrl;
    temp_frm.Chksum += packet.len;
    temp_frm.Chksum += packet.msg.code;
    temp_frm.Chksum += packet.msg.arg1;
    temp_frm.Chksum += packet.msg.arg2;
    temp_frm.Chksum = ~temp_frm.Chksum;
    temp_frm.end_xmit = ETX;
    send_frame(temp_frm);
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
    msg.arg1 = train_num+1;
    msg.arg2 = md.mag_dir;
    psend(5,&msg,sizeof(struct message));
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
    psend(5,&msg,sizeof(struct message));
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
    psend(5,&msg.message,sizeof(struct message));
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
    psend(5,&msg,sizeof(struct message));
}

struct packet window;
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
    int source_id;
    unsigned long long data =0;
    struct message t;
    struct packet packet;
    packet.pkt = 0;
    pbind(5);
    while (1)
    {
        precv(&source_id,&data,sizeof(long int));
        if (source_id == UART)
        {
            packet.pkt = data;
            if (packet.ctr.ns == nr)
            {
                nr = (nr + 1) % 8;
                switch (packet.ctr.type)
                {
                    case DATA:
                        // note: you only want to send hall trigger msg
                        psend(6, &packet.msg, packet.len);
                        packet.msg.code = 0;
                        packet.msg.arg1 = 0;
                        packet.msg.arg2 = 0;
                        packet.ctr.nr = nr;
                        packet.ctr.ns = 0;
                        packet.ctr.type = ACK;
                        packet.len = 0;
                        encapsulate(packet);
                        break;
                    case ACK:
                        if (packet.ctr.nr == ns)
                        {
                            window.pkt=0;
                        }
                        else
                        {
                            encapsulate(window);
                        }
                        break;
                    case NACK:
                        break;
                    default:
                        break;
                }
            }
        }
        else if (source_id == 6)
        {
            t.message = data;
            packet.msg.code = t.code;
            packet.msg.arg1 = t.arg1;
            packet.msg.arg2 = t.arg2;
            packet.ctr.nr = nr;
            packet.ctr.ns = ns;
            ns = (ns + 1) % 8;
            packet.ctr.type = DATA;
            packet.len = 3;
            window.pkt = packet.pkt;
            encapsulate(packet);
        }
    }
}


void send_frame (struct frame temp)
{
    if ( UART1_state == BUSY )
        enqueue_frame(&temp);
        /* IF UART is idle - not transmitting any characters */
    else
    {
        UART1_state = BUSY;          // Signal UART is busy
        send.frame = temp.frame;
        UART1_DR_R = temp.frames[counter++];  // Load character into data reg.
    }
}
