#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct {
        int button_floor;
        int button_type;
} Button_click;

int initialize_hardware();
int go_to_floor(int desired_floor);
int hold_doors_open(int duration);
int return_current_floor();
int return_button_input(Button_click *button_order);
