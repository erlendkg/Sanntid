#ifndef SYSTEM_BEHAVIOUR_H_DEF
#define SYSTEM_BEHAVIOUR_H_DEF

#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "basic_queue_functions.h"
#include "basic_light_functions.h"
#include "order_handling_functions.h"
#include <pthread.h>

typedef struct {
        int num;
        int desired_floor[10];
        int current_floor;
        int is_busy;
        int button_click;
        int button_floor;
        int button_type; //0 = up, 1 = down, 2 = inside
        int is_connected_to_network;
        int server_socket;
} Elevator_client_information;



//Server
int main_server();

void* thread_main_server(void *net_status);
void* thread_send_orders_to_idle_elevators(void *elevatorInfo);

void update_elevinfo_for_message_type_1(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int msg_elevator_number, int msg_elevator_floor);
void create_and_send_message(Elevator_master_information Data_elevators, int elevator_id);

void unpack_message_to_variables(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor, int* light_status);

void* recieve_messages_from_server(void* this_elevator);
void* thread_carry_out_orders(void* this_elevator);

#endif
