#ifndef MESSAGE_HANDLING_H_DEF
#define MESSAGE_HANDLING_H_DEF

#include <stdio.h>

int unpack_current_floor_message(char* buffer, int* elevator_id, int* current_floor);
int unpack_button_click_message(char* buffer, int* elevator_id, int* button_type, int* button_floor, int* queue_message);

int queue_format_to_floor_and_button(int queue_order, int* floor, int* button_type);
int floor_and_button_to_queue_format(int* queue_order, int floor, int buttonType);

#endif
