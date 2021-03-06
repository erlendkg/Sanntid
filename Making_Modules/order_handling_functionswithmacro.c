#include "order_handling_functions.h"

void place_order_on_the_way(int order_queue[QUEUE_SIZE], int * status, int desired_floor, int button_order){

        int i = 0;

        if (*status == GOING_UP ) {

                while(1) {

                        if (desired_floor == order_queue[i]) {
                                break;//Order already exists, therefore nothing is done
                        }
                        else if ((desired_floor < order_queue[i]) || (i > 0 && order_queue[i] < order_queue[i-1])) { //the latter secures that data is stored if desired_floor is larger than the largest value in the queue.
                                insert_item(order_queue, i, button_order);
                                *status = 0;
                                break;//The new order is added in the desired positon.
                        }
                        else if (desired_floor > order_queue[i]) {
                                i++;
                        }

                }
        }
        else if (*status == GOING_DOWN) {

                while(1) {

                        if (button_order == order_queue[i]) {
                                break;//Order already exists, therefore nothing is done
                        }
                        else if ((button_order > order_queue[i])||(i > 0 && order_queue[i] > order_queue[i-1])) {
                                insert_item(order_queue, i, button_order);
                                *status = 1;
                                break;//The new order is added in the desired positon.
                        }
                        else if (button_order < order_queue[i]) {
                                i++;
                        }

                }
        }
}

void place_order_not_on_the_way(int order_queue[QUEUE_SIZE], int * status, int desired_floor, int button_order){

        int i = QUEUE_SIZE-1;
        printf("is not on the way\n");
        if (*status == GOING_UP ) {

                while(1) {

                        if (i == 0) {

                                insert_item(order_queue, 0, button_order);
                                break;
                        }
                        else if (desired_floor > order_queue[i]) {
                                i--;
                        }
                        else if (desired_floor == order_queue[i]) {
                                break;//Order already exists, therefore nothing is done
                        }
                        else if (order_queue[i] != 0) {
                                insert_item(order_queue, i+1, button_order);
                                *status = GOING_UP;
                                break;//The new order is added in the desired positon.
                        }

                }
        }
        if (*status == GOING_DOWN) {

                while(1) {

                        if (i == 0) {

                                insert_item(order_queue, 0, button_order);
                                break;
                        }

                        else if (button_order < order_queue[i] || order_queue[i] == 0) {
                                i--;
                        }
                        else if (button_order == order_queue[i]) {
                                break;
                        }
                        else if (order_queue[i] != 0) {
                                insert_item(order_queue, i+1, button_order);
                                *status = GOING_DOWN;
                                break;
                        }
                }
        }
}

void place_bt2_order( Elevator_master_information * E, int button_order){

        int on_way;
        int desired_floor, button_type;


        if (E->status != IDLE) {

                queue_format_to_floor_and_button(button_order, &desired_floor, &button_type);


                on_way = is_order_on_the_way(E->current_floor, E->status, desired_floor);
                if (on_way == 1) {
                        place_order_on_the_way(E->queue, &E->status, desired_floor, button_order);
                }
                else if(on_way == 0) {
                        place_order_not_on_the_way(E->queue, &E->status, desired_floor, button_order);
                }
        }
        else if (E->status == IDLE) {

                insert_item(E->queue, 0, button_order);
                //update_elevator_status_and_queuesize(E->queue, &E->status, &E->queue_size, E->current_floor);
        }
}

void place_bt0_order( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array){

        int closest_elev = -1;
        int distance_from_closest_elev_to_desired_floor = 9999;
        int smallest_queue = 9999;
        int smallest_elev;

        int desired_floor;
        int button_type;

        queue_format_to_floor_and_button(button_order, &desired_floor, &button_type);

        for(int i = 0; i < length_of_elevator_array; i++) {
                //Check if the elevator is disabled, jump over it if it is.
                i = i + is_elevator_disabled(E[i].status);

                //for every iteration, check if the order is on the way up for an elevator
                if(E[i].status == GOING_UP && is_order_on_the_way(E[i].current_floor, E[i].status, desired_floor)) {
                        place_order_on_the_way(E[i].queue, &E[i].status, desired_floor, button_order);
                        break;
                }
                else if(( E[i].status == IDLE )&& ( abs(E[i].current_floor - desired_floor) < distance_from_closest_elev_to_desired_floor )) {
                        distance_from_closest_elev_to_desired_floor = abs(E[i].current_floor - desired_floor);
                        closest_elev = i;
                }

                if(E[i].queue_size < smallest_queue) {

                        smallest_elev = i;
                        smallest_queue = E[i].queue_size;
                }

                //the order was not on the way for any elevators,
                if((i >= length_of_elevator_array - 1) && (closest_elev != -1)) {
                        insert_item(E[closest_elev].queue, 0, button_order);

                }
                else if((i >= length_of_elevator_array - 1) && (closest_elev == -1)) {
                        place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, desired_floor, button_order);
                }
        }
}

