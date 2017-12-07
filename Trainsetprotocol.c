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
/*10*/{{2, STR, CCW}, {2, STR, CCW}, {3, STR, CW}, {3, STR, CW},
          /*5*/           /*6*/         /*7*/         /*8*/
      {3, STR, CW}, {3, STR, CW}, {3, STR, CW}, {3, STR, CW},
          /*9*/          /*10*/         /*11*/        /*12*/
      {3, STR, CW}, {0, UNCH, AT_DST}, {0, UNCH, CCW}, {2, STR, CCW},
          /*13*/         /*14*/         /*15*/         /*16*/
      {2, STR, CCW}, {2, STR, CCW}, {2, STR, CCW}, {2, STR, CCW},
          /*17*/         /*18*/         /*19*/         /*20*/
      {2, STR, CCW}, {3, STR, CW}, {3, STR, CW}, {3, STR, CW},
          /*21*/         /*22*/         /*23*/         /*24*/
      {2, STR, CCW}, {2, STR, CCW}, {2, DIV, CCW}, {2, DIV, CCW}},

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



char map [MAP_HEIGHT][MAP_WIDTH]=
{{"                  --20-------------------------19--                 "},
 {"                /                                   \\              "},
 {"    ----14------------13------12-------11------10--------09-----   "},
 {"  /                               /                              \\ "},
 {" /               --24-------23---/                                \\"},
 {"15                                                                08"},
 {"|                                                                  |"},
 {"16                                                                07"},
 {" \\                                  /--21--------22--             /"},
 {"  \\                                /                             / "},
 {"    ----01-----------02------03-------04------05---------06-----   "},
 {"                \\                                   /              "},
 {"                  --17-------------------------18--                 "}
 };

struct location sensors[HALL_SEN_NUM] =
{{"01", 9, 10}, {"02", 22, 10}, {"03", 30, 10}, {"04", 39, 10}, {"05", 47, 10},
 {"06", 58, 10}, {"07", 67, 7}, {"08", 67, 5}, {"09", 58, 2}, {"10", 48, 2},
 {"11", 40, 2}, {"12", 31, 2}, {"13", 23, 2}, {"14", 9, 2}, {"15", 1, 5},
 {"16", 1, 7}, {"17", 21, 12}, {"18", 48, 12}, {"19", 48, 0}, {"20", 21, 0},
 {"21", 40, 8}, {"22", 50, 8}, {"23", 29, 4}, {"24", 20, 4}
};

struct location switches[SWITCH_NUM] =
{{"/-", 16, 2}, {"/-", 36, 2}, {"\\-", 54, 2},
 {"/-", 54, 10}, {"/-", 35, 10}, {"\\-", 16, 10}
};

char train_symbols [TRAIN_NUM][MAX_NAME_SZ] = {"EE", "LL"};

int special_sensors[SPEC_SENSOR_NUM] = {1, 3, 6, 9, 11, 14};
int h;
struct packet temp_pkt;
enum Switch switch_state[SWITCH_NUM];
struct train trains[TRAIN_NUM];
unsigned nr;
unsigned ns;


/*******************************************************************************
 * Purpose:
 *             This function process an input character and stores it in
 *             a buffer. it is used by the virtual train process.
 * Arguments:
 *             in_buff:     input buffer to store incoming character
 *             in_char:     the incoming character
 *             buff_count:  index to the buffer
 * Return :
 *             TRUE         if a command is entered
 *             FALSE        if a command is still not entered
 *******************************************************************************/
int process_char (char *in_buff, int *buff_count, int in_char)
{
    int cmd = FALSE;
    // process the input character
    switch (in_char)
    {
    case ESC:
        // ignore this character
        break;

    case DEL:
        // if the buffer is not empty
        if ((*buff_count))
        {
            // echo back the character
            pdisplay_char((*buff_count), 20, in_char);

            // remove one character from buffer
            (*buff_count)--;
        }
        break;

    case CARRIAGE_RTN:
        // then a command is done
        // null terminate the buffer
        in_buff[(*buff_count)] = NUL;
        (*buff_count)=0;

        //clear the command from the command line
        pdisplay_str(1, 20, "                ");

        // put the cursor at the start
        pdisplay_char(1, 20, in_char);

        cmd = TRUE;
        break;

    default:
        // if the buffer is not full
        if ((*buff_count) < (MAX_INPUT-1))
        {
            //insert it in the buffer
            in_buff[(*buff_count)] = in_char;
            (*buff_count)++;
            // echo back the character
            pdisplay_char((*buff_count), 20, in_char);
        }
        break;
    }

    return cmd;
}

