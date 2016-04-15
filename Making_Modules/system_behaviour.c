#include "system_behaviour.h"
#include "panel_lights.h"

pthread_mutex_t net_stat_lock;
pthread_mutex_t elev_info_lock;
pthread_mutex_t send_message_lock;
pthread_mutex_t doors_open_lock;
int lamp_matrix[N_FLOORS][2];

int main_client(char const *server_ip) {
  int server_socket;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));
  pthread_mutex_init(&elev_info_lock, NULL);
  pthread_mutex_init(&send_message_lock, NULL);
  pthread_mutex_init(&doors_open_lock, NULL);

  initialize_lamp_matrix(lamp_matrix);
  initialize_hardware();
  update_panel_lights(lamp_matrix);

  this_elevator->is_busy = 0;
  this_elevator->is_connected_to_network = 0;

  while(1) {
    if ((this_elevator->is_connected_to_network) == 0) {
      printf("No network connection could be established\n");
      printf("Currently Running in single elevator mode\n");
      single_elevator_mode(this_elevator, &server_socket, server_ip);
      this_elevator->is_connected_to_network = 1;
      printf("Network connection established\n");
      printf("Switching to network mode\n");
      }

    if (this_elevator->is_connected_to_network == 1) {
        printf("Elevator is now i network mode");
        network_elevator_mode(this_elevator, server_ip);
        this_elevator->is_connected_to_network = 0;
      }
    }
  }

int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  int i,current_floor;
  pthread_t button_input, carry_out_orders, lights;

  this_elevator->is_busy = 0;
  this_elevator->current_floor = return_current_floor();
  this_elevator->desired_floor[0] = 1;

  for (i = 1; i <10; i++) {
    this_elevator->desired_floor[i] = 0;
  }

  pthread_create(&carry_out_orders, NULL, thread_single_elevator_carry_out_orders, (void* ) this_elevator);
  pthread_create(&button_input, NULL, thread_single_elevator_button_input, (void* ) this_elevator);
  pthread_create(&lights, NULL, thread_update_lights, NULL);

  while(1) {
    if((current_floor = return_current_floor()) != -1) {
      this_elevator->current_floor = current_floor;
    }

    if((*server_socket = initialize_client_socket(server_ip)) != 2) {
      pthread_cancel(button_input);
      pthread_cancel(carry_out_orders);
      pthread_cancel(lights);
      this_elevator->server_socket = *server_socket;
      //sende info til server
      return 0;
    }
  }
  pthread_join(carry_out_orders, NULL);
  pthread_join(button_input, NULL);
}

void* thread_single_elevator_button_input(void *this_elevator) {

  int floor;
  int i;
  Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

  while(1) {

    for(floor=0; floor<4; floor++){

          if (elev_get_button_signal(2, floor) == 1){
            inner_button_light_switch(floor,1);
            pthread_mutex_lock(&elev_info_lock);
            cast_this_elevator->button_floor = (floor );
            cast_this_elevator->button_type = 2;
            cast_this_elevator->button_click = 1;
            for (i = 0; i < 10; i++) {
              if(cast_this_elevator->desired_floor[i] == floor){
                break;
              }
              else if(cast_this_elevator->desired_floor[i] == 0) {
                cast_this_elevator->desired_floor[i] = floor+1;
                pthread_mutex_unlock(&elev_info_lock);
                sleep(1);
                break;
            }
          }
        }
        if (elev_get_button_signal(1, floor) == 1){
            update_lamp_matrix(lamp_matrix, floor, 1,1);
            pthread_mutex_lock(&elev_info_lock);
            cast_this_elevator->button_floor = (floor );
            cast_this_elevator->button_type = 1;
            cast_this_elevator->button_click = 1;
            for (i = 0; i < 10; i++) {
              if(cast_this_elevator->desired_floor[i] == floor){
                break;
              }
              if(cast_this_elevator->desired_floor[i] == 0) {
                cast_this_elevator->desired_floor[i] = floor+1;
                pthread_mutex_unlock(&elev_info_lock);
                sleep(1);
                break;

            }
          }
        }
        if (elev_get_button_signal(0, floor) == 1){
          update_lamp_matrix(lamp_matrix, floor, 0,1);
          pthread_mutex_lock(&elev_info_lock);
          cast_this_elevator->button_floor = (floor );
          cast_this_elevator->button_type = 0;
          cast_this_elevator->button_click = 1;
          for (i = 0; i < 10; i++) {
            if(cast_this_elevator->desired_floor[i] == floor){
              break;
            }
            if(cast_this_elevator->desired_floor[i] == 0) {
              cast_this_elevator->desired_floor[i] = floor+1;
              pthread_mutex_unlock(&elev_info_lock);
              sleep(1);
              break;

          }
        }
      }
    }
  }
}

