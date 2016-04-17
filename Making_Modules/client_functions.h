#ifndef CLIENT_FUNCTIONS_H_DEF
#define CLIENT_FUNCTIONS_H_DEF


#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "basic_queue_functions.h"
#include "basic_light_functions.h"
#include "order_handling_functions.h"
#include "panel_lights.h"
#include <pthread.h>

typedef struct {
        int num;
        int desired_floor[10];
        int current_floor;
        int is_busy;
        int is_connected_to_network;
        int server_socket;
} Elevator_client_information;

pthread_mutex_t elevator_client_information_lock;
pthread_mutex_t send_message_lock;
pthread_mutex_t doors_open_lock;

int main_client(char const *server_ip);
int single_elevator_mode(Elevator_client_information *this_elevator, int *server_socket, char const *server_ip);
void* thread_single_elevator_button_input(void *this_elevator);
void* thread_single_elevator_carry_out_orders(void *this_elevator);
//Client + network mode
void network_elevator_mode(Elevator_client_information *this_elevator, char const* server_ip);
void* thread_network_listen_for_button(void *this_elevator);
void* thread_carry_out_orders_network_mode(void *this_elevator);
void* thread_send_to_master(void *this_elevator);

void* recieve_messages_from_server(void* this_elevator);
void* thread_carry_out_orders(void* this_elevator);


#endif
