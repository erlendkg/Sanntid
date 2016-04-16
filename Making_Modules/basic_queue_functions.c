#include "basic_queue_functions.h"
#include "message_handling.h"

void print_queue(int order_queue[QUEUE_SIZE]) {

  printf("---------------------------------\n");
  for (int k = 0; k < QUEUE_SIZE; k++) {
    int order =   order_queue[k];
    printf("%d, ",order);
  }
  printf("\n---------------------------------\n");

}

int flush_order_queue(int order_queue[QUEUE_SIZE], size_t size_of_order_queue) {
    memset(order_queue, 0, size_of_order_queue);
    return 1;
  }

void update_elevator_status_and_queuesize(int order_queue[QUEUE_SIZE], int * status, int * queue_size, int current_floor){


  int queue_instance = order_queue[0];
  int floor_in_queue = 0;

  while(queue_instance >= 10) {
    floor_in_queue++;
    queue_instance -= 10;
  }


  if (queue_instance == 0){
      *status = 2;
  }
  else if (current_floor > floor_in_queue){

      *status = 1;
  }
  else if (current_floor < floor_in_queue){
  *status = 0;
  }

  for (int i = 0; i < QUEUE_SIZE; i++){

    if (order_queue[i] == 0) {
      *queue_size = i;
      break;
    }
  }
}

int is_elevator_disabled(int status){

    if (status == -1){
      return 1;
    }
    else {return 0;}
}

void disable_elevator_and_distribute_queue_to_other_elevators( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int disconnectedSocket){

  pthread_mutex_lock(&lock);

  int queueCounter = 0,elevCounter = 0, crashedElev;
  int length_of_elevator_array = E[99].length_of_elevator_array;

  while(1){
    if (E[elevCounter].socket == disconnectedSocket){
      crashedElev = elevCounter;
      break;
    }
    elevCounter++;
  }

  E[crashedElev].status = -1;

  while (E[crashedElev].queue[queueCounter] != 0){

    add_new_order_to_queue(E,E[crashedElev].queue[queueCounter],0,0,length_of_elevator_array);

    queueCounter++;
  }

  if (crashedElev == length_of_elevator_array - 1){
    E[99].length_of_elevator_array --;
  }

  pthread_mutex_unlock(&lock);
}

void activate_single_queue( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int elevatorNumber){

  size_t l = QUEUE_SIZE * sizeof (E[elevatorNumber].queue[0]);
  flush_order_queue(E[elevatorNumber].queue,l);

  E[elevatorNumber].status = 2;
  E[elevatorNumber].queue_size = 0;
  E[elevatorNumber].current_floor = 1;
  E[elevatorNumber].last_order = 0;

}

int assign_number_to_new_elevator( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators){

  int i;

  for(i = 0; i<numberOfElevators; i++){
    if (E[i].status == -1){
      return i;
    }
  }
  return numberOfElevators;
}

void initiate_queues( Elevator_data E[MAX_NUMBER_OF_ELEVATORS]){

      for(int i = 0; i < MAX_NUMBER_OF_ELEVATORS; i++){
        size_t l = QUEUE_SIZE * sizeof (E[i].queue[0]);
        flush_order_queue(E[i].queue,l);
      }
}

void remove_item_from_queue(int order_queue[QUEUE_SIZE]){
  for (int i = 0; i < QUEUE_SIZE-1; i++){
    order_queue[i] = order_queue[i+1];
  }
    order_queue[QUEUE_SIZE-1] = 0;
}

void insert_item(int order_queue[QUEUE_SIZE], int pos, int num){

  for (int i = QUEUE_SIZE; i > pos; i--){
    order_queue[i] = order_queue[i-1];
  }
    order_queue[pos] = num;
}

int is_order_on_the_way(int current_floor, int status, int button_order){

  int rel_pos = button_order- current_floor;

  if ((status == 0 && rel_pos > 0)||(status == 1 && rel_pos < 0)){
    return 1; //yes order is on the way
  }
  else {
    return 0; //no order is not on the way
  }
}