void* thread_single_elevator_carry_out_orders(void *this_elevator) {

  int i;
  Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

  while(1) {
    if(cast_this_elevator->is_busy == 0) {
      cast_this_elevator->is_busy = 1;
      for(i = 0; i < 10; i++) {
        if(cast_this_elevator->desired_floor[i] != 0){
          go_to_floor(cast_this_elevator->desired_floor[i]);
          inner_button_light_switch(cast_this_elevator->desired_floor[i]-1,0);
          update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[i]-1,0,0);
          update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[i]-1,1,0);
          pthread_mutex_lock(&elev_info_lock);
          cast_this_elevator->desired_floor[i] = 0;
          pthread_mutex_unlock(&elev_info_lock);
          hold_doors_open(1);

        }
      }
      cast_this_elevator->is_busy = 0;
    }
  }
  return NULL;
}

void addButtonLightsToQueue(int lamp_matrix[N_FLOORS][2], int elevator_queue[10]){

  int matrix_row_counter, matrix_column_counter, queue_counter;
  for (matrix_row_counter = 0; matrix_row_counter < N_FLOORS; matrix_row_counter++){
    for (matrix_column_counter = 0; matrix_column_counter < 2; matrix_column_counter++){
      for (queue_counter = 0; queue_counter < 10; queue_counter++){
        if(lamp_matrix[matrix_row_counter][matrix_column_counter] == elevator_queue[queue_counter]){
          break;
        }
        else if(elevator_queue[queue_counter] == 0) {
          insert_item(elevator_queue, queue_counter, lamp_matrix[matrix_row_counter][matrix_column_counter]);
          break;
        }
      }
    }
  }
}

void network_elevator_mode(Elev_info *this_elevator, char const *server_ip) {
  this_elevator->is_connected_to_network = 1;
  pthread_t button_input, main_client, message_to_master, orders, lights;
  int server_socket = this_elevator->server_socket;

  this_elevator->desired_floor[0] = 1;

  pthread_create(&main_client, NULL, recieve_messages_from_server, (void*) this_elevator);
  pthread_create(&button_input, NULL, thread_network_listen_for_button, (void*) this_elevator);
  pthread_create(&orders, NULL, thread_carry_out_orders, (void*) this_elevator);
  pthread_create(&lights, NULL, thread_update_lights, (void*) this_elevator);

  pthread_join(main_client, NULL);
  pthread_cancel(button_input);
  pthread_cancel(orders);
  pthread_cancel(lights);

  this_elevator->server_socket = 0;
  this_elevator->is_connected_to_network = 0;

  single_elevator_mode(this_elevator,&server_socket, server_ip);
  }

//Gi main client og main server bedre navn
void* thread_carry_out_orders_network_mode(void *this_elevator){
  char message[512];
  Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

    if (cast_this_elevator->desired_floor[0] != cast_this_elevator->current_floor) {

      go_to_floor(cast_this_elevator->desired_floor[0]);
      inner_button_light_switch(cast_this_elevator->desired_floor[0]-1,0);

      pthread_mutex_lock(&doors_open_lock);
      hold_doors_open(1);
      pthread_mutex_unlock(&doors_open_lock);

      cast_this_elevator->current_floor = return_current_floor();

      sprintf(message, "<1E%dF%d>", cast_this_elevator->num, cast_this_elevator->current_floor);
      send(cast_this_elevator->server_socket, message, sizeof(message), 0);
      }
      return NULL;
}

