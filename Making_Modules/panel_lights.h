#include "basic_network_functions.h"
#include "basic_light_functions.h"

#define EXTERNAL_BUTTONS 2

int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS];

int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]);
int update_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int floor, int button_type, int light_status);
int update_panel_lights(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]);
int bundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]);
int unbundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]);
