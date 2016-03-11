#include <string.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 100

int assign_order_to_queue(int order, int (order_queue[MAX_QUEUE_SIZE]), int max_queue_size);
int read_order_from_queue(int order_queue[MAX_QUEUE_SIZE], int max_queue_size, int index_of_order);
int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue);
void test_queue_functions();