void* thread_network_listen_for_button(void *this_elevator) {

    int floor;
    Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);
    char messageToMaster[512];
    clock_t start_t0 ,start_t1, start_t2, stop_t0, stop_t1, stop_t2;
    double dt0, dt1, dt2;
    int val;

    start_t0 = clock();
    start_t1 = clock();
    start_t2 = clock();


    while(1) {

      for(floor=1; floor <= 4; floor++){
        if (elev_get_button_signal(2, floor-1) == 1){

            stop_t2 = clock();
            dt2 = (double)(stop_t2 - start_t2)/CLOCKS_PER_SEC;

            if(dt2 > 0.5){
              start_t2 = clock();

              sprintf(messageToMaster, "<2E%dM%d>", cast_this_elevator->num, (floor*10 + 2));
            	printf("SEND to master: %s\n", messageToMaster);

              val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
              printf("Bytes sent: %d\n", val);
              memset(messageToMaster, 0, sizeof(messageToMaster));
              inner_button_light_switch(floor-1, 1);
        }
      }

        if (elev_get_button_signal(1, floor-1) == 1){

          stop_t1 = clock();

          dt1 = (double)(stop_t1 - start_t1)/CLOCKS_PER_SEC;

          if(dt1 > 0.5){
            start_t1 = clock();

            sprintf(messageToMaster, "<2E%dM%d>", cast_this_elevator->num, (floor*10 + 1));
            printf("SEND to master: %s\n", messageToMaster);

            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
            printf("Bytes sent: %d\n", val);
            memset(messageToMaster, 0, sizeof(messageToMaster));

        }
      }
        if (elev_get_button_signal(0, floor-1) == 1){

          stop_t0 = clock();

          dt0 = (double)(stop_t0 - start_t0)/CLOCKS_PER_SEC;

          if(dt0 > 0.5){
            start_t0 = clock();

            sprintf(messageToMaster, "<2E%dM%d>", cast_this_elevator->num, (floor*10));
            printf("SEND to master: %s\n", messageToMaster);

            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
            printf("Bytes sent: %d\n", val);
            memset(messageToMaster, 0, sizeof(messageToMaster));
        }
       }
     }
    }

    return NULL;
  }

int main_server() {
  Network_status *net_status = malloc(sizeof(Network_status));
  pthread_t listen_for_clients;
  initialize_lamp_matrix(lamp_matrix);

  net_status->master_socket = initialize_server_socket();
  pthread_mutex_init(&net_stat_lock, NULL);

  pthread_create(&listen_for_clients, NULL, thread_main_server, (void *) net_status);


  pthread_join(listen_for_clients, NULL);

  return 0;
}

