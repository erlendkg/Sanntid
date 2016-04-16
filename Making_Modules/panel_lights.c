#include "panel_lights.h"

int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
  int floor, button_type;

  for (floor = 0; floor < N_FLOORS; floor++) {
    for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
      if((floor == 0) && (button_type == 1)) {
        lamp_matrix[floor][button_type] = 2;
      }
      else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
      lamp_matrix[floor][button_type] = 2;
      } else {
      lamp_matrix[floor][button_type] = 0;
      }
    }
  }
  return 0;
}

int update_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int floor, int button_type, int light_status) {
    lamp_matrix[floor][button_type] = light_status;
    return 0;
}

int update_panel_lights(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
  int floor, button_type;

  for (floor = 0; floor < N_FLOORS; floor++) {
    for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
      if((floor == 0) && (button_type == 1)) {
        lamp_matrix[floor][button_type] = 2;
      } else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
        lamp_matrix[floor][button_type] = 2;
      } else {
        update_button_lights(button_type, floor, lamp_matrix[floor][button_type]);
      }
    }
  }
  return 0;
}

int bundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
  int floor, button_type, i;
  char current_char;
  int current_value;
  i = 1;
  message[0] = '3';

  for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
    for(floor = 0; floor < N_FLOORS; floor++) {
      current_value = lamp_matrix[floor][button_type];
      current_char = current_value + '0';
      message[i] = current_char;
      i++;
    }
  }
  message[i] = '\0';
  return 0;
}

int unbundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]) {
  int floor, button_type, i;
  char current_char;
  i = 1;

  for(button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
    for (floor = 0; floor < N_FLOORS; floor++) {
      current_char = message[i];
      printf("%c", current_char);
      lamp_matrix[floor][button_type] = current_char - '0';
      i++;
    }
  }
  return 0;
}
