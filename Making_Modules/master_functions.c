#include "master_functions.h"

pthread_mutex_t net_stat_lock;

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

        Network_status *my_net_status = (Network_status *) net_status;
        pthread_t send_order_to_elevator;
        struct sockaddr_in address;
        int max_sd, sd, activity, new_socket, new_queue_number;
        int valread, elev_counter;
        fd_set readfds;
        int addrlen = sizeof address;
        int message_type, elevator_id, current_floor, button_type, button_floor, queue_message;
        char buffer[MAX_MESSAGE_SIZE];
        Elevator_master_information Data_elevators[MAX_NUMBER_OF_ELEVATORS];
        struct timeval timeout;
        struct timespec start[MAX_NUMBER_OF_ELEVS][2];

        initiate_queues(Data_elevators);

        pthread_create(&send_order_to_elevator, NULL, thread_send_orders_to_idle_elevators, Data_elevators);

        if(listen(my_net_status->master_socket, MAX_NUMBER_OF_ELEVS) < 0) {
                perror("listen");
                exit(1);
        }

        while(1) {

          FD_ZERO(&readfds);
          timeout.tv_sec = 20;
          timeout.tv_usec = 5;
          max_sd = add_all_socks_to_fdlist(&readfds, my_net_status);

                for(elev_counter = 0; elev_counter < MAX_NUMBER_OF_ELEVS; elev_counter++) {
                        if(my_net_status->client_sockets[elev_counter] != 0) {
                                clock_gettime(CLOCK_MONOTONIC, &start[elev_counter][1]);

                                if(is_client_timed_out(start[elev_counter][1], start[elev_counter][0]) == 1) {

                                        getpeername(my_net_status->client_sockets[elev_counter], (struct sockaddr*)&address, (socklen_t*)&addrlen);
                                        printf("Client timeout , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                                        disable_elevator_and_distribute_queue_to_other_elevators(Data_elevators, my_net_status->client_sockets[elev_counter]);
                                        close(my_net_status->client_sockets[elev_counter]);
                                        FD_CLR(my_net_status->client_sockets[elev_counter], &readfds);
                                        my_net_status->client_sockets[elev_counter] = 0;
                                        my_net_status->active_connections -= 1;
                                        printf("Timeout socket\n");
                                }
                        }
                }

                if((activity = select(max_sd +1, &readfds, NULL, NULL, &timeout)) == 0) {
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
                                        Data_elevators[99].length_of_elevator_array++;
                                        Data_elevators[new_queue_number].socket = new_socket;
                                        send(Data_elevators[new_queue_number].socket, &new_queue_number, sizeof(new_queue_number), 0);
                                        for(elev_counter = 0; elev_counter < MAX_NUMBER_OF_ELEVS; elev_counter++) {
                                                if(my_net_status->client_sockets[elev_counter] == new_socket) {
                                                        clock_gettime(CLOCK_MONOTONIC, &start[elev_counter][0]);
                                                        break;
                                                }
                                        }
                                }
                        }
                        for(elev_counter = 0; elev_counter < MAX_NUMBER_OF_ELEVS; elev_counter++) {
                                sd = my_net_status->client_sockets[elev_counter];

                                if(FD_ISSET(sd, &readfds)) {
                                        if((valread = recv(sd, &buffer, sizeof(buffer),0)) <= 0) {

                                                disable_elevator_and_distribute_queue_to_other_elevators(Data_elevators, sd);

                                                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                                                printf("Client disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                                                close(sd);
                                                FD_CLR(sd, &readfds);
                                                my_net_status->client_sockets[elev_counter] = 0;
                                                my_net_status->active_connections -= 1;
                                        }
                                        else {
                                                printf("\n\nMottatt meldingen %s på socket %d\n",buffer, sd );
                                                message_type = buffer[1] - '0';
                                                clock_gettime(CLOCK_MONOTONIC, &start[elev_counter][0]);
                                                printf("Messagetype is: %d\n", message_type);

                                                if (message_type == 1) {
                                                        unpack_current_floor_message(buffer, &elevator_id, &current_floor);
                                                        queue_format_to_floor_and_button(Data_elevators[elevator_id].queue[0], &button_floor, &button_type);

                                                        if (button_floor == current_floor) {
                                                                update_lamp_matrix(lamp_matrix, current_floor-1, button_type,0);
                                                        }

                                                        update_elevinfo_for_message_type_1(Data_elevators, elevator_id, current_floor);

                                                        if (Data_elevators[elevator_id].queue[0] == 0) {

                                                                Data_elevators[elevator_id].status = 2;
                                                                printf("The queue is empty\n update status: 2\n");
                                                                break;
                                                        }
                                                        create_and_send_message(Data_elevators[elevator_id], elevator_id);
                                                        update_elevator_status_and_queuesize(Data_elevators[elevator_id].queue, &Data_elevators[elevator_id].status, &Data_elevators[elevator_id].queue_size, Data_elevators[elevator_id].current_floor);

                                                }
                                                else if (message_type == 2) {
                                                        unpack_button_click_message(buffer, &elevator_id, &button_type, &button_floor, &queue_message);
                                                        add_new_order_to_queue(Data_elevators, queue_message, button_type, elevator_id, Data_elevators[99].length_of_elevator_array);
                                                        if(button_type != 2) {
                                                                update_lamp_matrix(lamp_matrix, button_floor-1, button_type, 1);
                                                        }
                                                }
                                                for(int i = 0; i < Data_elevators[99].length_of_elevator_array; i++) {
                                                        if (Data_elevators[i].status != -1) {
                                                                printf("\n\n******************************************\n");
                                                                printf("Queue number: %d\t", i);
                                                                printf("Status: %d\n", Data_elevators[i].status);
                                                                printf("Current_floor: %d\n", Data_elevators[i].current_floor);
                                                                print_queue(Data_elevators[i].queue);
                                                                printf("******************************************\n\n");
                                                        }
                                                }
                                                memset(buffer, 0, sizeof(buffer));
                                        }
                                }
                        }
                }
        }
}

