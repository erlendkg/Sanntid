#ifndef PANEL_LIGHTS_H
#define PANEL_LIGHTS_H

#include "basic_network_functions.h"
#include "basic_light_functions.h"
#include "basic_queue_functions.h"
#include "basic_elevator_functions.h"

#define EXTERNAL_BUTTONS 2

int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS];

int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]);

int update_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int floor, int button_type, int light_status);
int update_panel_lights(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]);

int bundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]);
int unbundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]);

void* thread_update_lights();

void convert_lamp_matrix_to_orders(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int elevator_queue[10]);

#endif
