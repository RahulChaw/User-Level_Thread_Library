/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread2(void)
{
	uthread_create(thread3);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread1(void)
{
	uthread_create(thread2);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	return 0;
}

int thread6(void)
{
	int retval = 0;
	printf("thread%d\n", uthread_self());
	uthread_exit(retval);
	return 0;
}

int thread5(void)
{
	int retval;
	uthread_join(uthread_create(thread6), &retval);
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread4(void)
{
	int retval;
	uthread_join(uthread_create(thread5), &retval);
	printf("thread%d\n", uthread_self());
	return 0;
}

int main(void)
{
	//Yield tester
	uthread_start(0);
	uthread_join(uthread_create(thread1), NULL);
	uthread_stop();

	//join tester
	int retval;
	uthread_start(0);
	uthread_join(uthread_create(thread4), &retval);
	uthread_stop();

	return 0;
}
