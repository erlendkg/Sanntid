#include <stdio.h>
#include <unistd.h>

int initialize_hardware();
int go_to_floor(int desired_floor);
int hold_doors_open(int duration);
int return_current_floor();
int* return_button_input();
