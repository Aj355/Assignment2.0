
/* Write your functions below and then use the array at the end of this file,
 *  processes, to register each process in the kernel system
 */

#include "system_requests.h"


/* Register your processes here please in the following way:
 * {process name, priority} where priority ranges from 1,
 * lowest, to 3, highest. Any other number will result in
 * registration failure */

struct process_reg processes[] =
{
 {proc1, 2},
 //{proc2, 3},
 //{proc3, 3},
 //{proc4, 3}
};



void proc1 (void)
{
    int i;
    i = get_time();
    while (1);
}

void proc2 (void)
{
    pbind(1);
    psend(2,"Hello", 6);
    while(1);
}

void proc3 (void)
{
    pdisplay_str(1,3,"I am process # 3");
    while (1);

}


void proc4 (void)
{
    int src_id,size;
     char x[10];
     char p[10] = "proc = x";
     char s[10] = "size = x";
     pbind(2);
     size = precv(&src_id,x,10);
     p[7] = src_id + '0';
     s[7] = size   + '0';
     pdisplay_str(1,1,p);
     pdisplay_str(1,2,s);
     pdisplay_str(1,3,x);
     while (1);

}