void* thread_send_orders_to_idle_elevators(void* elevator_info){
        printf("Entering send to elevators thread\n");
        Elevator_master_information *my_elevator_info = (Elevator_master_information *)elevator_info;
        char message_to_elevator[MAX_MESSAGE_SIZE];
        int i;

        while(1) {
                sleep(1);
                for( i = 0; i <= my_elevator_info[99].length_of_elevator_array; i++ ) {

                        if (my_elevator_info[i].status == 2 && my_elevator_info[i].queue[0] != 0) {


                                printf("elevators currentfloor: %d\n", my_elevator_info[i].current_floor);
                                printf("elevators next queue: %d\n\n", my_elevator_info[i].queue[0]);


                                int queue_instance = my_elevator_info[i].queue[0];
                                int floor_in_queue = 0;

                                while(queue_instance >= 10) {
                                        floor_in_queue++;
                                        queue_instance -= 10;
                                }
                                printf("Elevator current floor %d\n", my_elevator_info[i].current_floor);
                                printf("queue floor %d\n", floor_in_queue);

                                if (my_elevator_info[i].current_floor > floor_in_queue) {

                                        my_elevator_info[i].status = GOING_DOWN;
                                }
                                else if (my_elevator_info[i].current_floor < floor_in_queue) {
                                        my_elevator_info[i].status = GOING_UP;
                                }
                                printf("elevators status: %d\n\n", my_elevator_info[i].status);
                                create_and_send_message(my_elevator_info[i], i);
                        }
                        bundle_lamp_matrix(message_to_elevator,lamp_matrix);
                        send(my_elevator_info[i].socket, message_to_elevator, sizeof(message_to_elevator),0);
                        memset(message_to_elevator, 0, sizeof(message_to_elevator));
                }
        }
        return NULL;
}

void create_and_send_message(Elevator_master_information Data_elevators, int elevator_id){

        int new_desired_floor = 0;
        int temp_queue_instance = Data_elevators.queue[0];
        char return_message[MAX_MESSAGE_SIZE];

        while(temp_queue_instance >= 10) {
                new_desired_floor++;
                temp_queue_instance -= 10;
        }

        sprintf(return_message, "<1E%dF%d>", elevator_id, new_desired_floor);
        send(Data_elevators.socket, return_message, sizeof(return_message), 0);


}

void update_elevinfo_for_message_type_1(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int msg_elevator_number, int msg_elevator_floor){

        int is_elevator_on_correct_floor;

        E[msg_elevator_number].current_floor = msg_elevator_floor;

        int temp_queue_instance = E[msg_elevator_number].queue[0];
        int floor_in_queue = 0;

        while(temp_queue_instance >= 10) {
                floor_in_queue++;
                temp_queue_instance -= 10;
        }

        is_elevator_on_correct_floor = (floor_in_queue == msg_elevator_floor);

        if (is_elevator_on_correct_floor == 1) {
                int first_item_in_queue = 0;
                remove_item_from_queue(E[msg_elevator_number].queue, first_item_in_queue);
        }
}
