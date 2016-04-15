#include "message_handling.h"

int unpack_current_floor_message(char* buffer, int* elevator_id, int* current_floor) {

  int temp_el_id, temp_current_floor;

  sscanf(buffer, "<1E%dF%d>", &temp_el_id, &temp_current_floor);

  *elevator_id = temp_el_id;
  *current_floor = temp_current_floor;

  return 0;
}

int unpack_button_click_message(char* buffer, int* elevator_id, int* button_type, int* button_floor, int* queue_message) {

  int temp_el_id;
  int floor_counter, temp_message, initial_message;
  sscanf(buffer, "<2E%dM%d>", &temp_el_id, &temp_message);
  floor_counter = 0;
  initial_message = temp_message;

  while(temp_message >= 10) {
    floor_counter++;
    temp_message -= 10;
  }

  *elevator_id = temp_el_id;
  *button_type = temp_message;
  *button_floor = floor_counter;
  *queue_message = initial_message;

  return 0;
}

int queue_format_to_floor_and_button(int queue_order, int* floor, int* button_type) {

  int floor_counter = 0;

  while(queue_order >= 10) {
    floor_counter++;
    queue_order -= 10;
  }

  *floor = floor_counter;
  *button_type = queue_order;
  return 0;
}

int floor_and_button_to_queue_format(int* queue_order, int floor, int buttonType) {
  return 0;
}
