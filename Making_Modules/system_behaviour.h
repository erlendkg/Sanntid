#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "basic_queue_functions.h"
#include "basic_light_functions.h"
#include <pthread.h>


typedef struct {
  int num;
  int desired_floor[10];
  int current_floor;
  int is_busy;
  int button_click;
  int button_floor;
  int button_type;  //0 = up, 1 = down, 2 = inside
  int is_connected_to_network;
  int server_socket;
} Elev_info;



int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip);
void network_elevator_mode(Elev_info *this_elevator, char const* server_ip);
int main_server();
int main_client(char const *server_ip);

void* thread_listen_for_button_input_single_elevator_mode(void *this_elevator);
void* thread_listen_for_button_input_and_send_to_master(void *this_elevator);
void* thread_carry_out_orders_single_elevator_mode(void *this_elevator);
void* thread_carry_out_orders_network_mode(void *this_elevator);
void* thread_send_to_master(void *this_elevator);
void* thread_main_server(void *net_status);
void* thread_recieve_orders_and_operate_elevator(void *this_elevator);
void* thread_send_orders_to_idle_elevators(void *elevatorInfo);
