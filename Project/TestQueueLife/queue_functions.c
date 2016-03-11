#include "queue_functions.h"


int assign_order_to_queue(int order, int order_queue[MAX_QUEUE_SIZE], int max_queue_size) {

  int index;

  for(index = 0; index < max_queue_size; index++) {

    if((order_queue[index]) == 0) {
      order_queue[index] = order;
      break;
    }
  }
  return 1;
}


int read_order_from_queue(int order_queue[MAX_QUEUE_SIZE], int max_queue_size, int index_of_order) {
  int order = order_queue[index_of_order];
  return order;
}

int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue)
{
  memset(order_queue, 0, size_of_order_queue);
  return 1;
}


void test_queue_functions()
{

  int k, order;
  int order_queue[10];
  int c;
  size_t l = sizeof order_queue;


  flush_order_queue(order_queue,l);


  for (k = 0; k < 10; k++) {
    printf("%d\n",order_queue[k]);
  }

  while(1) {
    printf("-1 for flush, 0 for remove a number, 1 to add into queue, anything else to add to queue.\n")
    scanf("%d", &c);

    if(c == -1)
    {
      flush_order_queue(order_queue,l);
    }
    else if(c == 0)
      remove
    printf("---------------------------------\n");

    assign_order_to_queue(c, order_queue, 10);


    for (k = 0; k < 10; k++) {
      order = read_order_from_queue(order_queue, 10, k);
      printf("%d\n",order);
    }

    printf("---------------------------------\n");


  }
  return;
}
