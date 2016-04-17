#include "client_functions.h"

int main_client(char const *server_ip) {
        int server_socket, initial_floor = 1;
        Elevator_client_information *this_elevator = malloc(sizeof(Elevator_client_information));
        pthread_mutex_init(&elevator_client_information_lock, NULL);
        pthread_mutex_init(&send_message_lock, NULL);
        pthread_mutex_init(&doors_open_lock, NULL);

        initialize_lamp_matrix(lamp_matrix);
        initialize_hardware();
        update_panel_lights(lamp_matrix);
        go_to_floor(&initial_floor);

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

int single_elevator_mode(Elevator_client_information *this_elevator, int *server_socket, char const *server_ip) {
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

        Elevator_client_information* cast_this_elevator = ((Elevator_client_information *) this_elevator);

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
                                        pthread_mutex_lock(&elevator_client_information_lock);

                                        for (i = 0; i < 10; i++) {
                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }
                                        pthread_mutex_unlock(&elevator_client_information_lock);
                                        sleep(1);
                                }
                        }
                        if (elev_get_button_signal(1, floor) == 1) {
                                if(cast_this_elevator->desired_floor[0] != floor+1) {

                                        update_lamp_matrix(lamp_matrix, floor, 1,1);
                                        pthread_mutex_lock(&elevator_client_information_lock);
                                        for (i = 0; i < 10; i++) {

                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }
                                        pthread_mutex_unlock(&elevator_client_information_lock);
                                        sleep(1);
                                }
                        }
                        if (elev_get_button_signal(0, floor) == 1) {
                                if(cast_this_elevator->desired_floor[0] != floor+1) {
                                        update_lamp_matrix(lamp_matrix, floor, 0,1);
                                        pthread_mutex_lock(&elevator_client_information_lock);

                                        for (i = 0; i < 10; i++) {
                                                if(cast_this_elevator->desired_floor[i] == 0) {
                                                        cast_this_elevator->desired_floor[i] = floor+1;
                                                        break;
                                                }
                                        }

                                        pthread_mutex_unlock(&elevator_client_information_lock);
                                        sleep(1);
                                }

                        }
                }
        }
}

void* thread_single_elevator_carry_out_orders(void *this_elevator) {

        int first_position_in_array = 0;
        Elevator_client_information *cast_this_elevator = (Elevator_client_information *) this_elevator;

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

void network_elevator_mode(Elevator_client_information *this_elevator, char const *server_ip) {
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
        Elevator_client_information* my_this_elevator = ((Elevator_client_information *) this_elevator);
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

        my_this_elevator->current_floor = return_current_floor();
        sprintf(message, "<1E%dF%d>", my_this_elevator->num, my_this_elevator->current_floor);
        send(my_this_elevator->server_socket, message, MAX_MESSAGE_SIZE, 0);
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
                                        printf("new desired floor %d\n", new_desired_floor);

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
        Elevator_client_information *my_this_elevator = (Elevator_client_information *) this_elevator;
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

        Elevator_client_information* cast_this_elevator = ((Elevator_client_information *) this_elevator);
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
