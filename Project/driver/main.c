
#include <stdio.h>

#include "elev.h"

#include <unistd.h>
#include<pthread.h>


int main() {

    pthread_t listen;
    elev_init();
    elev_button_type_t b;

    pthread_create(&listen, NULL, listen_for_button_input, NULL);
    printf("Press STOP button to stop elevator and exit program.\n");

    pthread_join(listen, NULL);






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
