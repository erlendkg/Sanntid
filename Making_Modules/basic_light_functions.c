#include "basic_light_functions.h"

int inner_button_light_switch(int floor, int value) {
  elev_set_button_lamp(BUTTON_COMMAND, floor, value);

}

int button_up_light_switch(int floor, int type) {
  elev_set_button_lamp(BUTTON_CALL_DOWN, floor, value);
}

int button_down_light_switch(int floor, int value) {
  elev_set_button_lamp(BYTTON_CALL_DOWN, floor, value);
}

int update_floor_indicator(int floor) {
  elev_set_floor_indicator(floor);
}
