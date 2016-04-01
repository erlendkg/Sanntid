
#include "queue_functions.h"


//Tar inn beskjeder Elevator, Floor og ButtonType
int main(){

  struct Elevator_data E[N_ELEVATORS];

    bt01Test_queues_initiate(E);

  E.status = 2;
  E.currentFloor = 9;
  E.queueSize = 0;
  int button_order = 1;

  insert_item(E.queue, 0, button_order);
  printf("Elevstatus %d\n", E.status);
  update_elevator_status(E.queue, &E.status, button_order);
  printf("Elevstatus %d\n", E.status);

//test_all_buttons();

}
