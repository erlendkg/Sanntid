#include "panel_lights.h"

int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][2]) {
  int floor, button_type;

  for (floor = 0; floor < N_FLOORS; floor++) {
    for (button_type = 0; button_type < 2; button_type++) {
      if((floor == 0) && (button_type == 1)) {
        lamp_matrix[floor][button_type] = -1;
      }
      else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
      lamp_matrix[floor][button_type] = -1;
      } else {
      lamp_matrix[floor][button_type] = 0;
      }
    }
  }
  return 0;
}

int update_lamp_matrix(int lamp_matrix[N_FLOORS][2], int floor, int button_type, int light_status) {
    lamp_matrix[floor][button_type] = light_status;
    return 0;
}

int update_panel_lights(int lamp_matrix[N_FLOORS][2]) {
  int floor, button_type;

  for (floor = 0; floor < N_FLOORS; floor++) {
    for (button_type = 0; button_type < 2; button_type++) {
      if((floor == 0) && (button_type == 1)) {
        lamp_matrix[floor][button_type] = -1;
      } else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
        lamp_matrix[floor][button_type] = -1;
      } else {
        update_button_lights(button_type, floor, lamp_matrix[floor][button_type]);
      }
    }
  }
  return 0;
}
