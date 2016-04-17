#ifndef ORDER_HANDLING_FUNCTIONS_H_DEF
#define ORDER_HANDLING_FUNCTIONS_H_DEF

#include "basic_queue_functions.h"
#include "message_handling.h"


#define GOING_UP 0
#define GOING_DOWN 1
#define IDLE 2
#define DISABLED -1

#define BUTTON_CALL_UP 0
#define BUTTON_CALL_DOWN 1
#define BUTTON_CALL_INTERNAL 2

typedef struct {
        int queue[QUEUE_SIZE];
        int queue_size;
        int status;
        int current_floor;
        int socket;
        int length_of_elevator_array;
} Elevator_master_information;

pthread_mutex_t lock;

void place_order_on_the_way(int order_queue[QUEUE_SIZE], int * status, int desired_floor, int button_order);
void place_order_not_on_the_way(int order_queue[QUEUE_SIZE], int * status, int desired_floor, int button_order);

void place_bt2_order( Elevator_master_information * E, int button_order);
void place_bt1_order( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array);
void place_bt0_order( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array);

int is_order_in_queue(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int desired_floor);
void add_new_order_to_queue( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int desired_floor, int buttonType, int elevator, int length_of_elevator_array);

int assign_number_to_new_elevator( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators);

void activate_single_queue( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int elevatorNumber);
void initiate_queues(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS]);

//Crash handling Functions
void disable_elevator_and_distribute_queue_to_other_elevators( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int disconnectedSocket);

int is_elevator_disabled(int status);
int is_order_on_the_way(int current_floor, int status, int desired_floor);

void update_elevator_status_and_queuesize(int order_queue[QUEUE_SIZE], int * status, int * queue_size, int current_floor);


#endif
