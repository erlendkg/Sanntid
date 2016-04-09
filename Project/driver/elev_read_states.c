#include "elev_read_state.h"

int return_current_floor() {
  int current_floor;

  current_floor = elev_get_floor_sensor_signal();
  current_floor += 1;

  return current_floor;
}

Order return_button_input() {
    int floor;
    Order my_order;

    while(1) {
      for(floor = 0; floor < N_FLOORS; floor++) {
        if(elev_get_button_signal(2, floor) == 1) {
          my_order.floor = (floor +1);
          my_order.button_type = 2;
          return my_order;
        }
        if(elev_get_button_signal(1,floor) == 1) {
          my_order.floor = (floor+1);
          my_order.button_type = 1;

          return my_order;
        }

       if(elev_get_button_signal(0,floor) == 1) {
        my_order.floor = (floor+1);
        my_order.button_type = 0;

        return my_order;

        }
      }
    }
  }
