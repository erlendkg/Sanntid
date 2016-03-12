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


void print_queue(int order_queue[MAX_QUEUE_SIZE]) {


  printf("---------------------------------\n");
  for (int k = 0; k < MAX_QUEUE_SIZE; k++) {
    int order =   order_queue[k];
    printf("%d\n",order);
  }
  printf("---------------------------------\n");

}

int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue)
{
  memset(order_queue, 0, size_of_order_queue);
  return 1;
}

int remove_first_item(int order_queue[MAX_QUEUE_SIZE]){
  for (int i = 0; i < MAX_QUEUE_SIZE-1; i++){
    order_queue[i] = order_queue[i+1];
  }
    order_queue[MAX_QUEUE_SIZE-1] = 0;

    return 1;
}

int insert_item(int order_queue[MAX_QUEUE_SIZE], int pos, int num){

  for (int i = MAX_QUEUE_SIZE; i > pos; i--){
    order_queue[i] = order_queue[i-1];
  }
    order_queue[pos] = num;
}


int is_order_on_the_way(int currentFloor, int status, int button_order){

  int rel_pos = button_order- currentFloor;

  if ((status == 0 && rel_pos > 0)||(status == 1 && rel_pos < 0)){
    return 1; //yes order is on the way
  }
  else {
    return 0; //no order is not on the way
  }
}

void place_order_on_the_way(int order_queue[MAX_QUEUE_SIZE], int * status, int button_order){

  int i = 0;

  if (*status == 0){

    while(1){

      if (button_order == order_queue[i]){
        break;//Order already exists, therefore nothing is done
      }
      else if ((button_order < order_queue[i]) || (i > 0 && order_queue[i] < order_queue[i-1])){ //the latter secures that data is stored if button_order is larger than the largest value in the queue.
        insert_item(order_queue, i, button_order);
        *status = 0;
        break;//The new order is added in the desired positon.
      }
      else if (button_order > order_queue[i]){
        i++;
      }

    }
  }
  else if (*status == 1){

    while(1){

      if (button_order == order_queue[i]){
        break;//Order already exists, therefore nothing is done
      }
      else if (button_order > order_queue[i]){
        insert_item(order_queue, i, button_order);
        *status = 1;
        break;//The new order is added in the desired positon.
      }
      else if (button_order < order_queue[i]){
        i++;
      }

    }
  }
}

void place_order_not_on_the_way(int order_queue[MAX_QUEUE_SIZE], int * status, int button_order){

  int i = MAX_QUEUE_SIZE-1;

  if (*status == 0){

    while(1){
      if (button_order > order_queue[i]){
        i--;
      }
      else if (button_order == order_queue[i]){
        break;//Order already exists, therefore nothing is done
      }
      else if (button_order < order_queue[i]){
        insert_item(order_queue, i+1, button_order);
        *status = 0;
        break;//The new order is added in the desired positon.
      }

    }
  }
  if (*status == 1){

    while(1){
      if (button_order < order_queue[i] || order_queue[i] == 0){
        i--;
      }
      else if (button_order == order_queue[i]){
        break;//Order already exists, therefore nothing is done
      }
      else if (button_order > order_queue[i]){
        insert_item(order_queue, i+1, button_order);
        *status = 1;
        break;//The new order is added in the desired positon.
      }

    }
  }
}
void place_bt2_order(struct Elevator_data * E, int button_order){

bool on_way;

    on_way =  is_order_on_the_way(E->currentFloor, E->status, button_order);

    if (on_way == 1){
      place_order_on_the_way(E->queue, &E->status, button_order);
    }
    else if(on_way == 0){
      place_order_not_on_the_way(E->queue, &E->status, button_order);
    }

}
