#include "system_behaviour.h"

pthread_mutex_t net_stat_lock;
pthread_mutex_t elev_info_lock;
pthread_mutex_t door_open_lock;
int lamp_matrix[N_FLOORS][2];


int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  int i;
  pthread_t button_input, carry_out_orders;
  printf("Single elevator mode \n");

  this_elevator->is_busy = 0;
  this_elevator->current_floor = return_current_floor();
  for (i = 1; i <10; i++) {
    this_elevator->desired_floor[i] = 1;
  }

  pthread_create(&carry_out_orders, NULL, thread_carry_out_orders_single_elevator_mode, (void* ) this_elevator);
  pthread_create(&button_input, NULL, thread_listen_for_button_input_single_elevator_mode, (void* ) this_elevator);

  while(1) {
    this_elevator->current_floor = return_current_floor();

    if((*server_socket = initialize_client_socket(server_ip)) != 2) {
      pthread_cancel(button_input);
      pthread_cancel(carry_out_orders);
      this_elevator->server_socket = *server_socket;
      //sende info til server
      return 0;
    }
  }
  pthread_join(carry_out_orders, NULL);
  pthread_join(button_input, NULL);
}

void network_elevator_mode(Elev_info *this_elevator, char const *server_ip) {
  this_elevator->is_connected_to_network = 1;
  pthread_t button_input, main_client, message_to_master;
  int server_socket = this_elevator->server_socket;


   this_elevator->desired_floor[0] = 1;


  pthread_create(&button_input, NULL, thread_listen_for_button_input_and_send_to_master, (void*) this_elevator);
  pthread_create(&main_client, NULL, thread_recieve_orders_and_operate_elevator, (void*) this_elevator);
  pthread_create(&message_to_master, NULL, thread_send_to_master, (void*) this_elevator);

  pthread_join(main_client, NULL);
  pthread_cancel(button_input);
  pthread_cancel(message_to_master);

  this_elevator->server_socket = 0;
  this_elevator->is_connected_to_network = 0;

  single_elevator_mode(this_elevator,&server_socket, server_ip);

  }

int main_client(char const *server_ip) {
  int server_socket;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));
  initialize_lamp_matrix(lamp_matrix);
  update_panel_lights(lamp_matrix);

  pthread_mutex_init(&elev_info_lock, NULL);
  pthread_mutex_init(&door_open_lock, NULL);


  initialize_hardware();

  this_elevator->current_floor = return_current_floor();

  this_elevator->is_busy = 0;
  this_elevator->is_connected_to_network = 0;


  while(1) {
    update_panel_lights(lamp_matrix);
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

        network_elevator_mode(this_elevator, server_ip);

      }
    }

  }

void* thread_recieve_orders_and_operate_elevator(void *this_elevator) {
  Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
  int message_type, b, c, new_desired_floor, light_status;
  int initial_message;
  char buffer[512];
  int length = sizeof(buffer);
  pthread_t carry_out_orders;
  memset(buffer, 0, sizeof(buffer));


  if(recv(my_this_elevator->server_socket, &initial_message, sizeof(initial_message),0) == 0) {
    printf("Disconnected from server\n");
  }

  my_this_elevator->num = initial_message;
  printf("My number is %d", initial_message);

  while(1) {

    if (listen_for_message_from_master(buffer, my_this_elevator->server_socket, sizeof(buffer)) == -1){
      printf("Disconnected from master\n");
      return NULL;
    } else {
      printf("*****************************\n");
      printf("Message from master: %s\n", buffer);
      printf("*****************************\n\n");

      unpack_message_to_variables(buffer, &message_type, &b, &c, &new_desired_floor, &light_status);
      memset(buffer, 0, sizeof(buffer));

      if(new_desired_floor != my_this_elevator->desired_floor[0]){
        my_this_elevator->desired_floor[0] = new_desired_floor;
        pthread_mutex_lock(&door_open_lock);
        printf("Operator locked mutex\n");

        if((message_type = pthread_cancel(carry_out_orders)) != 0){
          printf("couldnt close thread: %d\n", message_type);
        }
        pthread_mutex_unlock(&door_open_lock);
        printf("Operator unlocked mutex\n");

        pthread_create(&carry_out_orders, NULL, thread_carry_out_orders_network_mode, (void*) this_elevator);
        printf("Going to floor %d\n", my_this_elevator->desired_floor[0]);
      }
      if(message_type == 3) {
        initialize_lamp_matrix(lamp_matrix);
        unbundle_lamp_matrix(buffer, lamp_matrix);
        update_panel_lights(lamp_matrix);
      }
    }
  }
}
//Gi main client og main server bedre navn
void* thread_carry_out_orders_single_elevator_mode(void *this_elevator) {

    int i;
    Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

    while(1) {
      if(cast_this_elevator->is_busy == 0) {
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

void* thread_carry_out_orders_network_mode(void *this_elevator){

  Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

    if (cast_this_elevator->desired_floor[0] != cast_this_elevator->current_floor){

      go_to_floor(cast_this_elevator->desired_floor[0]);
      pthread_mutex_lock(&door_open_lock);
      printf("holder døren åpen\n");
      hold_doors_open(1);
      printf("kom ut\n");
      pthread_mutex_unlock(&door_open_lock);
      printf("lukker døren\n");


      }
      return NULL;

}

void* thread_listen_for_button_input_single_elevator_mode(void *this_elevator) {
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

void* thread_listen_for_button_input_and_send_to_master(void *this_elevator) {

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

      for(floor=0; floor<4; floor++){
        if (elev_get_button_signal(2, floor) == 1){

            stop_t2 = clock();

            dt2 = (double)(stop_t2 - start_t2)/CLOCKS_PER_SEC;

            if(dt2 > 0.5){
              start_t2 = clock();

              sprintf(messageToMaster, "<2E%dBT2F%d>", cast_this_elevator->num, (floor +1));
            	printf("SEND to master: %s\n", messageToMaster);


              val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
              printf("Bytes sent: %d\n", val);
              memset(messageToMaster, 0, sizeof(messageToMaster));

        }
      }

        if (elev_get_button_signal(1, floor) == 1){

          stop_t1 = clock();

          dt1 = (double)(stop_t1 - start_t1)/CLOCKS_PER_SEC;

          if(dt1 > 0.5){
            start_t1 = clock();

            sprintf(messageToMaster, "<2E%dBT1F%d>", cast_this_elevator->num, (floor +1));
            printf("SEND to master: %s\n", messageToMaster);

            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
            printf("Bytes sent: %d\n", val);
            update_lamp_matrix(lamp_matrix, floor, 1,1);

            memset(messageToMaster, 0, sizeof(messageToMaster));
            bundle_lamp_matrix(messageToMaster, lamp_matrix);
            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster),0);
            printf("Lamp matrix sent to master. Bytes = %d\n", val);
            memset(messageToMaster, 0, sizeof(messageToMaster));

        }
      }
        if (elev_get_button_signal(0, floor) == 1){

          stop_t0 = clock();

          dt0 = (double)(stop_t0 - start_t0)/CLOCKS_PER_SEC;

          if(dt0 > 0.5){
            start_t0 = clock();

            sprintf(messageToMaster, "<2E%dBT0F%d>", cast_this_elevator->num, (floor +1));
            printf("SEND to master: %s\n", messageToMaster);
            update_lamp_matrix(lamp_matrix, floor, 0,1);

            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster), 0);
            printf("Bytes sent: %d\n", val);
            memset(messageToMaster, 0, sizeof(messageToMaster));
            bundle_lamp_matrix(messageToMaster, lamp_matrix);
            val = send(cast_this_elevator->server_socket, messageToMaster, sizeof(messageToMaster),0);
            printf("Lamp matrix sent to master. Bytes = %d\n", val);
            memset(messageToMaster, 0, sizeof(messageToMaster));


        }
       }
     }
    }

    return NULL;
  }

