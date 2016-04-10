#ifndef QUEUE_FUNCTIONS_H_DEF
#define QUEUE_FUNCTIONS_H_DEF

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define QUEUE_SIZE 10
#define MAX_NUMBER_OF_ELEVATORS 100

typedef struct {
  int queue[QUEUE_SIZE];
  int queue_size;
  int status; // 0 Is up, 1 is down, 2 is idle. -1 is elevator not connected
  int current_floor;
  int last_order;
  int socket;
  int length_of_elevator_array; //elev 99 contains this info
} Elevator_data;

pthread_mutex_t lock;

//Basic array functions
void print_queue(int order_queue[QUEUE_SIZE]);
int flush_order_queue(int order_queue[QUEUE_SIZE], size_t size_of_order_queue);
void remove_item_from_queue(int order_queue[QUEUE_SIZE]);
void insert_item(int order_queue[QUEUE_SIZE], int pos, int num);

//logic and browsing functions
int is_elevator_disabled(int status);
int is_order_on_the_way(int current_floor, int status, int button_order);

void update_elevator_struct(int order_queue[QUEUE_SIZE], int * status, int * queue_size, int current_floor);

//Placing functions
void place_order_on_the_way(int order_queue[QUEUE_SIZE], int *status, int button_order);
void place_order_not_on_the_way(int order_queue[QUEUE_SIZE], int * status, int button_order);
void place_bt2_order( Elevator_data * E, int button_order);
void place_bt1_order( Elevator_data E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array);
void place_bt0_order( Elevator_data E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array);
void add_new_order_to_queue( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int desired_floor, int buttonType, int elevator, int length_of_elevator_array);

//Crash handling Functions
void disable_elevator_and_distribute_queue_to_other_elevators( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int disconnectedSocket);

//Message handling functions
void unpack_message_to_variables(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor, int* light_status);

//Startup functions
void initiate_queues( Elevator_data E[MAX_NUMBER_OF_ELEVATORS]);

//New elevator connected functions
int assign_number_to_new_elevator( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators);
void activate_single_queue( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int elevatorNumber);

//When recieving a message from master
char *act_on_message_from_master( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], char *messageFromElevator, int length_of_elevator_array);

#endif