/*******************************************************************************
 * Purpose:
 *             This process takens input from the UART0 and processes
 *             commands from the user. The commands can control the
 *             train set or they can by commands to execute virtual
 *             operations such as simulating that a hall sensor is
 *             triggered
 * Arguments:
 *             NONE
 * Return :
 *             NONE
 *******************************************************************************/
void virtual_train (void)
{
    int src_id;      // used to receive messages
    char in_char;    // input character from UART
    struct transmit msg; // to send messages
    char sensor;


    char in_buff[MAX_INPUT];    // contain the input commands
    int buff_count=0;           // index to buffer

    pbind(VERTUAL_TRN);

    pdisplay_str(1, 19, "Command line input:\n\r");
    pdisplay_str(1, 22, "Command line output:");
    while (1)
    {
        // receive a message from UART
        precv(&src_id, &in_char, sizeof(char));

        // process the character
        if(process_char(in_buff, &buff_count, in_char))
        {
            // if it is a hall sensor trigger command
            if (in_buff[0] == 'h' && isdigit(in_buff[1]))
            {
                sensor = (in_buff[1]-'0');
                if (isdigit(in_buff[2]))
                    sensor = 10*sensor + in_buff[2]-'0';
                if (sensor <= 24)
                {
                    pdisplay_str(1, 23, "                         ");
                    pdisplay_str(1, 23, "Hall Sensor ");
                    pdisplay_char(13, 23, (sensor/10)+'0');
                    pdisplay_char(14, 23, (sensor%10)+'0');
                    pdisplay_str(16, 23, "triggered");
                    msg.xmit[0] = HALL_CMD;
                    msg.xmit[1] = sensor;
                    psend(6, &msg, sizeof(char));
                }
            }
            // if it is a destination command
            if (in_buff[0] == "d" && isdigit(in_buff[1]))
            {
                sensor = (in_buff[1]-'0');
                if (isdigit(in_buff[2]))
                    sensor = 10*sensor + in_buff[2]-'0';
                if (sensor <= 24)
                {
                    pdisplay_str(1, 23, "                         ");
                    pdisplay_str(1, 23, "Destination ");
                    pdisplay_char(13, 23, (sensor/10)+'0');
                    pdisplay_char(14, 23, (sensor%10)+'0');
                    pdisplay_str(16, 23, "is chosen");
                    msg.xmit[0] = DEST_CMD;
                    msg.xmit[1] = sensor;
                    psend(6, &sensor, sizeof(char));
                }
            }
        }


    }
}

/* this function updates the position of a train on the map */
void update_trn_pos (int train, int sensor, int *x_pos, int *y_pos)
{
    pdisplay_str(*x_pos, *y_pos+MAP_POS, sensors[trains[EXPRESS].head-1].name);
    *x_pos = sensors[sensor-1].x_pos;
    *y_pos = sensors[sensor-1].y_pos;
    pdisplay_str(*x_pos, *y_pos+MAP_POS, train_symbols[train]);
}

/* this function gets information from the routing table */
void get_action (int train, int dest, int *dir, int *sw_num, int *sw_state)
{
    *dir = routing_tbl[trains[train].head-1][dest-1].dir;
    *sw_num = routing_tbl[trains[train].head-1][dest-1].sw_num;
    *sw_state = routing_tbl[trains[train].head-1][dest-1].sw_state;
}