void* thread_main_server(void *net_status) {

  pthread_t send_order_to_elevator;
  Network_status *my_net_status = (Network_status *) net_status;
  struct sockaddr_in address;
  int max_sd, sd, activity;
  int new_socket, new_queue_number;
  int valread, i;
  fd_set readfds;
  int addrlen = sizeof address;
  int message_type, elevator_id, current_floor, button_type, button_floor, queue_message;
  struct timeval timeout;
  timeout.tv_sec = 30;
  timeout.tv_usec = 0;

  char buffer[512];
  char return_message[512];
  Elevator_data Data_elevators[MAX_NUMBER_OF_ELEVATORS];

  initiate_queues(Data_elevators);

  pthread_create(&send_order_to_elevator, NULL, thread_send_orders_to_idle_elevators, Data_elevators);

  if(listen(my_net_status->master_socket, MAX_NUMBER_OF_ELEVS) < 0) {
    perror("listen");
    exit(1);
  }

  while(1) {
    FD_ZERO(&readfds);

    max_sd = add_all_socks_to_fdlist(&readfds, my_net_status);

    if((activity = select(max_sd +1, &readfds, NULL, NULL, NULL)) == 0) {
      printf("Timeout: server has most likely lost it's connection to the clients\n");
      printf("Please check your network connection and restart the server");
      exit(1);
    }
    else {

    if(FD_ISSET(my_net_status->master_socket, &readfds)) {
      if (accept_client(my_net_status, &new_socket) == -1) {
      printf("Failed to accept client\n");
      }
      else {
        //INITIATE CONTACT ELEVATOR
        new_queue_number = assign_number_to_new_elevator(Data_elevators, Data_elevators[99].length_of_elevator_array);
        activate_single_queue(Data_elevators, new_queue_number);
        Data_elevators[99].length_of_elevator_array ++;
        Data_elevators[new_queue_number].socket = new_socket;
        send(Data_elevators[new_queue_number].socket, &new_queue_number, sizeof(new_queue_number), 0);

      }
    }
    for(i = 0; i < MAX_NUMBER_OF_ELEVS; i++) {
      sd = my_net_status->client_sockets[i];
      if(FD_ISSET(sd, &readfds)) {
        if((valread = recv(sd, &buffer, sizeof(buffer),0)) <= 0) {
          getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
          printf("Client disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          disable_elevator_and_distribute_queue_to_other_elevators(Data_elevators, sd);
          //Close the socket and mark as 0 in list for reuse
          close(sd);
          my_net_status->client_sockets[i] = 0;
          my_net_status->active_connections -= 1;
        }
        else{
          printf("\n\nMottatt meldingen %s på socket %d\n",buffer, sd );

          message_type = buffer[1] - '0';

          printf("Messagetype is: %d\n", message_type);

          if (message_type == 1){
            unpack_current_floor_message(buffer, &elevator_id, &current_floor);
            queue_format_to_floor_and_button(Data_elevators[elevator_id].queue[0], &button_floor, &button_type);

            if (button_floor == current_floor){
              update_lamp_matrix(lamp_matrix, current_floor-1, button_type,0);
            }

            update_elevinfo_for_message_type_1(Data_elevators, elevator_id, current_floor);

            if (Data_elevators[elevator_id].queue[0] == 0){

              Data_elevators[elevator_id].status = 2;
              printf("The queue is empty\n update status: 2\n");
              break;
            }
            create_and_send_message(Data_elevators[elevator_id], elevator_id);
          }
          else if (message_type == 2){
              unpack_button_click_message(buffer, &elevator_id, &button_type, &button_floor, &queue_message);
              add_new_order_to_queue(Data_elevators, queue_message, button_type, elevator_id, Data_elevators[99].length_of_elevator_array);
              if(button_type != 2){
                update_lamp_matrix(lamp_matrix, button_floor-1, button_type, 1);
                }
              }
          for(int i = 0; i < Data_elevators[99].length_of_elevator_array; i++){
            if (Data_elevators[i].status != -1){
              printf("\n\n******************************************\n");
              printf("Queue number: %d\t", i);
              printf("Status: %d\n", Data_elevators[i].status);
              printf("Current_floor: %d\n", Data_elevators[i].current_floor);
              print_queue(Data_elevators[i].queue);
              printf("******************************************\n\n");
            }
          }
          memset(buffer, 0, sizeof(buffer));
          memset(return_message, 0, sizeof(return_message));
        }
      }
      }
    }
  }
}

void* thread_send_orders_to_idle_elevators(void* elevatorInfo){
    printf("Entering send to elevators thread\n");
    Elevator_data *myElevatorInfo = (Elevator_data *)elevatorInfo;
    char messageToElevator[512];
    int i;

    while(1){
      sleep(1);
      for( i = 0; i <= myElevatorInfo[99].length_of_elevator_array; i++ ){

        if (myElevatorInfo[i].status == 2 && myElevatorInfo[i].queue[0] != 0) {
          create_and_send_message(myElevatorInfo[i], i);
        }
        bundle_lamp_matrix(messageToElevator,lamp_matrix);
        send(myElevatorInfo[i].socket, messageToElevator, sizeof(messageToElevator),0);
        memset(messageToElevator, 0, sizeof(messageToElevator));
      }
    }
    return NULL;
  }

void create_and_send_message(Elevator_data Data_elevators, int elevator_id){

  int new_desired_floor = 0;
  int temp_queue_instance = Data_elevators.queue[0];
  char return_message[512];

    while(temp_queue_instance >= 10) {
      new_desired_floor++;
      temp_queue_instance -= 10;
    }

    sprintf(return_message, "<1E%dF%d>", elevator_id, new_desired_floor);
    send(Data_elevators.socket, return_message, sizeof(return_message), 0);


}

void unpack_message_to_variables(char* str, int* msgType, int* elevatorNumber, int* buttonType, int* elevatorFloor, int* light_status) {


    int tempMsgType = -1, tempMsgEl = -1, tempMsgFloor = -1, tempMsgButton = -1, temp_light_status = -1;

    tempMsgType = str[1] - '0';


    if (tempMsgType == 1) {

      sscanf(str, "<1E%dF%d>", &tempMsgEl, &tempMsgFloor);
    }

    else if (tempMsgType == 2) {

      sscanf(str, "<2E%dBT%dF%d>", &tempMsgEl, &tempMsgButton, &tempMsgFloor);

    } else if (tempMsgType == 3) {
      sscanf(str, "<3F%dBT%dT%d>", &tempMsgEl, &tempMsgButton, &temp_light_status);
    }

    *msgType = tempMsgType;
    *elevatorNumber = tempMsgEl;
    *buttonType = tempMsgButton;
    *elevatorFloor = tempMsgFloor;
    *light_status = temp_light_status;
  }

void update_elevinfo_for_message_type_1(Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int msg_elevator_number, int msg_elevator_floor){

  int is_elevator_on_correct_floor;

  E[msg_elevator_number].current_floor = msg_elevator_floor;

  int temp_queue_instance = E[msg_elevator_number].queue[0];
  int floor_in_queue = 0;

  while(temp_queue_instance >= 10) {
    floor_in_queue++;
    temp_queue_instance -= 10;
  }

  is_elevator_on_correct_floor = (floor_in_queue == msg_elevator_floor);

  if (is_elevator_on_correct_floor == 1){
    remove_item_from_queue(E[msg_elevator_number].queue);
  }
}

//New Threads
void* recieve_messages_from_server(void* this_elevator) {
  Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
  char message[512];
  int elevator_id, new_desired_floor = -1, current_floor;

  if(recv(my_this_elevator->server_socket, &elevator_id, sizeof(int),0) == 0) {
    printf("Disconnected from server\n");
  }

  my_this_elevator->num = elevator_id;
  printf("My number is %d", elevator_id);
  memset(message,0,512);

  while(1) {
    send(my_this_elevator->server_socket, "Alive",5,0);

    if (listen_for_message_from_master(message, my_this_elevator->server_socket, 512) == -1){
      printf("Disconnected from master\n");
      return NULL;
    } else {
      printf("*****************************\n");
      printf("Message from master: %s\n", message);
      printf("*****************************\n\n");

      if((message[0] - '0')  == 3) {
        initialize_lamp_matrix(lamp_matrix);
        unbundle_lamp_matrix(message, lamp_matrix);
        memset(message, 0, 512);
      } else if((message[1] - '0') == 1) {
        unpack_current_floor_message(message, &elevator_id, &new_desired_floor);
        if(new_desired_floor != my_this_elevator->desired_floor[0]){
          my_this_elevator->desired_floor[0] = new_desired_floor;
        }
        memset(message,0,512);
      }
    }

    current_floor = return_current_floor();

    if((my_this_elevator->current_floor != current_floor) && (current_floor != -1)) {
      memset(message,0,512);
      my_this_elevator->current_floor = current_floor;
      printf("Elev is now on floor: %d, going to: %d\n", my_this_elevator->current_floor, my_this_elevator->desired_floor[0] );
      printf("Elev num is: %d\n", my_this_elevator->num );
      sprintf(message, "<1E%dF%d>", my_this_elevator->num, my_this_elevator->current_floor);
      send(my_this_elevator->server_socket, message, 512, 0);
      memset(message,0,512);
    }
  }
}

void* thread_carry_out_orders(void* this_elevator) {
  Elev_info *my_this_elevator = (Elev_info *) this_elevator;
  char message[512];

  while(1) {
    if((my_this_elevator->desired_floor[0] != my_this_elevator->current_floor) && (my_this_elevator->desired_floor[0] != 0)) {
      go_to_floor(my_this_elevator->desired_floor[0]);
      inner_button_light_switch(my_this_elevator->desired_floor[0]-1,0);

      pthread_mutex_lock(&doors_open_lock);
      hold_doors_open(1);
      pthread_mutex_unlock(&doors_open_lock);

      my_this_elevator->current_floor = return_current_floor();

      sprintf(message, "<1E%dF%d>", my_this_elevator->num, my_this_elevator->current_floor);
      send(my_this_elevator->server_socket, message, 512, 0);
    }
  }
}

void* thread_update_lights() {
  int current_floor;

  while(1) {
    current_floor = return_current_floor();
    update_panel_lights(lamp_matrix);
    if(current_floor != -1) {
      update_floor_indicator((current_floor)-1);
    }
  }
}

//Old Threads
/*
void* thread_send_to_master(void *this_elevator){
    Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

    char message[512];
    int New_floor;

    memset(message, 0, sizeof(message));
      while(1) {
        if(cast_this_elevator->is_connected_to_network == 1) {

          New_floor = return_current_floor();

          if(cast_this_elevator->current_floor != New_floor && New_floor != -1){
            memset(message, 0, sizeof(message));

            cast_this_elevator->current_floor = New_floor;

            printf("Elev is now on floor: %d, going to: %d\n", cast_this_elevator->current_floor, cast_this_elevator->desired_floor[0] );
            printf("Elev num is: %d\n", cast_this_elevator->num );

            sprintf(message, "<1E%dF%d>", cast_this_elevator->num, cast_this_elevator->current_floor);

            send(cast_this_elevator->server_socket, message, sizeof(message), 0);
        }
      }
    }
  }

void* thread_recieve_orders_and_operate_elevator(void *this_elevator) {
  Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
  int message_type, b, c, new_desired_floor, light_status;
  int current_floor;
  int initial_message;
  char buffer[512];
  pthread_t carry_out_orders;
  memset(buffer, 0, sizeof(buffer));


  if(recv(my_this_elevator->server_socket, &initial_message, sizeof(initial_message),0) == 0) {
    printf("Disconnected from server\n");
  }

  my_this_elevator->num = initial_message;
  printf("My number is %d", initial_message);

  while(1) {
    send(my_this_elevator->server_socket, "Alive",5,0);

    if((current_floor = return_current_floor()) != -1) {
      update_floor_indicator(current_floor-1);
    }

    if (listen_for_message_from_master(buffer, my_this_elevator->server_socket, sizeof(buffer)) == -1){
      printf("Disconnected from master\n");
      return NULL;
    } else {
      printf("*****************************\n");
      printf("Message from master: %s\n", buffer);
      printf("*****************************\n\n");

      if((buffer[0] - '0')  == 3) {
        initialize_lamp_matrix(lamp_matrix);
        unbundle_lamp_matrix(buffer, lamp_matrix);
        update_panel_lights(lamp_matrix);
        memset(buffer, 0, sizeof(buffer));
      } else {

      unpack_message_to_variables(buffer, &message_type, &b, &c, &new_desired_floor, &light_status);
      memset(buffer, 0, sizeof(buffer));

      if(new_desired_floor != my_this_elevator->desired_floor[0]){
        my_this_elevator->desired_floor[0] = new_desired_floor;
        pthread_mutex_lock(&doors_open_lock);
        printf("Operator locked mutex\n");

        printf("Operator unlocked mutex\n");
        pthread_mutex_unlock(&doors_open_lock);

        pthread_create(&carry_out_orders, NULL, thread_carry_out_orders_network_mode, (void*) this_elevator);
        my_this_elevator->current_floor = return_current_floor();

        }
      }
    }
  }
}
*/
