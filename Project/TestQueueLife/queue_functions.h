#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAX_QUEUE_SIZE 10
#define N_ELEVATORS 3

struct Elevator_data {
  int queue[MAX_QUEUE_SIZE];
  int queueSize;
  int status; // 0 Is up, 1 is down, 2 is idle.
  int currentFloor;
  int last_order;
};

//Basic array functions
int assign_order_to_queue(int order, int (order_queue[MAX_QUEUE_SIZE]), int max_queue_size);
void print_queue(int order_queue[MAX_QUEUE_SIZE]);
int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue);
void removeItemFromQueue(int order_queue[MAX_QUEUE_SIZE]);
int insert_item(int order_queue[MAX_QUEUE_SIZE], int pos, int num);

//logic and browsing arrays
int is_order_on_the_way(int currentFloor, int status, int button_order);
void place_order_on_the_way(int order_queue[MAX_QUEUE_SIZE], int *status, int button_order);
void place_order_not_on_the_way(int order_queue[MAX_QUEUE_SIZE], int * status, int button_order);

//Subfunctions
void updateElevatorStruct(int order_queue[MAX_QUEUE_SIZE], int * status, int * queueSize, int currentFloor);
void place_bt2_order(struct Elevator_data * E, int button_order);
void place_bt1_order(struct Elevator_data E[N_ELEVATORS-1], int button_order);
void place_bt0_order(struct Elevator_data E[N_ELEVATORS-1], int button_order);

//Functions for master to run
void initiateQueues(struct Elevator_data E[N_ELEVATORS]);
void addNewOrderToQueue(struct Elevator_data E[N_ELEVATORS], int desired_floor, int buttonType, int elevator);
int isElevatorOnCorrectFloor(struct Elevator_data * E, int MsgFloor);

//Test functions (test_functions.c)
void test_queue_functions(int order_queue[MAX_QUEUE_SIZE]);
void test_onway_functions();
void bt2Test_queues_initiate(struct Elevator_data * E);
void bt2Test();
void bt01Test_queues_initiate(struct Elevator_data E[N_ELEVATORS-1]);
void test_all_buttons();
void test_addorder();
