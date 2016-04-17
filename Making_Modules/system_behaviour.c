#include "system_behaviour.h"
#include "panel_lights.h"
#include "message_handling.h"


pthread_mutex_t net_stat_lock;
pthread_mutex_t elev_info_lock;
pthread_mutex_t send_message_lock;
pthread_mutex_t doors_open_lock;
int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS];

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
        int queue_index,current_floor;
        pthread_t button_input, carry_out_orders, lights;

        this_elevator->is_busy = 0;
        this_elevator->current_floor = return_current_floor();
        for (queue_index = 1; queue_index <10; queue_index++) {
                this_elevator->desired_floor[queue_index] = 0;
        }

        convert_lamp_matrix_to_orders(lamp_matrix, this_elevator->desired_floor);

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
                        return 0;
                }
        }
}

void* thread_single_elevator_button_input(void *this_elevator) {

        Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

        int floor;
        int i,j;

        while(1) {
                for(floor=0; floor<4; floor++) {

                        if (elev_get_button_signal(2, floor) == 1) {
                                for(j = 0; j < 10; j++) {
                                        printf("%d,", cast_this_elevator->desired_floor[j]);
                                }
                                printf("\n");
                                if(cast_this_elevator->desired_floor[0] != floor+1) {
                                        inner_button_light_switch(floor,1);
                                        pthread_mutex_lock(&elev_info_lock);

                                        for (i = 0; i < 10; i++) {
                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }
                                        pthread_mutex_unlock(&elev_info_lock);
                                        sleep(1);
                                }
                        }
                        if (elev_get_button_signal(1, floor) == 1) {
                                if(cast_this_elevator->desired_floor[0] != floor+1) {

                                        update_lamp_matrix(lamp_matrix, floor, 1,1);
                                        pthread_mutex_lock(&elev_info_lock);
                                        for (i = 0; i < 10; i++) {

                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }
                                        pthread_mutex_unlock(&elev_info_lock);
                                        sleep(1);
                                }
                        }
                        if (elev_get_button_signal(0, floor) == 1) {
                                if(cast_this_elevator->desired_floor[0] != floor+1) {
                                        update_lamp_matrix(lamp_matrix, floor, 0,1);
                                        pthread_mutex_lock(&elev_info_lock);

                                        for (i = 0; i < 10; i++) {
                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }

                                        pthread_mutex_unlock(&elev_info_lock);
                                        sleep(1);
                                }

                        }
                }
        }
}

void* thread_single_elevator_carry_out_orders(void *this_elevator) {

        int first_position_in_array = 0;
        Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

        cast_this_elevator->is_busy = 0;

        while(1) {
                if(cast_this_elevator->is_busy == 0) {
                        cast_this_elevator->is_busy = 1;
                        if(cast_this_elevator->desired_floor[0] != 0) {
                                go_to_floor(&cast_this_elevator->desired_floor[0]);
                                inner_button_light_switch(cast_this_elevator->desired_floor[0]-1, OFF);
                                update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[0]-1,BUTTON_CALL_UP, OFF);
                                update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[0]-1,BUTTON_CALL_DOWN, OFF);
                        }
                        if(cast_this_elevator->desired_floor[0] == cast_this_elevator->current_floor) {
                                hold_doors_open(DOORS_OPEN_DURATION);
                                inner_button_light_switch(cast_this_elevator->desired_floor[0]-1, OFF);
                                update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[0]-1,BUTTON_CALL_UP, OFF);
                                update_lamp_matrix(lamp_matrix, cast_this_elevator->desired_floor[0]-1,BUTTON_CALL_DOWN, OFF);
                                remove_item_from_queue(cast_this_elevator->desired_floor, first_position_in_array);
                        }
                }
                cast_this_elevator->is_busy = 0;
        }
        return NULL;
}

void network_elevator_mode(Elev_info *this_elevator, char const *server_ip) {
        this_elevator->is_connected_to_network = 1;
        pthread_t button_input, main_client, orders, lights;
        int server_socket = this_elevator->server_socket;

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
        close(this_elevator->server_socket);

        single_elevator_mode(this_elevator,&server_socket, server_ip);
}

