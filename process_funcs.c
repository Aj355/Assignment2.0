
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
 {proc1, 3}, {proc2, 2}

};

void proc1 (void)
{
    while (1);
}

void proc2 (void)
{
    while(1);
}

