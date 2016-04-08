#include "basic_network_functions.h"
#include "basic_elevator_functions.h"

int main(int argc, char const *argv[]) {

  int* r;
  int current_floor;
  initialize_hardware();

  while(1) {
    r = return_button_input();
    current_floor = return_current_floor();
    printf("type: %d, floor: %d\n", r[0], r[1]);
    printf("Current floor: %d\n", current_floor);
    go_to_floor(r[1]);
    sleep(5);
  }

  return 0;
}
