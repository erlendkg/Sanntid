#include "basic_light_functions.h"

int inner_button_light_switch(int floor, int value) {
        elev_set_button_lamp(BUTTON_COMMAND, floor, value);
        return 0;
}

int button_up_light_switch(int floor, int value) {
        elev_set_button_lamp(BUTTON_CALL_UP, floor, value);
        return 0;
}

int button_down_light_switch(int floor, int value) {
        elev_set_button_lamp(BUTTON_CALL_DOWN, floor, value);
        return 0;
}

int update_floor_indicator(int floor) {
        elev_set_floor_indicator(floor);
        return 0;
}

int update_button_lights(int button_type, int floor, int light_status) {
        if(button_type == 0) {
                button_up_light_switch(floor, light_status);
        } else if (button_type == 1) {
                button_down_light_switch(floor, light_status);
        } else if (button_type == 2) {
                inner_button_light_switch(floor, light_status);
        }
        return 0;
}
