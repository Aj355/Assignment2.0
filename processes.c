
/* Write your functions below and then use the array at the end of this file,
 *  processes, to register each process in the kernel system
 */

#include <stdio.h>
#include "system_requests.h"

/* Register your processes here please in the following way:
 * {process name, priority} where priority ranges from 1,
 * lowest, to 3, highest. Any other number will result in
 * registration failure */

struct process_reg processes[] =
{
 {proc1, 3}, {proc1, 2}, {proc1, 1}
};

void proc1 (void)
{
    pbind(pgetid());
    while(1)
    {
    pdisplay_str(1, pgetid(), "1");
    //sleep(10);
    pdisplay_str(1, pgetid(), "2");
    sleep(10);
    pdisplay_str(1, pgetid(), "3");
    //sleep(10);
    }
    //pbind(7);
    //pdisplay_str(1, 1, "I am process #");
    //x[0] = pgetid();
    //x[1] = '/0';
    //sleep(30);
   // while(1)
   // {
   //     pdisplay_str(50, 1, "1");
        //sleep(1);
   //     pdisplay_str(50, 1, "2");
        //sleep(1);
    ///}
    //pdisplay_str(1, 1, "Sleeping");
    //sleep(20);
    //pdisplay_str(1, 2, "Done Sleeping");
    //pdisplay_str(1, 3, "Sending");
    //psend(8, "Hello", 6);
    while (1);
}

void proc2 (void)
{


    //precv(&src_id, x, 10);
    //pdisplay_str(1, 4, "received");
    //pdisplay_str(10, 4, x);
    //pdisplay_str(20, 4, "from");
    //x[0] = src_id+'0';
    //x[1] = '\0';
    //pdisplay_str(30, 4, x);
    //while(1);
}

