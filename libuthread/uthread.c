#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

typedef enum {
	RUNNING,
	READY,
	BLOCKED,
	ZOMBIE
} STATE;

typedef enum {
	MAIN,
	REGULAR
} THREAD_LEVEL;

typedef struct uthread {
	uthread_t tid;
	uthread_ctx_t *context;
	void *stack;
	STATE state;
	THREAD_LEVEL level;
	struct uthread* child;
} uthread;

queue_t running_queue;
queue_t ready_queue;
queue_t zombie_queue;
queue_t blocked_queue;

uthread_t global_tid = 0;

typedef ucontext_t uthread_ctx_t;

uthread_ctx_t *old_ctx;

int uthread_start(int preempt)
{
	if (preempt == 0) {
		preempt_start();
	}

	struct uthread *main_thread = (struct uthread*)malloc(sizeof(struct uthread));
	uthread_ctx_t* uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	void* stack = uthread_ctx_alloc_stack();

	//Initializing main thread
	main_thread->tid = global_tid;
	main_thread->stack = stack;
	uthread_ctx_init(uctx, main_thread->stack, NULL);
	main_thread->context = uctx;
	main_thread->state = RUNNING;
	main_thread->level = MAIN;
	main_thread->child = NULL;

	ready_queue = queue_create();
	running_queue = queue_create();

	preempt_disable();
	queue_enqueue(running_queue, main_thread);
	preempt_enable();

	global_tid++;

	return 0;
}

int uthread_stop(void)
{
	//Checking edge conditions
	if(queue_length(ready_queue) == 0)
	{
		return -1;
	}
	else if(queue_length(blocked_queue) == 0)
	{
		return -1;
	}
	else if(queue_length(zombie_queue) == 0)
	{
		return -1;
	}

	//Destroying queues to stop multithreading
	queue_destroy(ready_queue);
	queue_destroy(blocked_queue);
	queue_destroy(zombie_queue);
	preempt_stop();

	return 0;
}

int uthread_create(uthread_func_t func)
{
	if (global_tid == 0){
		return -1;
	}

	preempt_disable();
	struct uthread *regular_thread = (struct uthread*)malloc(sizeof(struct uthread));
	uthread_ctx_t* uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	void* stack = uthread_ctx_alloc_stack();

	//Initializing new thread
	regular_thread->tid = global_tid;
	regular_thread->stack = stack;
	uthread_ctx_init(uctx, regular_thread->stack, func);
	regular_thread->context = uctx;
	regular_thread->state = READY;
	regular_thread->level = REGULAR;
	regular_thread->child = NULL;

	queue_enqueue(ready_queue, regular_thread);
	preempt_enable();

	global_tid++;

	return regular_thread->tid;
}

void uthread_yield(void)
{
	if(queue_length(ready_queue) != 0)
	{
		struct uthread *current_thread = (struct uthread*)malloc(sizeof(struct uthread));
		struct uthread *next_thread = (struct uthread*)malloc(sizeof(struct uthread));

		preempt_disable();
		queue_dequeue(running_queue, (void**)&current_thread);
		queue_dequeue(ready_queue, (void**)&next_thread);
		queue_enqueue(ready_queue, current_thread);
		queue_enqueue(running_queue, next_thread);
		preempt_enable();

		uthread_ctx_switch(current_thread->context, next_thread->context);
		next_thread->state = RUNNING;
		current_thread->state = READY;
	}
}

uthread_t uthread_self(void)
{
	struct uthread *current_thread;

	preempt_disable();
	queue_dequeue(running_queue, (void**)&current_thread);
	queue_enqueue(running_queue, current_thread);
	preempt_enable();

	uthread_t tid = ((struct uthread*)current_thread)->tid;

	return tid;
}

void uthread_exit(int retval)
{
	struct uthread *exit_thread = (struct uthread*)malloc(sizeof(struct uthread));
	struct uthread *next_thread = (struct uthread*)malloc(sizeof(struct uthread));

	preempt_disable();
	queue_dequeue(running_queue, (void**)&exit_thread);
	queue_dequeue(ready_queue, (void**)&next_thread);
	if(exit_thread->child != NULL)
	{
		uthread_join(exit_thread->child->tid, &retval);
	}
	while(exit_thread->stack != NULL)
	{
		uthread_ctx_destroy_stack(exit_thread->stack);
	}

	preempt_enable();
	uthread_ctx_switch(exit_thread->context, next_thread->context);
	preempt_disable();

	if(exit_thread->state != ZOMBIE)
	{
		exit_thread->state = ZOMBIE;
		queue_enqueue(zombie_queue, exit_thread);
	}
	else
	{
		queue_delete(zombie_queue, &exit_thread);
	}
	retval = 0;
	uthread_ctx_switch(exit_thread->context, next_thread->context);
	queue_enqueue(running_queue, next_thread);
	preempt_enable();

	next_thread->state = RUNNING;
}

int uthread_join(uthread_t tid, int *retval)
{
	//Checking for 0 condition
	if(tid == 0)
	{
		return -1;
	}

	queue_t zombie_copy = queue_create();

	struct uthread *wanted_thread = (struct uthread*)malloc(sizeof(struct uthread));
	struct uthread *current_thread = (struct uthread*)malloc(sizeof(struct uthread));
	struct uthread *current_thread_zom = (struct uthread*)malloc(sizeof(struct uthread));

	//Checking zombir queue for the thread we want to join
	preempt_disable();
	while(queue_dequeue(zombie_queue, (void**)&current_thread_zom))
	{
		if(current_thread_zom->tid == tid)
		{
			wanted_thread = current_thread_zom;
		}
		queue_enqueue(zombie_copy, current_thread_zom);
	}
	//Restoring the zombie queue as it was
	while(queue_dequeue(zombie_copy, (void**)&current_thread_zom))
	{
		queue_enqueue(zombie_queue, current_thread_zom);
	}
	queue_destroy(zombie_copy); //freeing memory
	*retval = queue_delete(zombie_queue, &wanted_thread);
	free(current_thread_zom);
	if(*retval == 0)
	{
		return 0;
	}

	//blocking current thread and running joining thread
	queue_dequeue(running_queue, (void**)&current_thread);
	queue_enqueue(blocked_queue, current_thread);
	queue_dequeue(ready_queue, (void**)&wanted_thread);
	queue_enqueue(running_queue, wanted_thread);
	uthread_ctx_switch(current_thread->context, wanted_thread->context);

	//Checking ready queue for the joining thread
	while(wanted_thread->tid != tid)
	{
		uthread_yield();
		queue_dequeue(running_queue, (void**)&wanted_thread);
		queue_enqueue(running_queue, wanted_thread);
	}
	current_thread->child = wanted_thread;
	uthread_exit(*retval);
	queue_dequeue(blocked_queue, (void**)&current_thread);
	queue_enqueue(ready_queue, current_thread);
	preempt_enable();
	
	//freeing memory
	free(current_thread);
	free(wanted_thread);
	if(*retval == 0)
	{
		return 0;
	}

	return -1; //returning -1 if no condition is satisfied
}
