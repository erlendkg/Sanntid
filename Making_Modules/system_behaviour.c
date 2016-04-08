#include "system_behaviour.h"

int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  pthread_t button_input, go_to_floor;
  this_elevator->desired_floor = 1;

  pthread_create(&button_input, NULL, thread_listen_for_button_input, (void*) this_elevator);
  pthread_create(&go_to_floor, NULL, thread_carry_out_orders, (void *) this_elevator);

  while(1) {

    if((*server_socket = initialize_client_socket(server_ip)) != 2) {
      pthread_cancel(button_input);
      pthread_cancel(go_to_floor);
      return 0;
    }
  }
  pthread_join(go_to_floor, NULL);
  pthread_join(button_input, NULL);
}

int network_elevator_mode(Elev_info *this_elevator, int server_socket, char const *server_ip) {
  pthread_t button_input, go_to_floor, send_status_to_server, recieve_orders;
  int a, bytes;
  char *message;

  pthread_create(&button_input, NULL, thread_listen_for_button_input, (void*) this_elevator);

  elev_go_to_floorFUNCTION(this_elevator);

  while(1) {


	printf("Recieving order from master: Which floor?\n");
	scanf("%d",&a);
	this_elevator->desired_floor = a;

	elev_go_to_floorFUNCTION(this_elevator);

  sprintf(message, "<1E%dF%d>", this_elevator->num, this_elevator->current_floor);

	printf("SEND MESSAGE TO MASTER\n");
  bytes = send(server_socket, message, 32, 0);
  printf("%i ", bytes);


  }
}

void* thread_listen_for_button_input(void *this_elevator) {
  int floor;
  Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

  while(1) {

    for(floor=0; floor<4; floor++){
      if (elev_get_button_signal(2, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Inside\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 2;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
      }
      if (elev_get_button_signal(1, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Down\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 1;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
      }
      if (elev_get_button_signal(0, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Up\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 0;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
        }
      }
    }
  }

void* thread_carry_out_orders(void *this_elevator) {
    int floorSignal;
    Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

    while(1){
      if ((floorSignal = elev_get_floor_sensor_signal()) != -1) {
        if (floorSignal  != cast_this_elevator->current_floor) { //INSIDE HERE WE NEED TO SEND A MESSAGE TO MASTER **************************

          //pthread_mutex_lock(&elev_info_lock);
          cast_this_elevator->current_floor = elev_get_floor_sensor_signal() ;
          printf("Current Floor: %d    \tDesiredFloor: %d\n", cast_this_elevator->current_floor, cast_this_elevator->desired_floor);
          //pthread_mutex_unlock(&elev_info_lock);
          elev_set_floor_indicator((cast_this_elevator->current_floor));
      }
    }
      if(cast_this_elevator->desired_floor == cast_this_elevator->current_floor && cast_this_elevator->is_busy == 1){
        printf("Elevator have reached desired floor\n");
        cast_this_elevator->is_busy = 0;
      }

      if (cast_this_elevator->current_floor > cast_this_elevator->desired_floor){
        elev_set_motor_direction(DIRN_DOWN);

      }  else if (cast_this_elevator->current_floor < cast_this_elevator->desired_floor){
        elev_set_motor_direction(DIRN_UP);

      }  else if (cast_this_elevator->current_floor == cast_this_elevator->desired_floor){
        elev_set_motor_direction(DIRN_STOP);

      }

    }

    return NULL;
  }

int main_server() {
  Network_status *net_status = malloc(sizeof(Network_status));
  pthread_t listen_for_clients, listen_to_elevators;

  net_status->master_socket = initialize_server_socket();
  pthread_mutex_init(&net_stat_lock, NULL);

  pthread_create(&listen_for_clients, NULL, listen_for_incoming_connections, (void *) net_status);

  pthread_join(listen_for_clients, NULL);

    return 0;
  }

int main_client(char const *server_ip) {

  int server_socket, networkModeActive;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));

  pthread_mutex_init(&elev_info_lock, NULL);

  elev_init();
  elev_set_motor_direction(DIRN_STOP);

  this_elevator->current_floor = elev_get_floor_sensor_signal();

  this_elevator->is_busy = 0;
  this_elevator->is_connected_to_network = 0;

  while(1) {

    if ((this_elevator->is_connected_to_network) == 0) {
      printf("No network connection could be established\n");
      printf("Currently Running in single elevator mode\n");
      single_elevator_mode(this_elevator, &server_socket, server_ip);
  	//test_mode(this_elevator);
      this_elevator->is_connected_to_network = 1;
      printf("Network connection established\n");
      printf("Switching to network mode\n");
      }

    if (this_elevator->is_connected_to_network == 1) {
        //printf("Elevator is now i network mode");
      if (networkModeActive == 0) {
        network_elevator_mode(this_elevator, server_socket, server_ip);
        networkModeActive = 1;
        }
        //Hvis får ordre sender til server
        //Mottar ordre
        //Utfører Oppgave
        //Oppgave utført
      }
    }

  }
