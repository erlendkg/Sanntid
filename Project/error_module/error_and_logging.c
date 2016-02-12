#include "error_and_logging.h"

int main() {


  print_network_error("I am a network error");
  print_elevator_hardware_error("I am an elevator hardware error");
  print_elevator_software_error("I am an elevator software error");
  print_fatal_error("I am a fatal error");
  return 0;
}

int print_network_error(char error_message[]) {
  fprintf(stderr, COLOR_BLUE "%s" COLOR_RESET "\n", error_message);
  return 1;
}

int print_elevator_hardware_error(char error_message[]) {
  fprintf(stderr, COLOR_GREEN "%s" COLOR_RESET "\n" , error_message);
  return 1;
}

int print_elevator_software_error(char error_message[]) {
  fprintf(stderr, COLOR_YELLOW "%s" COLOR_RESET "\n", error_message);
  return 1;
}

int print_fatal_error(char error_message[]) {
  fprintf(stderr, COLOR_RED "%s" COLOR_RESET "\n", error_message);
  return 1;
}
