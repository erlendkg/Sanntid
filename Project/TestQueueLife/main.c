
#include "queue_functions.h"


//Tar inn beskjeder Elevator, Floor og ButtonType
int main() {

      struct Elevator_data E0;
      queues_initiate(&E0);

      printf("%d\n%d\n%d\n", E0.queue[1], E0.status, E0.currentFloor);

      int button_order;

      while(1){

        printf("give BT2 order\n");
        scanf("%d", &button_order);

        place_bt2_order(&E0, button_order);
        print_queue(E0.queue);
      }
}
