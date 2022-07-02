#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
	int count;
	struct node {
		void *item;
		struct node *next;
		struct node *prev;
	}node;
	struct node *head;
	struct node *tail;
}q;

queue_t queue_create(void)
{
	struct queue *queue_name = malloc(sizeof(struct queue));
	if (queue_name == NULL)
	{
		return NULL;
	}
	queue_name->head = NULL;
	queue_name->tail = NULL;
	queue_name->count = 0;
	return queue_name;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->head != NULL)
	{
		return -1;
	}
	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
	{
		return -1;
	}
	struct node *newNode = malloc(sizeof(struct node));
  	newNode->item = data;
  	newNode->next = NULL;
	newNode->prev = NULL;
	queue->count++;
	if (queue->head != NULL) //if there is something in the queue
	{
		queue->tail->next = newNode;
		newNode->prev = queue->tail;
		queue->tail = newNode;
		queue->tail->next = NULL;
		return 0;
	}
	else //if there is nothing in the queue
	{
		queue->head = newNode;
		queue->tail = newNode;
		return 0;
	}

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	struct node *headNode;
	if (queue->count == 0) //if there's nothing in the queue to dequeue
	{
		return -1;
	}
	else if (queue->count == 1) //if there is only one node in the queue
	{
		headNode = queue->head;
		*data = headNode->item;
		free(headNode);
		queue->head = NULL;
		queue->tail = NULL;
		queue->count--;
	}
	else //if queue is normally filled, dequeue the first
	{
		headNode = queue->head;
		*data = headNode->item;
		queue->head = headNode->next;
		queue->head->prev = NULL;
		free(headNode);
		queue->count--;
	}
	if (*data == NULL) //if there's nothing in the data
	{
		return -1;
	}

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue->count == 0) //if queue is empty
	{
		return -1;
	}
	void *headItem = queue->head->item;
	void *tailItem = queue->tail->item;
	void *findData = data;
	struct node *currentNode;

	if (queue->count == 1 && headItem == findData) //if one node and found item
	{
		currentNode = queue->head;
		queue->head = NULL;
		queue->tail = NULL;
		free(currentNode);
		return 0;
	}
	else if (queue->count == 1 && headItem != findData) //if one node and not found item
	{
		return -1;
	}
	else if (headItem == findData)	//if multiple nodes and item found in head
	{
		currentNode = queue->head;
		queue->head = currentNode->next;
		queue->head->prev = NULL;
		currentNode->next = NULL;
		free(currentNode);
		return 0;
	}
	else if (tailItem == findData) //if multiple nodes and item found in tail
	{
		currentNode = queue->tail;
		queue->tail = currentNode->prev;
		queue->tail->next = NULL;
		currentNode->prev = NULL;
		free(currentNode);
		return 0;
	}
	else	//if multiple nodes look in the middle
	{
		currentNode = queue->head;
		void *currentItem = currentNode->item;
		while (currentItem != findData && currentNode != queue->tail)
		{
			currentNode = currentNode->next;
			currentItem = currentNode->item;
		}
		if (currentNode == queue->tail)
		{
			return -1;
		}
		struct node *prevTmp = currentNode->prev;
		struct node *nextTmp = currentNode->next;
		prevTmp->next = nextTmp;
		nextTmp->prev = prevTmp;
		currentNode->next = NULL;
		currentNode->prev = NULL;
		free(currentNode);
		return 0;
	}
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if(queue == NULL) //if queue does not exist
	{
		return -1;
	}

	struct node *itr = queue->head;
	while(itr != queue->tail->next) //iterating through queue and calling functions
	{
		void *item = itr->item;
		if(data != NULL)  //if address provided to store data
		{
			*data = item;
		}
		(*func)(queue, item, arg);
		itr = itr->next;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	if(queue == NULL) //if queue does not exist
	{
		return -1;
	}

	return queue->count;
}
