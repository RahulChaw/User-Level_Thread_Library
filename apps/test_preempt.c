#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <private.h>

# define MAX 3030303
int child_thread(void)
{
    int count = 0;

	uthread_yield();
	printf("Long child thread%d\n", uthread_self());
    while(1)
    {
        if (count != MAX)
        {
            printf("Sorry, not at %d yet, count = %ld\n", MAX, count);
            count++;
        }
        else
        {
            printf("Congrats! You counted to %ld!\n", count);
            break;
        }
    }
	return 0;
}

int parent_thread(void)
{
	uthread_create(child_thread);
	uthread_yield();
	printf("Parent thread%d\n", uthread_self());
	uthread_yield();
	return 0;
}

int main(void)
{
    uthread_t tid;

	uthread_start(0);
    tid = uthread_create(parent_thread);
	uthread_join(tid, NULL);
	uthread_stop();

	return 0;
}
