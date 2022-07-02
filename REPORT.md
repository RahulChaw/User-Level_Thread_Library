# ECS 150 User-level Thread Library

## Summary 

This program is to implement a user-level thread library for Linux. Our library 
allows independent threads to run concurrently. The application creates the 
threads and scheduals them to execute in a repetative turn around system. The 
threads can join each other by using a synchronized mechanism that we installed 
to allow multithreading to occur. A preemptive management is provided to keep 
the schduler orderly and functioning.

## Implementation

The implementation of this project is divided into three sections of 
implementations:

1. Constructing a generic queue API that is compatible with any types
2. Implementing a thread management system to create multithreading
3. Managing potential uncooperative threads that keeps the processing resources 
for themselves

### Queue API

To implement a queue, first we must think what is the best structure to support 
what we want this queue to do. We want this queue to be able to store data is a 
FIFO system. We must also consider the unpredictable size of the queue and 
being able to remove any data from any position on the queue. With all the 
factors, we decided that a linked list structure would be the best foundationin 
implementing a queue.

Our object queue will be structured as a linked list that is made up of node 
objects, including a node head and tail. Each node stores a type void item to 
keep our queue generic. 

Funtions `queue_create()` allocates memory using `malloc()`. 

Function `queue_destroy()` deallocates the queue's memory by calling `free()` 
only when the queue is empty.

Funtion `queue_enqueue()` will allocate memory to initalize a `newNode` and 
stores the passing `void *data` argument as its data. If nodes already exists 
in the queue, the queue's current tail's `prev` pointer is assigned to the 
pointer variable `newNode` and the `newNode` is now the queue's new tail. 
Otherwise, the `newNode` is assigned as both head and tail of the queue.

Function `queue_dequeue()` will only pop off the head node of the queue. To do 
this, we must store the pointer of the current head node to a pointer varible 
`headNode`. In the scenario of there being a single node in the queue, we can 
call `free()` on `headNode` and reassign the queue's head and tail as NULL. 
However, if the queue contains multiple nodes, we are popping off the current 
head node after reassigning the head's `next` pointer as the new head node. 

Function `queue_delete()` takes the passed in argument `void *data` and checks 
all edge conditions of which node in the queue will store the matching data 
pointer. It first checks if the queue or data is empty. If the queue has a 
length of 1, it compares the head node's pointer item to the pointer data. A 
match will `free()` the pointer node entirely and a failed match will return a 
-1. In the case of the queue being filled with multiple nodes, there are three 
sub-conditions to take. First, if the data pointer is to be found in the head 
node, the head node will be `free()` and the head's `next` will be the new head 
node. Second, if the data pointer is to be found in the tail node, the tail 
node will be `free()` and the tail's `prev` will be the new tail node. Lastly, 
if the data pointer is to be found in anywhere in the middle of the linked 
list, we will then iterate through the middle, not including the head and tail 
node, looking for a node with the matching data pointer. Once found, we will 
reassign the `next` and `prev` of the nodes around our found node. Then we can 
safely `free()` the found node.

Our function `queue_iterate()` works by using `next` pointer in each node to go 
through the queue and used the function calling using function pointer to call 
the function on each item of the queue with the provided argument. We check for 
the edge condition if the queue does not exist and return -1. 

Function `queue_length()` will simply return the value of the object queue's 
`count` feature if the queue is not empty.

### Uthread API

The Uthread API is a thread management that utilizes the constructed queue and 
makes multithreading an interface possible. It is a scheduling concepts that 
categorizes a pool of threads into four process states: ready, running, 
blocked, and zombie. In the early executiong of scheduling, only two queues are 
created and categorized as ready and running. This is because when we start 
with our pool of threads, only the first processed thread is stored into the 
`running queue` since only one thread can have the CPU. The remaining threads 
are stored into the `ready queue`, waiting for their turn with the CPU. One by 
one, a thread from the ready queue is elected to run in the `running queue` 
until they are done. When a thread is done in the `running queue`, it is then 
examined to see if its data was collected or not. If scucessfully collected, 
the thread is then destroyed from the whole multithreading process entirely. 
However, the thread is transitioned to a zombie state if the thread's data was 
never collected at the end of its termination. To switch to a zombie state, we 
must enqueue the thread to the `zombie queue` as it waits. If we encounter a 
thread that needs to wait to collect from a currently active thread, it must 
yield and be enqueued to the `block queue`.

Function `uthread_stop()` checks if all the queues (ready, zombie, blocked) are 
empty and destroy them if they are and return 0. It also checks for the edge 
conditions i.e., if any of the queues are not empty and return -1. 

Function `uthread_yield()` starts by dequeuing the `running queue` and `ready 
queue` (i.e., get the running thread and the ready thread on top of the queue) 
and context switch them and change their states before pushing them on their 
respective queues (i.e., the thread which was on the top of ready queue is 
pushed on the running queue and the running thread at the end of ready queue.)

Function `uthread_exit()` dequeues the current running thread from running 
library. First we check if the child has returned for this thread, if not then 
we join the child thread. Then we let the thread run until its stack is empty 
and switch the context with the thread on top of `ready queue`. And then if the 
thread to be deleted is not in zombie state, then we change it to zombie state 
and push it on `zombie queue`. If it is already in zombie state, then we use 
`queue_delete()` to delete the thread. Finally, we push the thread on top of 
`ready queue` to the `running queue`.

Function `uthread_join()` focuses on two cases that the thread can be joined. 
Can either be completed (in `zombie queue`) or still need to finish execution 
(in `ready queue`). First, we check if it is in `zombie queue`, but dequeuing 
in the whole queue and saving it in a copy, and the restoring the `zombie 
queue` again. If it was in the `zombie queue`, we call `uthread_exit()` to 
delete and collect the thread.

### Preemption API 

The Preemption API manages uncoorperative threads to share the processing 
resources. 

Fuction `preempt_start()` starts off by setting up a handler for the timer by 
initalizing an instance of a sigaction object. We install it by assigning its 
handler to a timer interrupt handler that forcefully calls `uthread_yield()` on 
the currently running thread. We then configurate a timer by initalizing an 
instance of a settimer with a itimercal object. The timer is to send a virtual 
alarm a hundred times per second.

Function `preempt_stop()` will restore the previous signal action, and restore 
the previous timer configuration. We reassigned the signal handler back to the 
timer handler. Also, the timer configurator will send a virtural alarm a 
hundreed times per second again.

Function `preempt_enable()` and `preempt_disable` both calls the `sigpromask()` 
to change the mask of the signals. `preempt_enable()` will block the signals 
and the `preempt_disable()` will unblock the signals.

## Testing

To test our Thread Library, we broke it down to three sections of testing and 
create functions that performs every edge condition that may rise when running 
the program:

1. A functioning, generic queue
2. A sucessful thread management has a flowing scheduling system
3. A working preemptive scheduler that situates uncooperative threads

## Sources

1. GNU C Library source links provided by Professor Porquet in `Project2.html`
2. https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/
3. https://www.ibm.com/docs/en/i/7.3?topic=handler-resetting-signal-action