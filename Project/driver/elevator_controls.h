#include "elev.h"
#define MAX_QUEUE_SIZE 10

typedef struct {
  int floor;
  int button_type;
} Order;


int go_to_floor(int current_floor, int desired_floor);
int return_current_floor();
int add_order_to_history(int floor, int button_type, Order order_history[MAX_QUEUE_SIZE]);
int empty_order_history();

Order return_button_input();
void *thread_monitor_button_inputs(void* order_history[MAX_QUEUE_SIZE]);
