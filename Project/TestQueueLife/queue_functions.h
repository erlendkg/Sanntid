#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 10

struct Elevator_data {
  int queue[MAX_QUEUE_SIZE];
  int status; // 0 Is up, 1 is down, 2 is idle.
  int currentFloor;
  int last_order;
};

//Basic array functions
int assign_order_to_queue(int order, int (order_queue[MAX_QUEUE_SIZE]), int max_queue_size);
void print_queue(int order_queue[MAX_QUEUE_SIZE]);
int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue);
int remove_first_item(int order_queue[MAX_QUEUE_SIZE]);
int insert_item(int order_queue[MAX_QUEUE_SIZE], int pos, int num);

//logic and browsing arrays
int is_order_on_the_way(int currentFloor, int status, int button_order);
void place_order_on_the_way(int order_queue[MAX_QUEUE_SIZE], int *status, int button_order);
void place_order_not_on_the_way(int order_queue[MAX_QUEUE_SIZE], int * status, int button_order);


//real functions
void place_bt2_order(struct Elevator_data * E, int button_order);

//Test functions (test_functions.c)
void test_queue_functions(int order_queue[MAX_QUEUE_SIZE]);
void test_onway_functions();
void queues_initiate(struct Elevator_data * E);
