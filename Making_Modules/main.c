#include "basic_network_functions.h"
#include "basic_elevator_functions.h"
#include "panel_lights.h"

int main(int argc, char const *argv[]) {

  if(argc == 1) {
    printf("Not enough input arguments");
    exit(1);
  }

  if(atoi(argv[1]) == 1) {
    main_server();
  }

  if(atoi(argv[1]) == 2) {
    if(argc != 3) {
      printf("Please input server ip adress\n");
      exit(1);
    }
    main_client(argv[2]);

  }

  return 0;
}
