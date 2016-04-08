#include "elevator_controls.h"

int go_to_floor(int current_floor, int desired_floor) {

  while(current_floor != desired_floor) {
    if(current_floor > desired_floor) {
      elev_set_motor_direction(DIRN_DOWN);
    }
    else if (current_floor < desired_floor) {
      elev_set_motor_direction(DIRN_UP);
    }
  }
    elev_set_motor_direction(DIRN_DOWN);
    return 0;
}

int return_current_floor() {
  int current_floor;

  current_floor = elev_get_floor_sensor_signal();
  current_floor += 1;

  return current_floor;
}

int add_order_to_history(int floor, int button_type, Order order_history[MAX_QUEUE_SIZE]) {
  int i;

  for(i = 0; i < MAX_QUEUE_SIZE; i++) {
    if(order_history[i].floor == 0) {

      order_history[i].floor = floor;
      order_history[i].button_type = button_type;
      return 0;
    }
    else {
      return 1;
    }
  }
  return 0;
}

int empty_order_history(Order order_history[MAX_QUEUE_SIZE]) {
  int i;

  for(i = 0; i < MAX_QUEUE_SIZE; i++) {
    order_history[i].floor = 0;
    order_history[i].button_type;
  }
  return 0;
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

void *thread_monitor_button_inputs(void* order_history[MAX_QUEUE_SIZE]) {
  Order *my_order_history = (Order *) order_history;
  Order incoming_order;

  while(1) {
    incoming_order = return_button_input();
    if(incoming_order.floor != -1) {
      add_order_to_history(incoming_order.floor, incoming_order.button_type, my_order_history);
    }
  }
}