void* recieve_messages_from_server(void* this_elevator) {
        Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
        char message[MAX_MESSAGE_SIZE];
        int elevator_id, new_desired_floor = -1;
        clock_t start, stop;
        double dt;

        if(recv(my_this_elevator->server_socket, &elevator_id, sizeof(int),0) == 0) {
                printf("Disconnected from server\n");
        }

        my_this_elevator->num = elevator_id;
        printf("My number is %d\n\n", elevator_id);
        memset(message,0,sizeof(message));

        start = clock();

        while(1) {
                stop = clock();
                dt = (double)(stop - start)/(CLOCKS_PER_SEC*2);
                if(dt > 10) {
                        sprintf(message, "<5E%d>", my_this_elevator->num);
                        send(my_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                        memset(message,0,MAX_MESSAGE_SIZE);
                        start = clock();

                }

                if (listen_for_message_from_master(message, my_this_elevator->server_socket, MAX_MESSAGE_SIZE) == -1) {
                        printf("Disconnected from master\n");
                        close(my_this_elevator->server_socket);
                        my_this_elevator->is_connected_to_network = 0;
                        return NULL;
                } else {
                        if((message[0] - '0')  == 3) {
                                initialize_lamp_matrix(lamp_matrix);
                                unbundle_lamp_matrix(message, lamp_matrix);
                                memset(message, 0, sizeof(message));
                        } else if((message[1] - '0') == 1) {
                                unpack_current_floor_message(message, &elevator_id, &new_desired_floor);

                                if(new_desired_floor != my_this_elevator->desired_floor[0]) {
                                        my_this_elevator->desired_floor[0] = new_desired_floor;

                                } else if(new_desired_floor == my_this_elevator->current_floor) {

                                        hold_doors_open(DOORS_OPEN_DURATION);
                                        inner_button_light_switch(new_desired_floor-1,0);

                                        memset(message,0,MAX_MESSAGE_SIZE);
                                        sprintf(message, "<1E%dF%d>", my_this_elevator->num, my_this_elevator->current_floor);

                                        send(my_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                                        memset(message,0,MAX_MESSAGE_SIZE);
                                        new_desired_floor = -1;
                                }
                                memset(message,0,MAX_MESSAGE_SIZE);
                        }
                }
        }
}

void* thread_carry_out_orders(void* this_elevator) {
        Elev_info *my_this_elevator = (Elev_info *) this_elevator;
        char message[MAX_MESSAGE_SIZE];

        while(1) {
                if((my_this_elevator->desired_floor[0] != my_this_elevator->current_floor) && (my_this_elevator->desired_floor[0] != 0)) {
                        if(go_to_floor(&my_this_elevator->desired_floor[0]) == -1) {
                                stop_elevator();
                                printf("Hardware error, shutting down\n");
                                exit(1);
                        }
                        inner_button_light_switch(my_this_elevator->desired_floor[0]-1,0);

                        pthread_mutex_lock(&doors_open_lock);
                        hold_doors_open(DOORS_OPEN_DURATION);
                        pthread_mutex_unlock(&doors_open_lock);

                        my_this_elevator->current_floor = return_current_floor();

                        sprintf(message, "<1E%dF%d>", my_this_elevator->num, my_this_elevator->current_floor);
                        send(my_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                }
        }
}

void* thread_network_listen_for_button(void *this_elevator) {

        Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);
        int floor, bytes_sent;
        char message[MAX_MESSAGE_SIZE];
        clock_t start_t0,start_t1, start_t2, stop_t0, stop_t1, stop_t2;
        double dt0, dt1, dt2;

        start_t0 = clock();
        start_t1 = clock();
        start_t2 = clock();

        while(1) {

                for(floor=1; floor <= 4; floor++) {
                        if (elev_get_button_signal(2, floor-1) == 1) {

                                stop_t2 = clock();
                                dt2 = (double)(stop_t2 - start_t2)/CLOCKS_PER_SEC;

                                if(dt2 > 0.5) {
                                        start_t2 = clock();

                                        sprintf(message, "<2E%dM%d>", cast_this_elevator->num, (floor*10 + 2));
                                        printf("SEND to master: %s\n", message);

                                        bytes_sent = send(cast_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                                        printf("Bytes sent: %d\n", bytes_sent);
                                        memset(message, 0, MAX_MESSAGE_SIZE);
                                        inner_button_light_switch(floor-1, 1);
                                }
                        }

                        if (elev_get_button_signal(1, floor-1) == 1) {

                                stop_t1 = clock();

                                dt1 = (double)(stop_t1 - start_t1)/CLOCKS_PER_SEC;

                                if(dt1 > 0.5) {
                                        start_t1 = clock();

                                        sprintf(message, "<2E%dM%d>", cast_this_elevator->num, (floor*10 + 1));
                                        printf("SEND to master: %s\n", message);

                                        bytes_sent = send(cast_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                                        printf("Bytes sent: %d\n", bytes_sent);
                                        memset(message, 0, MAX_MESSAGE_SIZE);

                                }
                        }
                        if (elev_get_button_signal(0, floor-1) == 1) {

                                stop_t0 = clock();

                                dt0 = (double)(stop_t0 - start_t0)/CLOCKS_PER_SEC;

                                if(dt0 > 0.5) {
                                        start_t0 = clock();

                                        sprintf(message, "<2E%dM%d>", cast_this_elevator->num, (floor*10));
                                        printf("SEND to master: %s\n", message);

                                        bytes_sent = send(cast_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
                                        printf("Bytes sent: %d\n", bytes_sent);
                                        memset(message, 0, MAX_MESSAGE_SIZE);
                                }
                        }
                }
        }
        free(message);
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

        Network_status *my_net_status = (Network_status *) net_status;
        pthread_t send_order_to_elevator;
        struct sockaddr_in address;
        int max_sd, sd, activity, new_socket, new_queue_number;
        int valread, elev_counter;
        fd_set readfds;
        int addrlen = sizeof address;
        int message_type, elevator_id, current_floor, button_type, button_floor, queue_message;
        char buffer[MAX_MESSAGE_SIZE];
        Elevator_data Data_elevators[MAX_NUMBER_OF_ELEVATORS];
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
        Elevator_data *my_elevator_info = (Elevator_data *)elevator_info;
        char message_to_elevator[MAX_MESSAGE_SIZE];
        int i;

        while(1) {
                sleep(1);
                for( i = 0; i <= my_elevator_info[99].length_of_elevator_array; i++ ) {

                        if (my_elevator_info[i].status == 2 && my_elevator_info[i].queue[0] != 0) {

                                create_and_send_message(my_elevator_info[i], i);
                                printf("elevators currentfloor: %d\n", my_elevator_info[i].current_floor);
                                printf("elevators next queue: %d\n\n", my_elevator_info[i].queue[0]);


                                int queue_instance = my_elevator_info[i].queue[0];
                                int floor_in_queue = 0;

                                while(queue_instance >= 10) {
                                        floor_in_queue++;
                                        queue_instance -= 10;
                                }

                                if (my_elevator_info[i].current_floor > floor_in_queue) {

                                        my_elevator_info[i].status = 1;
                                }
                                else if (my_elevator_info[i].current_floor < floor_in_queue) {
                                        my_elevator_info[i].status = 0;
                                }

                        }
                        bundle_lamp_matrix(message_to_elevator,lamp_matrix);
                        send(my_elevator_info[i].socket, message_to_elevator, sizeof(message_to_elevator),0);
                        memset(message_to_elevator, 0, sizeof(message_to_elevator));
                }
        }
        return NULL;
}

void create_and_send_message(Elevator_data Data_elevators, int elevator_id){

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

        if (is_elevator_on_correct_floor == 1) {
                int first_item_in_queue = 0;
                remove_item_from_queue(E[msg_elevator_number].queue, first_item_in_queue);
        }
}