/* this function changes the state of switch and updates map */
void change_switch (int num, int state)
{
    if (state == UNCH)
        return;

    send_sw(num, state);
    send_sw(num, state);
    pdisplay_char(switches[num-1].x_pos, switches[num-1].y_pos+MAP_POS, switches[num-1].name[state]);
}


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
    int dir, sw_num, sw_state;  // to hold info from routing table
    int dest;                   // destination location
    char sensor;                // triggered hall sensor
    int src_id;                 // for receiving messages
    struct transmit msg;        // to receive message
    int x_pos[TRAIN_NUM];       // x position of train on map
    int y_pos[TRAIN_NUM];       // y position of train on map
    int i;

    /* bind to a mailbox */
    pbind(APP);

    /* reset all hall sensors */
    reset_hall_queue();

    /* print the map on the screen */
    for (i=0; i<MAP_HEIGHT; i++)
    {
        pdisplay_str(FIRST_LINE, MAP_POS+i, map[i]);
    }

    /* initialize head position, speed, and destination */
    trains[EXPRESS].head = INIT_POS;
    trains[EXPRESS].speed = INIT_SPEED;
    dest = INIT_DEST;

    /* put the initial place of the train on the map */
    update_trn_pos(EXPRESS, trains[EXPRESS].head, &x_pos[EXPRESS], &y_pos[EXPRESS]);

    /* get direction and switch info from routing table */
    get_action (EXPRESS, dest, &dir, &sw_num, &sw_state);

    /* update the current direction of the train */
    trains[EXPRESS].dir = dir;

    /* send direction and switch commands */
    send_md(EXPRESS, trains[EXPRESS].speed, trains[EXPRESS].dir);
    change_switch(sw_num, sw_state);
    
    /* start infinite loop */
    while (1)
    {
        /* get a message from the mailbox and send ack*/
        precv(&src_id, &msg, sizeof(struct transmit));

        /* if the received message is from the virtual machine */
        if (src_id == VERTUAL_TRN && msg.xmit[0] == DEST_CMD)
        {
            /* get direction and switch info from routing table */
            get_action (EXPRESS, dest, &dir, &sw_num, &sw_state);

            /* update the current direction of the train */
            trains[EXPRESS].dir = dir;

            /* send direction and switch commands */
            send_md(EXPRESS, trains[EXPRESS].speed, trains[EXPRESS].dir);
            change_switch(sw_num, sw_state);

            continue;
        }

        /* if it is from the train set */
        sensor = msg.xmit[1];
        hall_sensor_ack(sensor);

        // if the received hall sensor is the same as the head pos
        if (sensor == trains[EXPRESS].head || sensor == trains[EXPRESS].prev_h)
            trains[EXPRESS].tail = sensor;
        // if it is a new sensor
        else
        {
            update_trn_pos(EXPRESS, sensor, &x_pos[EXPRESS], &y_pos[EXPRESS]);

            // update the head position
            trains[EXPRESS].prev_h = trains[EXPRESS].head;
            trains[EXPRESS].head = sensor;

            // get next direction and switch from routing table
            get_action (EXPRESS, dest, &dir, &sw_num, &sw_state);

            // if the train is at the destination
            if (dir == AT_DST)
            {
                // stop the train
                // reverse the direction of the train
                send_md(EXPRESS, REVERSE_SPD, !trains[EXPRESS].dir);
                // wait for some time in order to force the train to stop fast
                sleep(REVERSE_TIME);
                // send a message to the train to stop
                send_md(EXPRESS, 0, 0);
                //update train direction
                trains[EXPRESS].dir = dir;
            }
            // else if the new direction = new direction
            else if (dir == trains[EXPRESS].dir)
            {
                change_switch(sw_num, sw_state);
            }
            // else if new direction does not equal old direction
            // and it is not one of the special sensors
            else if (!special_sensor(sensor))
            {
                // send switch and direction command
                change_switch(sw_num, sw_state);
                send_md(EXPRESS, trains[EXPRESS].speed, dir);

                //update the train position
                update_trn_pos(EXPRESS, sensor, &x_pos[EXPRESS], &y_pos[EXPRESS]);

                // the head and tail have the same position b/c train is reversing
                trains[EXPRESS].tail = trains[EXPRESS].head;
                trains[EXPRESS].prev_h = 0;

                //update the direction of the train
                trains[EXPRESS].dir = dir;
            }
        }
    }
}

