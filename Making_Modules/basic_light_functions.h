#ifndef BASIC_LIGHT_FUNCTIONS_H_DEF
#define BASIC_LIGHT_FUNCTIONS_H_DEF

#include "elev.h"
#define ON 1
#define OFF 0

int inner_button_light_switch(int floor, int value);
int button_up_light_switch(int floor, int type);
int button_down_light_switch(int floor, int value);

int update_floor_indicator(int floor);

int update_button_lights(int button_type, int floor, int light_status);

#endif
