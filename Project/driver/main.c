
#include <stdio.h>

#include "elev.h"

#include <unistd.h>



int main() {


    elev_init();
    elev_button_type_t b;

    while(1) {
      if(elev_get_button_signal(b = BUTTON_CALL_UP, 1) == 1) {
        printf("Hello up\n");
        sleep(1);
      }

      if(elev_get_button_signal(b = BUTTON_CALL_DOWN, 1) == 1) {
        printf("Hello down\n");
        sleep(1);
      }


    }




    printf("Press STOP button to stop elevator and exit program.\n");

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
