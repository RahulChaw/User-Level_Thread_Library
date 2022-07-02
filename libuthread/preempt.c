#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> 

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 * 10,000 milliseconds
 */
#define HZ 100

struct sigaction sa; // instance of sigaction
struct itimerval itval; // instance of settimer: specify when a timer should expire

void alarm_hanler(int signal_num);


/* Signal handler aka Timer interrupt handler. Force currently running thread to yield */
void timer_handler(int signum)
{
	signum++;
	signum--;
	uthread_yield();
}

void preempt_start(void)
{
	/* TODO */

	/* Set up handler for timer */
	sa.sa_handler = timer_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGVTALRM);
	sa.sa_flags = 0;				
	sigaction(SIGVTALRM, &sa, NULL);

	/* Configure a timer */
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = HZ * HZ;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = HZ * HZ;

	if (setitimer(ITIMER_VIRTUAL, &itval, NULL))
	{
		perror("settimer");
	}
}

/* Restore the previous signal action, and restore the previous timer configuration. */
void preempt_stop(void)
{
	/* TODO */
	/* Restore the previous signal action */
	sa.sa_handler = timer_handler;
	sa.sa_flags = SA_RESETHAND;
	sigaction(SIGINT, &sa, NULL);

	/* Restore the previous timer configuration */
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_usec = HZ * HZ;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_usec = HZ * HZ;
	setitimer(ITIMER_VIRTUAL, &itval, NULL);
}

void preempt_enable(void)
{
	/* TODO */
	sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);
}

void preempt_disable(void)
{
	/* TODO */
	sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
}

/*
	//Other attempt for enabling and disabling
	sigset_t block_mask;
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGVTALRM);
	sa.sa_mask = block_mask;

	sigprocmask(SIG_BLOCK, &sa, NULL);
*/