void* thread_send_to_master(void *this_elevator){
  Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

  char message[512];
  int New_floor;

  memset(message, 0, sizeof(message));
  while(1){
      if(cast_this_elevator->is_connected_to_network == 1) {

        New_floor = return_current_floor();

        if(cast_this_elevator->current_floor != New_floor && New_floor != -1){
          memset(message, 0, sizeof(message));

          cast_this_elevator->current_floor = New_floor;

          printf("Elev is now on floor: %d, going to: %d\n", cast_this_elevator->current_floor, cast_this_elevator->desired_floor[0] );
          printf("Elev num is: %d\n", cast_this_elevator->num );

          sprintf(message, "<1E%dF%d>", cast_this_elevator->num, cast_this_elevator->current_floor);

          send(cast_this_elevator->server_socket, message, sizeof(message), 0);

          // send_all(cast_this_elevator->server_socket, message, sizeof(message));

        }

        }
      }
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

  char buffer[512];
  char return_message[512];
  char *temp;
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

    activity = select(max_sd +1, &readfds, NULL, NULL, NULL);

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
        if((valread = read(sd, &buffer, sizeof(buffer))) == 0) {
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

          temp = act_on_message_from_master(Data_elevators, buffer, Data_elevators[99].length_of_elevator_array);

          strcpy(return_message, temp);


          if (strcmp(return_message, "2") == 0) {

            printf("Order has been added to queue\n");

          }
          else if (strcmp(return_message, "0") == 0){

            printf("Den var null gitt\n");

          }
          else if (strchr(return_message, '<') != 0){

            printf("her sender jeg: %s \n", return_message);
            send(sd, return_message, sizeof(return_message), 0);
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

void* thread_send_orders_to_idle_elevators(void* elevatorInfo){
    printf("Entering send to elevators thread\n");
    Elevator_data *myElevatorInfo = (Elevator_data *)elevatorInfo;
    char messageToElevator[512];
    int i;

    while(1){

      for( i = 0; i <= myElevatorInfo[99].length_of_elevator_array; i++ ){

        if (myElevatorInfo[i].status == 2 && myElevatorInfo[i].queue[0] != 0){

          printf("First pos in queue is: %d: \n", myElevatorInfo[i].queue[0]);

          sprintf(messageToElevator, "<1E%dF%d>", i, myElevatorInfo[i].queue[0]);

          printf("Sending msg to elev %d: %s, on socket %d\n", i, messageToElevator, myElevatorInfo[i].socket);

          send(myElevatorInfo[i].socket, messageToElevator, sizeof(messageToElevator), 0);

          update_elevator_struct(myElevatorInfo[i].queue, &myElevatorInfo[i].status, &myElevatorInfo[i].queue_size, myElevatorInfo[i].current_floor);

        }
      }
    }

    return NULL;
  }
