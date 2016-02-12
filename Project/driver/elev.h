

// Wrapper for libComedi Elevator control.
// These functions provide an interface to the elevators in the real time lab


// Number of floors. Hardware-dependent, do not modify.
#define N_FLOORS 4

// Number of buttons (and corresponding lamps) on a per-floor basis
#define N_BUTTONS 3

#define DOOR_OPEN_TIME 1

typedef enum tag_elev_motor_direction {
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} elev_motor_direction_t;

typedef enum tag_elev_lamp_type {
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elev_button_type_t;

typedef struct {

  int Num;
  int DesiredFloor;
  int CurrentFloor;
  int TaskComplete;
  int ButtonClick;
  int ButtonFloor;
  int ButtonType;  //0 = up, 1 = down, 2 = inside
} El_status;


El_status E;

int run_elevator();
int run_down_until_hit_floor();


void* listen_for_orders(void *sockfd);
int initialize_listen();

void elev_init(void);

void* elev_go_to_floor();
void* listen_for_button_input();
int elev_hold_door_open();

void elev_set_motor_direction(elev_motor_direction_t dirn);
void elev_set_button_lamp(elev_button_type_t button, int floor, int value);
void elev_set_floor_indicator(int floor);
void elev_set_door_open_lamp(int value);
void elev_set_stop_lamp(int value);


int elev_get_button_signal(elev_button_type_t button, int floor);
int elev_get_floor_sensor_signal(void);
int elev_get_stop_signal(void);
int elev_get_obstruction_signal(void);
