#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Destroy */
void test_destroy(void)
{
	queue_t q;
	fprintf(stderr, "*** TEST destroy ***\n");

	q = queue_create();
	TEST_ASSERT(queue_destroy(q) == 0);
}

/* Destroy Filled*/
void test_destroy_filled(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST destroy_filled ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Enqueue Empty Queue */
void test_enqueue_empty(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue_empty ***\n");

	q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &data) == 0);
}

/* Enqueue Filled Queue */
void test_enqueue_filled(void)
{
	int data1 = 3, data2 = 2, data3 = 6;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue_filled ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	TEST_ASSERT(queue_enqueue(q, &data3) == 0);
}

/* Dequeue Empty Queue */
void test_dequeue_empty(void)
{
	int *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_dequeue_empty ***\n");

	q = queue_create();
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
}

/* Dequeue Filled Queue */
void test_dequeue_filled(void)
{
	int data1 = 3, data2 = 2, data3 = 6, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_dequeue_filled ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_dequeue(q, (void**)&ptr);
	queue_enqueue(q, &data3);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data2);
}

/* Delete Empty Queue */
void test_delete_empty(void)
{
	int delete_data = 5;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_empty ***\n");

	q = queue_create();
	TEST_ASSERT(queue_delete(q, &delete_data) == -1);
}

/* Delete Filled with One Node Queue */
void test_delete_one_filled(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_one_filled ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_delete(q, &data) == 0);
}

/* Delete Filled with One Node (no match) Queue */
void test_delete_one_filled_noMatch(void)
{
	int data = 3, delete_data = 5;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_one_filled_noMatch ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_delete(q, &delete_data) == -1);
}

/* Delete Found in Head Queue */
void test_delete_head(void)
{
	int data1 = 3, data2 = 4, data3 = 5;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_head ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	TEST_ASSERT(queue_delete(q, &data1) == 0);
}

/* Delete Found in Tail Queue */
void test_delete_tail(void)
{
	int data1 = 3, data2 = 4, data3 = 5;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_tail ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	TEST_ASSERT(queue_delete(q, &data3) == 0);
}

/* Delete Found in Middle Queue */
void test_delete_middle(void)
{
	int data1 = 3, data2 = 4, data3 = 5, data4 = 6, data5 = 7;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_middle ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	TEST_ASSERT(queue_delete(q, &data4) == 0);
}

/* Delete Not Found Queue */
void test_delete_not_found(void)
{
	int data1 = 3, data2 = 4, data3 = 5, data4 = 6, data5 = 7, delete_data = 10;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_noMatch ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	TEST_ASSERT(queue_delete(q, &delete_data) == -1);
}

/* Length */
void test_queue_length(void)
{
	int data1 = 3, data2 = 4, data3 = 5, *ptr;
	queue_t q = NULL;

	fprintf(stderr, "*** TEST queue_length ***\n");

	TEST_ASSERT(queue_length(q) == -1);
	
	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	TEST_ASSERT(queue_length(q) == 3);

	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(queue_length(q) == 0);
}

/* Iterate */
static int inc_item(queue_t q, void *data, void *arg)
{
	int *a = (int*)data;
	int inc = (int)(long)arg;

	*a += inc;
	queue_length(q);
	
	return 0;
}

void test_queue_iterate(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
	size_t i;

	fprintf(stderr, "*** TEST queue_iterate ***\n");

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
	{
		queue_enqueue(q, &data[i]);
	}

	/* Add value '1' to every item of the queue, delete item '42' */
	queue_iterate(q, inc_item, (void*)1, NULL);
	TEST_ASSERT(data[0] == 2);
}

int main(void)
{
	test_create();
	test_destroy();
	test_destroy_filled();
	test_queue_simple();
	test_enqueue_empty();
	test_enqueue_filled();
	test_dequeue_empty();
	test_dequeue_filled();
	test_delete_empty();
	test_delete_one_filled();
	test_delete_one_filled_noMatch();
	test_delete_head();
	test_delete_tail();
	test_delete_middle();
	test_delete_not_found();
	test_queue_length();
	test_queue_iterate();
	return 0;
}
