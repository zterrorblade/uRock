/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * uRedistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * uRedistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * uRedistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This demo shows how to create a static thread and how to terminate it
 * after some period of time.
 * A GTimer is used to termiate the thread.
 *
 * Note: The GOS API does not provide any function to terminate or kill a thread
 *       as not every operating system allows it to terminate a thread out
 *       from another one. Therefore, we use the parameters to implement a
 *       "fall over the edge" algorithm.
 */

#include "gfx.h"

GTimer				gt;
gfxThreadHandle		thd;

/*
 * Thread function
 * Prints a message
 */
threadreturn_t Thread_function(void* param)
{	
	/* Cast the paramter into a bool pointer so we can use it */
	bool_t* doExit = (bool_t*)param;

	/* Execute this until we shall be terminated */
	while (*doExit == FALSE) {
		printf("Message from Thread\r\n");
		gfxSleepMilliseconds(500);
	}

	/* Don't return anything */
	return (threadreturn_t)0;
}

/*
 * Timer callback function
 * Will be called when timer expires/fires
 */
void timerCallback(void* param)
{
	/* Cast the paramter into a bool pointer so we can use it */
	bool_t* threadExit = (bool_t*)param;
	
	/* Ask the Thread to fall over the end */
	printf("Closing thread!\r\n");
	*threadExit = TRUE;
}

/*
 * The main function
 */
int main(void)
{
	bool_t exitThread = FALSE;

	gfxInit();

	/* Initialize the timer */
	gtimerInit(&gt);

	/* Create a static thread from the default heap with normal priority.
	 * We pass a the parameter to the thread which tells the thread whether to return or not
	 */
	thd = gfxThreadCreate(NULL, 128, NORMAL_PRIORITY, Thread_function, (void*)&exitThread);

	/* Start the timer. The callback function will be called once after 2000ms
	 * We will pass the thread handle as a parameter so the timer can ask the thread to termite
	 */
	gtimerStart(&gt, timerCallback, (void*)&exitThread, FALSE, 2000);

	while(TRUE) {
		gfxSleepMilliseconds(500);
	}   
}