void place_order_on_the_way(int order_queue[QUEUE_SIZE], int * status, int button_order){

  int i = 0;

  if (*status == 0 ){

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

void place_order_not_on_the_way(int order_queue[QUEUE_SIZE], int * status, int button_order){

  int i = QUEUE_SIZE-1;

  if (*status == 0 ){

    while(1){

      printf("Button ORder: %d, Order queue qyeye: %d, I = %d\n", button_order, order_queue[i], i);

      if (i == 0){
        printf("HESTNIGGERS her: button order: %d, plass: 0 \n", button_order);

        insert_item(order_queue, 0, button_order);
      }
      else if (button_order > order_queue[i]){
        i--;
      }
      else if (button_order == order_queue[i]){
        break;//Order already exists, therefore nothing is done
      }
      else if (button_order < order_queue[i]){
        printf("inserter her: button order: %d, plass: %d \n", button_order, i+1);
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

void place_bt2_order( Elevator_data * E, int button_order){

  bool on_way;

      if (E->status != 2){
      on_way = is_order_on_the_way(E->current_floor, E->status, button_order);
      if (on_way == 1){
        place_order_on_the_way(E->queue, &E->status, button_order);
      }
      else if(on_way == 0){
        place_order_not_on_the_way(E->queue, &E->status, button_order);
      }
     }
    else if (E->status == 2){

        insert_item(E->queue, 0, button_order);
       //update_elevator_status_and_queuesize(E->queue, &E->status, &E->queue_size, E->current_floor);
    }
}

void place_bt0_order( Elevator_data E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;

  int desired_floor;
  int button_type;

  queue_format_to_floor_and_button(button_order, &desired_floor, &button_type);

  for(int i = 0; i < length_of_elevator_array; i++){
    //Check if the elevator is disabled, jump over it if it is.
    i = i + is_elevator_disabled(E[i].status);

    //for every iteration, check if the order is on the way up for an elevator
    if(E[i].status == 0 && is_order_on_the_way(E[i].current_floor, E[i].status, desired_floor)){

      place_order_on_the_way(E[i].queue, &E[i].status, desired_floor);
      break;
    }
    else if(( E[i].status == 2 )&& ( abs(E[i].current_floor - desired_floor) < closeness )){
      closeness = abs(E[i].current_floor - desired_floor);
      closest_elev = i;
    }

    if(E[i].queue_size < smallest_queue){

      smallest_elev = i;
      smallest_queue = E[i].queue_size;
    }

    //the order was not on the way for any elevators,
    if((i >= length_of_elevator_array - 1) && (closest_elev != -1)){
      insert_item(E[closest_elev].queue, 0, button_order);
      //update_elevator_status_and_queuesize(E[i].queue, &E[i].status, &E[i].queue_size, E[i].current_floor);
    }
    else if((i >= length_of_elevator_array - 1) && (closest_elev == -1)){

      place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, button_order);
      //update_elevator_status_and_queuesize(E[smallest_elev].queue, &E[smallest_elev].status, &E[smallest_elev].queue_size, E[smallest_elev].current_floor);
    }
  }
}

void place_bt1_order( Elevator_data E[MAX_NUMBER_OF_ELEVATORS-1], int button_order, int length_of_elevator_array){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;
  int desired_floor;
  int button_type;

  queue_format_to_floor_and_button(button_order, &desired_floor, &button_type);
  printf("button order: %d, DF: %d, BT: %d\n", button_order, desired_floor, button_type);

  for(int i = 0; i < length_of_elevator_array; i++){
    //Check if the elevator is disabled, jump over it if it is.
    i = i + is_elevator_disabled(E[i].status);

    //for every iteration, check if the order is on the way up for an elevator
    if(E[i].status == 1 && is_order_on_the_way(E[i].current_floor, E[i].status, desired_floor)){

      place_order_on_the_way(E[i].queue, &E[i].status, desired_floor);
      printf("**********\nPlasserte on the way\n");
      break;
    }
    else if(( E[i].status == 2 )&& ( abs(E[i].current_floor - desired_floor) < closeness )){
      closeness = abs(E[i].current_floor - desired_floor);
      closest_elev = i;
    }

    if(E[i].queue_size < smallest_queue){

      smallest_elev = i;
      smallest_queue = E[i].queue_size;
    }

    //the order was not on the way for any elevators,
    if((i == length_of_elevator_array - 1) && (closest_elev != -1)){
      insert_item(E[closest_elev].queue, 0, button_order);
      printf("**********\nPlasserte in empty\n");
      }
    else if((i == length_of_elevator_array - 1) && (closest_elev == -1)){
      printf("queue1 : %d\n", E[smallest_elev].queue[0]);
      place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, button_order);
      printf("**********\nPlasserte not on the way, jesus is the way\n");
      }
  }
}

void add_new_order_to_queue( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int desired_floor, int buttonType, int elevator, int length_of_elevator_array){

  if (buttonType == 0) {

    place_bt0_order(E, desired_floor, length_of_elevator_array);

  }
  else if (buttonType == 1) {

    place_bt1_order(E, desired_floor, length_of_elevator_array);

  }
  else if (buttonType == 2) {

    place_bt2_order(&E[elevator], desired_floor);

  }
}
