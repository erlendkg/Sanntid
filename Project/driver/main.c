
#include <stdio.h>

#include "elev.h"

#include <unistd.h>
#include<pthread.h>
#include "queue_functions.h"



int main() {
  run_elevator();






  /*

    pthread_t t_listen, t_go_to_floor;
    elev_init();
    elev_button_type_t b;

    E.DesiredFloor = 1;
    E.CurrentFloor = 3;


    pthread_create(&t_listen, NULL, listen_for_button_input, NULL);
    pthread_create(&t_go_to_floor, NULL, elev_go_to_floor, NULL);
    printf("Press STOP button to stop elevator and exit program.\n");

    pthread_join(t_listen, NULL);
    pthread_join(t_go_to_floor, NULL);


*/



/*
    elev_set_motor_direction(DIRN_UP);

    while (1) {
        // Change direction when we reach top/bottom floor
        if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
            elev_set_motor_direction(DIRN_DOWN);
        } else if (elev_get_floor_sensor_signal() == 0) {
            elev_set_motor_direction(DIRN_UP);
        }

        // Stop elevator and exit program if the stop button is pressed
        if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            return 0;
        }
    }
    */
}
