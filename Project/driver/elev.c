#include "elev.h"
#include "channels.h"
#include "io.h"
#include "modules/error_and_logging.h"
#include "queue_functions.h"
#include "Network.h"


#include <assert.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


#define MOTOR_SPEED 2800


static const int lamp_channel_matrix[N_FLOORS][N_BUTTONS] = {
    {LIGHT_UP1, LIGHT_DOWN1, LIGHT_COMMAND1},
    {LIGHT_UP2, LIGHT_DOWN2, LIGHT_COMMAND2},
    {LIGHT_UP3, LIGHT_DOWN3, LIGHT_COMMAND3},
    {LIGHT_UP4, LIGHT_DOWN4, LIGHT_COMMAND4},
};


static const int button_channel_matrix[N_FLOORS][N_BUTTONS] = {
    {BUTTON_UP1, BUTTON_DOWN1, BUTTON_COMMAND1},
    {BUTTON_UP2, BUTTON_DOWN2, BUTTON_COMMAND2},
    {BUTTON_UP3, BUTTON_DOWN3, BUTTON_COMMAND3},
    {BUTTON_UP4, BUTTON_DOWN4, BUTTON_COMMAND4},
};


int single_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  pthread_t button_input, go_to_floor;

  pthread_create(&button_input, NULL, listen_for_button_input, (void*) this_elevator);
  pthread_create(&go_to_floor, NULL, elev_go_to_floor, (void *) this_elevator);

  while(1) {

    if((*server_socket = initialize_client_socket(server_ip)) != 2) {
      pthread_cancel(button_input);
      pthread_cancel(go_to_floor);
      return 0;
    }
  }
  pthread_join(go_to_floor, NULL);
  pthread_join(button_input, NULL);
}

int network_elevator_mode(Elev_info *this_elevator, int *server_socket, char const *server_ip) {
  pthread_t button_input, go_to_floor, send_status_to_server, recieve_orders;

  pthread_create(&button_input, NULL, listen_for_button_input, (void*) this_elevator);
  pthread_create(&go_to_floor, NULL, elev_go_to_floor, (void *) this_elevator);


}

void *send_status_to_server(void* this_elevator) {
  Elev_info* my_this_elevator = ((Elev_info *) this_elevator);
  char message[sizeof(Elev_info)];
  int length = sizeof(Elev_info);

  while(1) {
  sprintf(message, "%d%d", my_this_elevator->current_floor, my_this_elevator->desired_floor);
  sendall(my_this_elevator->server_socket, message, &length);
  }
}

int run_elevator(Elev_info *this_elevator) {

  pthread_t button_input, go_to_floor;
  int order_queue[MAX_QUEUE_SIZE];
  size_t size_of_queue = sizeof order_queue;
  int s;
  int len;
  char buf[100];
  char ip_addr[32] = "78.91.2.218";


  elev_set_motor_direction(DIRN_STOP);
  elev_init();
  run_down_until_hit_floor();
  elev_set_motor_direction(DIRN_STOP);

  pthread_create(&button_input, NULL, listen_for_button_input, NULL);
  pthread_create(&go_to_floor, NULL, elev_go_to_floor, NULL);


 while(1) {
    sprintf(buf, "<1E%dF%d>", this_elevator->current_floor, this_elevator->desired_floor);
    len = strlen(buf);
    sendall(s, buf, &len);
    sleep(2);
  }


  pthread_join(go_to_floor, NULL);
  pthread_join(button_input, NULL);

  return 1;
}

int run_down_until_hit_floor(){
  int currentFloor;

  currentFloor = elev_get_floor_sensor_signal();
    printf("%d", currentFloor);
  if(currentFloor == -1) {
    elev_set_motor_direction(DIRN_DOWN);
  }

  while((currentFloor == -1)) {

    if((currentFloor != -1)) {
         elev_set_motor_direction(DIRN_STOP);
        return 1;
    }
   }

}

