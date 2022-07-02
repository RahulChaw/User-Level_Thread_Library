/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int hello(void)
{
	printf("Hello world!\n");
	return 0;
}

int main(void)
{
	uthread_t tid;

	if(uthread_start(0) == 0)
	{
		printf("uthread_start(0): PASS\n");
	}
	tid = uthread_create(hello);
	if(tid == 2)
	{
		printf("uthread_create(hello): PASS\n");
	}
	if(uthread_join(tid, NULL) == 0)
	{
		printf("uthread_join(tid, NULL): PASS\n");
	}
	if(uthread_stop() == 0)
	{
		printf("uthread_stop(): PASS\n");
	}

	return 0;
}
