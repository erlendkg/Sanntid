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

void update_elevator_status(int order_queue[MAX_QUEUE_SIZE], int * status, int currentFloor){

  if (currentFloor < order_queue[0]){
      *status = 0;
  }
  else if (currentFloor > order_queue[0]){
      *status = 1;
  }
  else{
      *status = 2;
  }

}

void remove_first_item(int order_queue[MAX_QUEUE_SIZE]){
  for (int i = 0; i < MAX_QUEUE_SIZE-1; i++){
    order_queue[i] = order_queue[i+1];
  }
    order_queue[MAX_QUEUE_SIZE-1] = 0;
}

int handle_elevator_message(struct Elevator_data * E, int MsgFloor){

  E->currentFloor = MsgFloor;

  if(E->queue[0] == MsgFloor){
    remove_first_item(E->queue);
    update_elevator_status(E->queue, &E->status, E->currentFloor);
  }
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
      else if ((button_order > order_queue[i])||(i > 0 && order_queue[i] > order_queue[i-1])){
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
      if (i == 0){
        insert_item(order_queue, 0, button_order);
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


void queues_initiate(struct Elevator_data E[N_ELEVATORS]){

  for(int i = 0; i < N_ELEVATORS; i++){
    size_t l = MAX_QUEUE_SIZE * sizeof (E[i].queue[0]);
    flush_order_queue(E[i].queue,l);
  }
}

void place_bt2_order(struct Elevator_data * E, int button_order){

bool on_way;

    if (E->status != 2){
      on_way = is_order_on_the_way(E->currentFloor, E->status, button_order);
      if (on_way == 1){
        place_order_on_the_way(E->queue, &E->status, button_order);
      }
      else if(on_way == 0){
        place_order_not_on_the_way(E->queue, &E->status, button_order);
      }
    }
    else if (E->status == 2){
        insert_item(E->queue, 0, button_order);
        update_elevator_status(E->queue, &E->status, E->currentFloor);
    }
}

void place_bt0_order(struct Elevator_data E[N_ELEVATORS-1], int button_order){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;

  for(int i = 0; i < N_ELEVATORS; i++){
    //for every iteration, check if the order is on the way up for an elevator
    if(E[i].status == 0 && is_order_on_the_way(E[i].currentFloor, E[i].status, button_order)){

      place_order_on_the_way(E[i].queue, &E[i].status, button_order);
      break;
    }
    else if(( E[i].status == 2 )&& ( abs(E[i].currentFloor - button_order) < closeness )){
      closeness = abs(E[i].currentFloor - button_order);
      closest_elev = i;
    }

    if(E[i].queueSize < smallest_queue){

      smallest_elev = i;
      smallest_queue = E[i].queueSize;
    }
    printf("closes eelev %d", closest_elev);
    //the order was not on the way for any elevators,
    if((i == N_ELEVATORS - 1) && (closest_elev != -1)){
      insert_item(E[closest_elev].queue, 0, button_order);
      printf("Elevstatus %d\n", E[i].status);
      update_elevator_status(E[i].queue, &E[i].status, button_order);
      printf("Elevstatus %d\n", E[i].status);
    }
    else if((i == N_ELEVATORS - 1) && (closest_elev == -1)){
      //insert_item(E[smallest_elev].queue, 0, button_order);
      place_order_not_on_the_way(E[i].queue, &E[i].status, button_order);
      update_elevator_status(E[i].queue, &E[i].status, button_order);
      printf("laks2");
    }
  }



}
void place_bt1_order(struct Elevator_data E[N_ELEVATORS-1], int button_order){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;

  for(int i = 0; i < N_ELEVATORS; i++){

    //for every iteration, check if the order is on the way up for an elevator
    if(E[i].status == 1 && is_order_on_the_way(E[i].currentFloor, E[i].status, button_order)){

      place_order_on_the_way(E[i].queue, &E[i].status, button_order);
      break;
    }
    else if(( E[i].status == 2 )&& ( abs(E[i].currentFloor - button_order) < closeness )){
      closeness = abs(E[i].currentFloor - button_order);
      closest_elev = i;
    }

    if(E[i].queueSize < smallest_queue){

      smallest_elev = i;
      smallest_queue = E[i].queueSize;
    }

    //the order was not on the way for any elevators,
    if((i == N_ELEVATORS - 1) && (closest_elev != -1)){
      insert_item(E[closest_elev].queue, 0, button_order);
        }
    else if((i == N_ELEVATORS - 1) && (closest_elev == -1)){
      place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, button_order);
    }
  }

}
