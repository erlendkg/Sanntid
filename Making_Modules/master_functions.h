#ifndef MASTER_FUNCTIONS_H_DEF
#define MASTER_FUNCTIONS_H_DEF

#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "order_handling_functions.h"
#include "panel_lights.h"
#include "message_handling.h"
#include <pthread.h>


int main_server();

void* thread_main_server(void *net_status);
void* thread_send_orders_to_idle_elevators(void *elevatorInfo);

void update_elevinfo_for_message_type_1(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int msg_elevator_number, int msg_elevator_floor);
void create_and_send_message(Elevator_master_information Data_elevators, int elevator_id);

void unpack_message_to_variables(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor, int* light_status);

#endif
