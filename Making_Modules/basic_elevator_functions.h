#ifndef BASIC_ELEVATOR_FUNCTIONS_H_DEF
#define BASIC_ELEVATOR_FUNCTIONS_H_DEF

#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct {
        int button_floor;
        int button_type;
} Button_click;

int initialize_hardware();


int go_up();
int go_down();
int stop_elevator();
int go_to_floor(int* desired_floor);

int hold_doors_open(int duration);

int return_current_floor();
int return_button_input(Button_click *button_order);


#endif
