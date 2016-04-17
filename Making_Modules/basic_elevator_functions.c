#include "basic_elevator_functions.h"
#include "elev.h"

int initialize_hardware() {
        elev_init();
        return 0;
}

int go_up() {
        elev_set_motor_direction(DIRN_UP);
        return 1;

}

int go_down() {
        elev_set_motor_direction(DIRN_DOWN);
        return 1;
}

int stop_elevator() {
        elev_set_motor_direction(DIRN_STOP);
        return 1;
}

int go_to_floor(int* desired_floor) {
        int current_floor;
        time_t start_time, current_time;
        double dt;

        start_time = clock();


        while(1) {
                current_time = clock();
                dt = (current_time - start_time)/(CLOCKS_PER_SEC*2);
                printf("Desired_floro: %d\n", *desired_floor);

                if(dt > N_FLOORS*5) {
                        return -1;
                }

                if((current_floor = return_current_floor()) != -1) {
                        if(current_floor < *desired_floor) {
                                elev_set_motor_direction(DIRN_UP);
                        }
                        if(current_floor > *desired_floor) {
                                elev_set_motor_direction(DIRN_DOWN);
                        }
                        if(current_floor == *desired_floor) {
                                elev_set_motor_direction(DIRN_STOP);
                                return 1;
                        }
                }
        }
}

int hold_doors_open(int duration) {
        int floor;


        if((floor = elev_get_floor_sensor_signal()) == -1) {
                //fprintf(stderr, "Elevator between floors\n");
                return -1;
        }

        elev_set_door_open_lamp(1);
        sleep(duration);
        elev_set_door_open_lamp(0);
        return 1;
}

int return_current_floor() {
        int current_floor;
        current_floor = elev_get_floor_sensor_signal();
        return current_floor;
}

int return_button_input(Button_click *button_order) {
        int floor;

        while(1) {
                for(floor = 0; floor < N_FLOORS; floor++) {
                        if(elev_get_button_signal(2, floor) == 1) {
                                button_order->button_type = 2;
                                button_order->button_floor = (floor +1);
                                return 0;
                        }
                        if(elev_get_button_signal(1,floor) == 1) {
                                button_order->button_type= 1;
                                button_order->button_floor = (floor +1);
                                return 0;
                        }
                        if(elev_get_button_signal(0,floor) == 1) {
                                button_order->button_type = 0;
                                button_order->button_floor = (floor +1);
                                return 0;
                        }
                }
        }
}
