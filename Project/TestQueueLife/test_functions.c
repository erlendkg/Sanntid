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

  void queues_initiate(struct Elevator_data * E){

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

      E->queue[0] = 10;
      E->queue[1] = 8;
      E->queue[2] = 6;
      E->queue[3] = 4;
      E->queue[4] = 1;
      E->queue[5] = 15;
      E->queue[6] = 20;

        int button_order;
        E->status = 1;
        E->currentFloor = 12;

  }