int special_sensor (int sensor)
{
    int i;

    for (i=0; i<SPEC_SENSOR_NUM; i++)
    {
        if (sensor == special_sensors[i])
            return TRUE;
    }
    
    return FALSE;
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
    struct transmit msg;
    struct mag_dir md;
    md.direction = dir;
    md.ignored   = 0;
    md.magnitude = mag;
    msg.xmit[CODE] = CHNG_SPDR_MSG;
    msg.xmit[ARG1] = train_num+1;
    msg.xmit[ARG2] = md.mag_dir;
    msg.length = 3;
    psend(DLL,&msg,sizeof(struct transmit));
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
    struct transmit msg;
    if (dir == UNCH)
        return;

    if (switch_num == 3 && dir == STR)
        return;

    msg.xmit[CODE] = CHNG_SWTC_MSG;
    msg.xmit[ARG1] = switch_num;
    msg.xmit[ARG2] = (unsigned char) dir;
    msg.length = 3;

    psend(DLL,&msg,sizeof(struct transmit));
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
    struct transmit msg;
    msg.xmit[CODE] = HALL_REST_MSG;
    msg.length = 1;
    psend(DLL,&msg,sizeof(struct transmit));
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
    struct transmit msg;
    msg.xmit[CODE] = HALL_REST_ACK;
    msg.xmit[ARG1] = sensor_num;
    msg.length = 2;
    psend(DLL,&msg,sizeof(struct transmit));
}


void xmit_packet (char *msg, struct control ctr, unsigned char len)
{
    struct transmit packet;
    int i=0;

    if (len > PKT_BYTE)
        return;
    packet.whole = 0;
    packet.xmit[i++] = ctr.cntrl;
    if (len != 0)
    {
        packet.xmit[i++] = len;
        for ( i = 0 ; i < len ; i++)
            packet.xmit[i + 2] = msg[i];
        i +=2;
    }
    packet.length = i;

    pkcall(PHYSICAL,&packet);
}

void xmit_window(void)
{

}

struct transmit window;
/* -------------------------------------------------------------------------- *
 * Purpose:       Insert an entry into the UART queue
 *
 * Arguments:
 *                req:  uart entry to be enqueued
 * Returns:
 *                TRUE  if enqueuing is successful
 *                FALSE if enqueuing is not successful
 * -------------------------------------------------------------------------- */
void DataLink(void)
{
    int source_id;
    struct transmit data;
    struct control ctr;
    int timeout = 0;
    pbind(DLL);
    while (1)
    {
        precv(&source_id,&data,sizeof(struct transmit));

        if (source_id == UART)
        {
            ctr.cntrl = data.xmit[CTRL];
            switch (ctr.type)
            {
            case DATA:
                if (ctr.nr <= ns && ctr.ns == nr)
                {
                    nr = (nr + 1) % 8;
                    if (data.xmit[2] == 0xa0)
                    {
                    psend(6, data.xmit+PKT_MSG, sizeof(struct transmit));
                    }
                    ctr.nr = nr;
                    ctr.ns = 0;
                    ctr.type = ACK;
                    xmit_packet(NULL,ctr,0);

                    if (dequeue_packet(&data))
                    {
                        ctr.nr = nr;
                        ctr.ns = ns;
                        ns = (ns + 1) % 8;
                        ctr.type = DATA;
                        xmit_packet(data.xmit,ctr,data.length);
                    }
                    else
                        window.whole = 0;

                }
                else
                {
                    ctr.nr = nr;
                    ctr.ns = 0;
                    ctr.type = NACK;
                    xmit_packet(NULL,ctr,0);
                }
                break;

            case ACK:
                if (ctr.nr <= ns)
                {
                    timeout = 0;
                    if (dequeue_packet(&data))
                    {
                        ctr.nr = nr;
                        ctr.ns = ns;
                        ns = (ns + 1) % 8;
                        ctr.type = DATA;
                        xmit_packet(data.xmit,ctr,data.length);
                    }
                    else
                        window.whole = 0;
                }
                else
                {
                    ctr.nr = nr;
                    ctr.ns = 0;
                    ctr.type = NACK;
                    xmit_packet(NULL,ctr,0);
                }
                break;

            case NACK:
                ctr.nr = nr;
                ctr.ns = (ns -1) % 8;
                ctr.type = DATA;
                xmit_packet(window.xmit,ctr,window.length);
                break;
            default:
                break;
            }
        }
        else if (source_id == APP)
        {
            ctr.nr = nr;
            ctr.ns = ns;
            ctr.type = DATA;
            if (!window.whole)
            {
                window.whole = data.whole;
                ns = (ns + 1) % 8;
                xmit_packet(data.xmit,ctr,data.length);
            }
            else
                enqueue_packet(&data);
        }
        else if (source_id == TIME_SERVER && timeout)
        {
            //xmit_window();
        }
    }
}