void place_bt1_order( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array){

        int closest_elev = -1;
        int distance_from_closest_elev_to_desired_floor = 9999;
        int smallest_queue = 9999;
        int smallest_elev;
        int desired_floor;
        int button_type;

        queue_format_to_floor_and_button(button_order, &desired_floor, &button_type);

        for(int i = 0; i < length_of_elevator_array; i++) {
                //Check if the elevator is disabled, jump over it if it is.
                i = i + is_elevator_disabled(E[i].status);

                //for every iteration, check if the order is on the way up for an elevator
                if(E[i].status == GOING_DOWN && is_order_on_the_way(E[i].current_floor, E[i].status, desired_floor)) {

                        place_order_on_the_way(E[i].queue, &E[i].status, desired_floor, button_order);
                        break;
                }
                else if(( E[i].status == IDLE )&& ( abs(E[i].current_floor - desired_floor) < distance_from_closest_elev_to_desired_floor )) {
                        distance_from_closest_elev_to_desired_floor = abs(E[i].current_floor - desired_floor);
                        closest_elev = i;
                }

                if(E[i].queue_size < smallest_queue) {

                        smallest_elev = i;
                        smallest_queue = E[i].queue_size;
                }

                //the order was not on the way for any elevators,
                if((i == length_of_elevator_array - 1) && (closest_elev != -1)) {
                        insert_item(E[closest_elev].queue, 0, button_order);
                }
                else if((i == length_of_elevator_array - 1) && (closest_elev == -1)) {
                        place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, desired_floor, button_order);
                }
        }
}

int is_order_in_global_queue(Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int desired_floor){

        int elevatorCounter, queueCounter;

        for (elevatorCounter = 0; elevatorCounter < MAX_NUMBER_OF_ELEVATORS; elevatorCounter++) {
                for (queueCounter = 0; queueCounter < QUEUE_SIZE; queueCounter++) {
                        if (desired_floor == E[elevatorCounter].queue[queueCounter]) {
                                return 1;
                        }
                }
        }
        return 0;
}

int is_order_in_local_queue(Elevator_master_information E, int desired_floor){

        int queue_counter;

        for (queue_counter = 0; queue_counter < QUEUE_SIZE; queue_counter++) {
                if (desired_floor == E.queue[queue_counter]) {
                        return 1;
                }
        }
        return 0;
}

void add_new_order_to_queue( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int desired_floor, int button_type, int elevator, int length_of_elevator_array){

        if (button_type == BUTTON_CALL_INTERNAL) {
                if(is_order_in_local_queue(E[elevator], desired_floor) == 1) {

                        return;
                }
                place_bt2_order(&E[elevator], desired_floor);

        }

        if(is_order_in_global_queue(E, desired_floor) == 1) {

                return;
        }

        if (button_type == BUTTON_CALL_UP) {

                place_bt0_order(E, desired_floor, length_of_elevator_array);

        }
        else if (button_type == BUTTON_CALL_DOWN) {

                place_bt1_order(E, desired_floor, length_of_elevator_array);

        }
}

int assign_number_to_new_elevator( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators){

        int i;

        for(i = 0; i<numberOfElevators; i++) {
                if (E[i].status == DISABLED) {
                        return i;
                }
        }
        return numberOfElevators;
}

void activate_single_queue( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int elevatorNumber){

        size_t l = QUEUE_SIZE * sizeof (E[elevatorNumber].queue[0]);
        flush_order_queue(E[elevatorNumber].queue,l);

        E[elevatorNumber].status = IDLE;
        E[elevatorNumber].queue_size = 0;
        E[elevatorNumber].current_floor = 1;

}

void initiate_queues( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS]){

        for(int i = 0; i < MAX_NUMBER_OF_ELEVATORS; i++) {
                size_t l = QUEUE_SIZE * sizeof (E[i].queue[0]);
                flush_order_queue(E[i].queue,l);
        }
}

void disable_elevator_and_distribute_queue_to_other_elevators( Elevator_master_information E[MAX_NUMBER_OF_ELEVATORS], int disconnected_socket){

        pthread_mutex_lock(&lock);

        int queue_counter = 0,elev_counter = 0, crashed_elev;
        int length_of_elevator_array = E[99].length_of_elevator_array;

        while(1) {
                if (E[elev_counter].socket == disconnected_socket) {
                        crashed_elev = elev_counter;
                        break;
                }
                elev_counter++;
        }

        E[crashed_elev].status = DISABLED;

        while (E[crashed_elev].queue[queue_counter] != 0) {

                add_new_order_to_queue(E,E[crashed_elev].queue[queue_counter],0,0,length_of_elevator_array);

                queue_counter++;
        }

        if (crashed_elev == length_of_elevator_array - 1) {
                E[99].length_of_elevator_array--;
        }

        pthread_mutex_unlock(&lock);
}

int is_order_on_the_way(int current_floor, int status, int desired_floor){

        int rel_pos = desired_floor- current_floor;

        if ((status == GOING_UP && rel_pos > 0)||(status == GOING_DOWN && rel_pos < 0)) {
                return 1; //yes order is on the way
        }
        else {
                return 0; //no order is not on the way
        }
}

int is_elevator_disabled(int status){

        if (status == DISABLED) {
                return 1;
        }
        else {return 0; }
}

void update_elevator_status_and_queuesize(int order_queue[QUEUE_SIZE], int * status, int * queue_size, int current_floor){


        int queue_instance = order_queue[0];
        int floor_in_queue = 0;

        while(queue_instance >= 10) {
                floor_in_queue++;
                queue_instance -= 10;
        }


        if (queue_instance == 0) {
                *status = IDLE;
        }
        else if (current_floor > floor_in_queue) {

                *status = GOING_DOWN;
        }
        else if (current_floor < floor_in_queue) {
                *status = GOING_UP;
        }

        for (int i = 0; i < QUEUE_SIZE; i++) {

                if (order_queue[i] == 0) {
                        *queue_size = i;
                        break;
                }
        }
}