void* listen_for_button_input(void *this_elevator) {

  int floor;
  Elev_info* cast_this_elevator = ((Elev_info *) this_elevator);

  while(1) {

    for(floor=0; floor<4; floor++){
      if (elev_get_button_signal(2, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Inside\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 2;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
      }
      if (elev_get_button_signal(1, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Down\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 1;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
      }
      if (elev_get_button_signal(0, floor) == 1){
          pthread_mutex_lock(&elev_info_lock);
          //printf("Floor %d, Up\n", (floor+1));
          cast_this_elevator->button_floor = (floor +1);
          cast_this_elevator->button_type = 0;
          cast_this_elevator->button_click = 1;
          cast_this_elevator->is_busy = 1;
          cast_this_elevator->desired_floor = (floor +1);
          pthread_mutex_unlock(&elev_info_lock);
      }
     }
    }

  return NULL;
}

void* elev_go_to_floor(void *this_elevator) {
  int floorSignal;
  Elev_info *cast_this_elevator = (Elev_info *) this_elevator;

  while(1){



    if ((floorSignal = elev_get_floor_sensor_signal()) != -1) {
      if (floorSignal +1 != cast_this_elevator->current_floor) { //INSIDE HERE WE NEED TO SEND A MESSAGE TO MASTER **************************

        //pthread_mutex_lock(&elev_info_lock);
        cast_this_elevator->current_floor = elev_get_floor_sensor_signal() +1;
        printf("Current Floor: %d    \tDesiredFloor: %d\n", cast_this_elevator->current_floor, cast_this_elevator->desired_floor);
        //pthread_mutex_unlock(&elev_info_lock);
        elev_set_floor_indicator((cast_this_elevator->current_floor -1));
    }
  }
    if(cast_this_elevator->desired_floor == cast_this_elevator->current_floor && cast_this_elevator->is_busy == 1){
      printf("Elevator have reached desired floor\n");
      cast_this_elevator->is_busy = 0;
    }

    if (cast_this_elevator->current_floor > cast_this_elevator->desired_floor){
      elev_set_motor_direction(DIRN_DOWN);

    }  else if (cast_this_elevator->current_floor < cast_this_elevator->desired_floor){
      elev_set_motor_direction(DIRN_UP);

    }  else if (cast_this_elevator->current_floor == cast_this_elevator->desired_floor){
      elev_set_motor_direction(DIRN_STOP);

    }

  }

  return NULL;
}

int elev_hold_door_open()
{
  int floor;

  if((floor = elev_get_floor_sensor_signal()) == -1) {
    fprintf(stderr, "Elevator between floors\n");
    return -1;
  }

  elev_set_door_open_lamp(1);
  sleep(DOOR_OPEN_TIME);
  elev_set_door_open_lamp(0);
  return 1;
}

//Basic functions below
void elev_init(void) {
    int init_success = io_init();
    assert(init_success &&  "Unable to initialize elevator hardware!" );

    for (int f = 0; f < N_FLOORS; f++) {
        for (elev_button_type_t b = 0; b < N_BUTTONS; b++){
            elev_set_button_lamp(b, f, 0);
        }
    }

    elev_set_stop_lamp(0);
    elev_set_door_open_lamp(0);
    elev_set_floor_indicator(0);
}


void elev_set_motor_direction(elev_motor_direction_t dirn) {
    if (dirn == 0){
        io_write_analog(MOTOR, 0);
    } else if (dirn > 0) {
        io_clear_bit(MOTORDIR);
        io_write_analog(MOTOR, MOTOR_SPEED);
    } else if (dirn < 0) {
        io_set_bit(MOTORDIR);
        io_write_analog(MOTOR, MOTOR_SPEED);
    }
}

void elev_set_button_lamp(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(button >= 0);
    assert(button < N_BUTTONS);

    if (value) {
        io_set_bit(lamp_channel_matrix[floor][button]);
    } else {
        io_clear_bit(lamp_channel_matrix[floor][button]);
    }
}


void elev_set_floor_indicator(int floor) {
    //assert(floor >= 0);
    //assert(floor < N_FLOORS);

    // Binary encoding. One light must always be on.
    if (floor & 0x02) {
        io_set_bit(LIGHT_FLOOR_IND1);
    } else {
        io_clear_bit(LIGHT_FLOOR_IND1);
    }

    if (floor & 0x01) {
        io_set_bit(LIGHT_FLOOR_IND2);
    } else {
        io_clear_bit(LIGHT_FLOOR_IND2);
    }
}


void elev_set_door_open_lamp(int value) {
    if (value) {
        io_set_bit(LIGHT_DOOR_OPEN);
    } else {
        io_clear_bit(LIGHT_DOOR_OPEN);
    }
}


void elev_set_stop_lamp(int value) {
    if (value) {
        io_set_bit(LIGHT_STOP);
    } else {
        io_clear_bit(LIGHT_STOP);
    }
}



int elev_get_button_signal(elev_button_type_t button, int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(button >= 0);
    assert(button < N_BUTTONS);


    if (io_read_bit(button_channel_matrix[floor][button])) {
        return 1;
    } else {
        return 0;
    }
}


int elev_get_floor_sensor_signal(void) {
    if (io_read_bit(SENSOR_FLOOR1)) {
        return 0;
    } else if (io_read_bit(SENSOR_FLOOR2)) {
        return 1;
    } else if (io_read_bit(SENSOR_FLOOR3)) {
        return 2;
    } else if (io_read_bit(SENSOR_FLOOR4)) {
        return 3;
    } else {
        return -1;
    }
}


int elev_get_stop_signal(void) {
    return io_read_bit(STOP);
}


int elev_get_obstruction_signal(void) {
    return io_read_bit(OBSTRUCTION);
}
