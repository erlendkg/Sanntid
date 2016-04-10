#include "basic_network_functions.h"
#include "basic_light_functions.h"

int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][2]);
int update_lamp_matrix(int lamp_matrix[N_FLOORS][2], int floor, int button_type, int light_status);
int update_panel_lights(int lamp_matrix[N_FLOORS][2]);
