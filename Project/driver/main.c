#include "elevator_controls.h"
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include "Network.h"


 int main(int argc, char const *argv[]) {
   if (atoi(argv[1]) == 1) {
     main_server();
  //    Order orders[MAX_QUEUE_SIZE];
  //    int i;
  //    elev_init();
  //    pthread_t buttons;
   //
  //    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
  //    orders[i] = malloc(sizeof(Order));
   //
  //  }
   //
  //    pthread_create(&buttons, NULL, thread_monitor_button_inputs, (void *) orders);
   //
   //
   //
  //    while(1) {
   //
   //
  //      for (i = 0; i < MAX_QUEUE_SIZE; i++) {
  //        printf("Floor: %d, Direction: %d\n", orders[i].floor, orders[i].button_type);
  //      }
  //      printf("--------------------\n");
  //      sleep(5);
  //    }
     //
    //  pthread_join(buttons, NULL);



   }
   else if (atoi(argv[1]) == 2) {

     if(argc != 3) {
       printf("Not enough input arguments\n");
       exit(1);
     }
    main_client(argv[2]);
   }
   return 0;
}
