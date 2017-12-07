
/* Write your functions below and then use the array at the end of this file,
 *  processes, to register each process in the kernel system
 */

#include <stdio.h>
#include "processes.h"
#include "trainsetprotocol.h"
/* Register your processes here please in the following way:
 * {process name, priority} where priority ranges from 1,
 * lowest, to 3, highest. Any other number will result in
 * registration failure */



void test (void)
{
    pbind(6);
    send_md(1,6,CW);

    while(1);
}
void proc1 (void)
{
    pbind(pgetid());
    while(1)
    {
    pdisplay_str(1, pgetid(), "1");
    sleep(10);
    pdisplay_str(1, pgetid(), "2");
    sleep(10);
    pdisplay_str(1, pgetid(), "3");
    sleep(10);
    }
}

void proc2 (void)
{

}

struct process_reg processes[] =
{
 {DataLink,3},{train_manager,3}
 //{proc1, 3}, {proc1, 3}, {proc1, 3}
};
