#include "queue_functions.h"


void test_queue_functions(int order_queue[MAX_QUEUE_SIZE])
{

  int k, order;
  int c;
  int a, b;
  size_t l = MAX_QUEUE_SIZE * sizeof (order_queue[0]);


  flush_order_queue(order_queue,l);


  for (k = 0; k < MAX_QUEUE_SIZE; k++) {
    printf("%d\n",order_queue[k]);
  }

  while(1) {
    printf("-1 for flush, 0 to remove first item, 666 to add number in desired pos.\n");
    scanf("%d", &c);

    if(c == -1)
    {
      flush_order_queue(order_queue,l);
    }
    else if(c == 0)
    {
      remove_first_item(order_queue);
    }
    else if(c == 666)
    {
      printf("On what position do you want to insert the number\n");
      scanf("%d", &a);
      printf("what number to be inserted?\n");
      scanf("%d", &b);
      insert_item(order_queue, a, b);
    }
    else {

      assign_order_to_queue(c, order_queue, MAX_QUEUE_SIZE);

    }
      print_queue(order_queue);
    }


}

void test_onway_functions(){

      struct Elevator_data E0;

        size_t l = MAX_QUEUE_SIZE * sizeof (E0.queue[0]);
        flush_order_queue(E0.queue,l);

      // E0.queue[0] = 10;
      // E0.queue[1] = 8;
      // E0.queue[2] = 6;
      // E0.queue[3] = 4;
      // E0.queue[4] = 1;
      // E0.queue[5] = 15;
      // E0.queue[6] = 20;
      //
      //   int button_order;
      //   E0.status = 1;
      //   E0.currentFloor = 12;

      E0.queue[0] = 6;
      E0.queue[1] = 8;
      E0.queue[2] = 10;
      E0.queue[3] = 14;
      E0.queue[4] = 19;
      E0.queue[5] = 3;
      E0.queue[6] = 1;

        int button_order;
        E0.status = 0;
        E0.currentFloor = 4;


        bool on_way;

        while(1){
          printf("give BT2 order\n");
          scanf("%d", &button_order);
          on_way =  is_order_on_the_way(E0.currentFloor, E0.status, button_order);
          printf("THIS IS on the way: %d\n", on_way);
          printf("STATUS: %d\n", E0.status);


          if (on_way == 1){
            place_order_on_the_way(E0.queue, &E0.status, button_order);
          }
          else if(on_way == 0){
            printf("her1\n");
            place_order_not_on_the_way(E0.queue, &E0.status, button_order);
          }
          print_queue(E0.queue);
      }

  }

  void bt2Test_queues_initiate(struct Elevator_data * E){

    size_t l = MAX_QUEUE_SIZE * sizeof (E->queue[0]);
    flush_order_queue(E->queue,l);

    //
    // E->queue[0] = 6;
    // E->queue[1] = 8;
    // E->queue[2] = 10;
    // E->queue[3] = 14;
    // E->queue[4] = 19;
    // E->queue[5] = 3;
    // E->queue[6] = 1;
    //
    //   int button_order;
    //   E->status = 0;
    //   E->currentFloor = 4;
      //
      // E->queue[0] = 10;
      // E->queue[1] = 8;
      // E->queue[2] = 6;
      // E->queue[3] = 4;
      // E->queue[4] = 3;
      // E->queue[5] = 15;
      // E->queue[6] = 20;
      //
      //
        E->status = 1;
        E->currentFloor = 12;

  }

  void bt2Test() {

        struct Elevator_data E[N_ELEVATORS-1];
        bt2Test_queues_initiate(&E[0]);

        printf("%d\n%d\n%d\n", E[0].queue[1], E[0].status, E[0].currentFloor);

        int button_order;

        while(1){

          printf("give BT2 order\n");
          scanf("%d", &button_order);

          place_bt2_order(&E[0], button_order);
          print_queue(E[0].queue);
        }
  }

  void bt01Test_queues_initiate(struct Elevator_data E[N_ELEVATORS]){

    for(int i = 0; i < N_ELEVATORS; i++){
      size_t l = MAX_QUEUE_SIZE * sizeof (E[i].queue[0]);
      flush_order_queue(E[i].queue,l);
    }

//ELEV 0 is going downwards from floor 12
      E[0].queue[0] = 10;
      E[0].queue[1] = 8;
      E[0].queue[2] = 6;
      E[0].queue[3] = 4;
      E[0].queue[4] = 3;
      E[0].queue[5] = 15;
      E[0].queue[6] = 20;


        E[0].status = 1;
        E[0].currentFloor = 12;
        E[0].queueSize = 7;

//ELEV 1 is going up from floor 8
      E[1].queue[0] = 9;
      E[1].queue[1] = 10;
      E[1].queue[2] = 12;
      E[1].queue[3] = 14;
      E[1].queue[4] = 19;
      E[1].queue[5] = 6;
      E[1].queue[6] = 3;


        E[1].status = 0;
        E[1].currentFloor = 8;
        E[1].queueSize = 7;

//ELEV 2 is idle floor 9
      E[2].status = 2;
      E[2].currentFloor = 9;
      E[2].queueSize = 0;

  }
void test_all_buttons(){

  struct Elevator_data E[N_ELEVATORS];
  int button_order, BT, elev, MsgElev, MsgFloor;

    bt01Test_queues_initiate(E);

    for(int i = 0; i < N_ELEVATORS; i++){
      printf("Tis is Queue %d\n", i);
      print_queue(E[i].queue);
    }

    while(1){
      printf("0 for up, 1 for down and 2 for inside, anything else to type a message from the elevators: \n");
      scanf("%d", &BT);

      if (BT == 2){
        printf("You said 2. Which elevator are you in?\n");
        scanf("%d", &elev);
        printf("What floor do you want to go to?\n");
        scanf("%d", &button_order);

        place_bt2_order(&E[elev], button_order);

      }
      else if (BT == 1){
        printf("You said 1. What floor do you want to go to?\n");
        scanf("%d", &button_order);
        place_bt1_order(E, button_order);
      }
      else if (BT == 0){
        printf("You said 0. What floor do you want to go to?\n");
        scanf("%d", &button_order);
        place_bt0_order(E, button_order);
      }
      else {
        printf("You are now simulating an incoming message.\n");
        printf("What elevator is the message from\n");
        scanf("%d", &MsgElev);
        printf("What what floor is it on:\n");
        scanf("%d", &MsgFloor);

        handle_elevator_message(&E[MsgElev], MsgFloor);

      }

      for(int i = 0; i < N_ELEVATORS; i++){
        printf("Tis is Queue %d\n", i);
        print_queue(E[i].queue);
      }
    }
}
