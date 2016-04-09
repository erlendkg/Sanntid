#include "system_behaviour.h"

pthread_mutex_t net_stat_lock;
pthread_mutex_t elev_info_lock;

int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  int i;
  pthread_t button_input, carry_out_orders;

  this_elevator->is_busy = 0;
  this_elevator->current_floor = return_current_floor();
  for (i = 1; i <10; i++) {
    this_elevator->desired_floor[i] = 0;
  }

  pthread_create(&carry_out_orders, NULL, thread_carry_out_orders, (void* ) this_elevator);
  pthread_create(&button_input, NULL, thread_listen_for_button_input, (void* ) this_elevator);

  while(1) {
    this_elevator->current_floor = return_current_floor();

    if((*server_socket = initialize_client_socket(server_ip)) != 2) {
      pthread_cancel(button_input);
      pthread_cancel(go_to_floor);
      this_elevator->server_socket = *server_socket;
      //sende info til server

      return 0;
    }
  }
  pthread_join(carry_out_orders, NULL);
  pthread_join(button_input, NULL);
}

int network_elevator_mode(Elev_info *this_elevator, int server_socket, char const *server_ip) {
  this_elevator->is_connected_to_network = 1;
  pthread_t button_input, carry_out_orders, main_client;
  int a, bytes;
  char message[sizeof(Elev_info)];
  int length = sizeof(Elev_info);

  pthread_create(&button_input, NULL, thread_listen_for_button_input, (void*) this_elevator);
  pthread_create(&carry_out_orders, NULL, thread_carry_out_orders, (void*) this_elevator);
  pthread_create(&main_client, NULL, thread_main_client, (void*) this_elevator);

  while(1) {
    if(this_elevator->is_connected_to_network == 1) {
      this_elevator->current_floor = return_current_floor();
	     printf("Recieving order from master: Which floor?\n");

       sprintf(message, "<1E%dF%d>", this_elevator->num, this_elevator->current_floor);

	      printf("SEND MESSAGE TO MASTER\n");
        send_all(server_socket, message, &length);
      }
    }
  }

int main_server() {
  Network_status *net_status = malloc(sizeof(Network_status));
  pthread_t listen_for_clients, listen_to_elevators;

  net_status->master_socket = initialize_server_socket();
  pthread_mutex_init(&net_stat_lock, NULL);

  pthread_create(&listen_for_clients, NULL, thread_main_server, (void *) net_status);


  pthread_join(listen_for_clients, NULL);

  return 0;
}

int main_client(char const *server_ip) {
  int server_socket, networkModeActive;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));

  pthread_mutex_init(&elev_info_lock, NULL);
  initialize_hardware();

  this_elevator->current_floor = return_current_floor();

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

void* thread_main_client(void *this_elevator) {
  Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
  int buffer_size = sizeof(Elev_info);
  char buffer[buffer_size];

  while(1) {
    if (listen_for_message_from_master(buffer, my_this_elevator->server_socket, buffer_size) == -1) {
      printf("Connection to server lost\n");
      my_this_elevator->is_connected_to_network = 0;
    } else {
      printf("%s\n", buffer);
    }
  }
}

void* thread_main_server(void *net_status) {

  Network_status *my_net_status = (Network_status *) net_status;
  struct sockaddr_in address;
  int max_sd, sd, activity;
  int new_socket;
  int valread, i;
  fd_set readfds;
  int addrlen = sizeof address;

  char buffer[sizeof(Network_status)];

  if(listen(my_net_status->master_socket, MAX_NUMBER_OF_ELEVS) < 0) {
    perror("listen");
    exit(1);
  }

  while(1) {
    FD_ZERO(&readfds);

    max_sd = add_all_socks_to_fdlist(&readfds, my_net_status);

    activity = select(max_sd +1, &readfds, NULL, NULL, NULL);

    if(FD_ISSET(my_net_status->master_socket, &readfds)) {
      if (accept_client(my_net_status) == -1) {
      printf("Failed to accept client\n");
      }
      else{
        //INITIATE CONTACT ELEVATOR
      }
    }
    for(i = 0; i < MAX_NUMBER_OF_ELEVS; i++) {
      sd = my_net_status->client_sockets[i];

      if(FD_ISSET(sd, &readfds)) {
        if((valread = read(sd, &buffer, 1024)) == 0) {
          getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
          printf("Client disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse
          close(sd);
          my_net_status->client_sockets[i] = 0;
          my_net_status->active_connections -= 1;
        }
        printf("%s", buffer);
      }
    }
  }
}

void* thread_carry_out_orders(void *this_elevator) {

    int floorSignal;
    int i;
    Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

    while(1) {
      if((cast_this_elevator->is_busy == 0)) {
        cast_this_elevator->is_busy = 1;
        for(i = 0; i < 10; i++) {
          if(cast_this_elevator->desired_floor[i] != 0){
            go_to_floor(cast_this_elevator->desired_floor[i]);
            pthread_mutex_lock(&elev_info_lock);
            cast_this_elevator->desired_floor[i] = 0;
            pthread_mutex_unlock(&elev_info_lock);
            hold_doors_open(1);
          }
          if(cast_this_elevator->desired_floor[i] == cast_this_elevator->current_floor) {
            printf("Hello\n");
            pthread_mutex_lock(&elev_info_lock);
            cast_this_elevator->desired_floor[i] = 0;
            pthread_mutex_unlock(&elev_info_lock);
          }
      }
      cast_this_elevator->is_busy = 0;
    }
  }
  return NULL;
}

void* thread_listen_for_button_input(void *this_elevator) {
  int floor;
  int i;
  Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

  while(1) {
    for(floor=0; floor<4; floor++){
      if (elev_get_button_signal(2, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Inside\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 2;
          cast_this_elevator->button_click = 1;
          for (i = 0; i < 10; i++) {
            if(cast_this_elevator->desired_floor[i] == 0) {
              cast_this_elevator->desired_floor[i] = (floor+1);
              pthread_mutex_unlock(&elev_info_lock);
              sleep(1);
              break;
            }
          }

      }
      if (elev_get_button_signal(1, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Down\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 1;
          cast_this_elevator->button_click = 1;
          for (i = 0; i < 10; i++) {
            if(cast_this_elevator->desired_floor[i] == 0) {
              cast_this_elevator->desired_floor[i] = (floor+1);
              pthread_mutex_unlock(&elev_info_lock);
              sleep(1);
              break;
            }
          }

      }
      if (elev_get_button_signal(0, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Up\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 0;
          cast_this_elevator->button_click = 1;
          for (i = 0; i < 10; i++) {
            if(cast_this_elevator->desired_floor[i] == 0) {
              cast_this_elevator->desired_floor[i] = (floor+1);
              pthread_mutex_unlock(&elev_info_lock);
              sleep(1);
              break;
            }
          }
        }
      }
    }
  }
