#include <stdio.h>

#define COLOR_RED     "\x1b[31m"  //Fatal errors making further program execution impossible
#define COLOR_YELLOW  "\x1b[33m"  //Warnings
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"

#define COLOR_RESET   "\x1b[0m"


int print_network_error(char error_message[]);
int print_elevator_hardware_error(char error_message[]);
int print_elevator_software_error(char error_message[]);
int print_fatal_error(char error_message[]);
