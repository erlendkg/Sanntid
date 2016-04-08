#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "elev.h"

typedef struct {
  int num;
  int desired_floor;
  int current_floor;
  int is_busy;
  int button_click;
  int button_floor;
  int button_type;  //0 = up, 1 = down, 2 = inside
  int is_connected_to_network;
  int server_socket;
} Elev_info;

pthread_mutex_t elev_info_lock;

int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip);
int network_elevator_mode(Elev_info *this_elevator, int server_socket, char const *server_ip);
int main_server();
int main_client(char const *server_ip);

void* thread_listen_for_button_input(void *this_elevator);
void* thread_carry_out_orders(void *this_elevator);
