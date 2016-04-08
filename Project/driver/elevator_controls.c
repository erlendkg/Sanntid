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

int hold_doors_open(int duration) {
  int floor;

  if((floor = elev_get_floor_sensor_signal()) == -1) {
    fprintf(stderr, "Elevator between floors\n");
    return -1;
  }

  elev_set_door_open_lamp(1);
  sleep(duration);
  elev_set_door_open_lamp(0);
  return 1;
}

int initialize_hardware() {
  elev_init();


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
