/*
 * kernel_calls.h
 *
 *  Created on: Oct 24, 2017
 *      Author: AbdullahAlhadlaq
 */

#ifndef KERNEL_CALLS_H_
#define KERNEL_CALLS_H_



/* function prototypes  */
void init_kernel (void);
void SVCHandler(struct stack_frame *argptr);
void SVCall(void);
void PendSV (void);

#endif /* KERNEL_CALLS_H_ */
