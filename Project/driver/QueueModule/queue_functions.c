#include "queue_functions.h"


void print_queue(int order_queue[MAX_QUEUE_SIZE]) {

  printf("---------------------------------\n");
  for (int k = 0; k < MAX_QUEUE_SIZE; k++) {
    int order =   order_queue[k];
    printf("%d, ",order);
  }
  printf("\n---------------------------------\n");

}

int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue)
  {
    memset(order_queue, 0, size_of_order_queue);
    return 1;
  }

void updateElevatorStruct(int order_queue[MAX_QUEUE_SIZE], int * status, int * queueSize, int currentFloor){

  if (order_queue[0] == 0){
      *status = 2;
  }
  else if (currentFloor > order_queue[0]){

      *status = 1;
  }
  else if (currentFloor < order_queue[0]){
  *status = 0;
  }

  for (int i = 0; i < MAX_QUEUE_SIZE; i++){

    if (order_queue[i] == 0) {
      *queueSize = i;
      break;
    }
  }
}

int isElevatorDisabled(int status){

    if (status == -1){
      return 1;
    }
    else {return 0;}
}

void disableElevatorAndDistributeQueueToOtherElevators( Elevator_data E[N_ELEVATORS], int disconnectedSocket){

  pthread_mutex_lock(&lock);

  int queueCounter = 0,elevCounter = 0, crashedElev;
  int lengthOfElevatorArray = E[99].lengthOfElevatorArray;

  while(1){
    if (E[elevCounter].socket == disconnectedSocket){
      crashedElev = elevCounter;
      break;
    }
    elevCounter++;
  }

  E[crashedElev].status = -1;

  while (E[crashedElev].queue[queueCounter] != 0){

    addNewOrderToQueue(E,E[crashedElev].queue[queueCounter],0,0,lengthOfElevatorArray);

    queueCounter++;
  }
  pthread_mutex_unlock(&lock);
}


void activateSingleQueue( Elevator_data E[N_ELEVATORS], int elevatorNumber){

  size_t l = MAX_QUEUE_SIZE * sizeof (E[elevatorNumber].queue[0]);
  flush_order_queue(E[elevatorNumber].queue,l);

  E[elevatorNumber].status = 2;
  E[elevatorNumber].queueSize = 0;
  E[elevatorNumber].currentFloor = 1;
  E[elevatorNumber].last_order = 0;

}

int assignNumberToNewElevator( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators){

  int i;

  for(i = 0; i<numberOfElevators; i++){
    if (E[i].status == -1){
      return i;
    }
  }
  return numberOfElevators;
}


void initiateQueues( Elevator_data E[N_ELEVATORS]){

      for(int i = 0; i < N_ELEVATORS; i++){
        size_t l = MAX_QUEUE_SIZE * sizeof (E[i].queue[0]);
        flush_order_queue(E[i].queue,l);
      }
}

void removeItemFromQueue(int order_queue[MAX_QUEUE_SIZE]){
  for (int i = 0; i < MAX_QUEUE_SIZE-1; i++){
    order_queue[i] = order_queue[i+1];
  }
    order_queue[MAX_QUEUE_SIZE-1] = 0;
}

void insert_item(int order_queue[MAX_QUEUE_SIZE], int pos, int num){

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



void place_bt2_order( Elevator_data * E, int button_order){

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
      //  updateElevatorStruct(E->queue, &E->status, &E->queueSize, E->currentFloor);
    }
}

void place_bt0_order( Elevator_data E[N_ELEVATORS-1], int button_order, int lengthOfElevatorArray){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;

  for(int i = 0; i < lengthOfElevatorArray; i++){

    //Check if the elevator is disabled, jump over it if it is.
    i = i + isElevatorDisabled(E[i].status);

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

    //the order was not on the way for any elevators,
    if((i >= lengthOfElevatorArray - 1) && (closest_elev != -1)){
      insert_item(E[closest_elev].queue, 0, button_order);
      updateElevatorStruct(E[i].queue, &E[i].status, &E[i].queueSize, E[i].currentFloor);

    }
    else if((i >= lengthOfElevatorArray - 1) && (closest_elev == -1)){
      //insert_item(E[smallest_elev].queue, 0, button_order);

      insert_item(E[smallest_elev].queue, E[smallest_elev].queueSize, button_order);
      updateElevatorStruct(E[smallest_elev].queue, &E[smallest_elev].status, &E[smallest_elev].queueSize, E[smallest_elev].currentFloor);
    }
  }



}

void place_bt1_order( Elevator_data E[N_ELEVATORS-1], int button_order, int lengthOfElevatorArray){

  int closest_elev = -1;
  int closeness = 9999;
  int smallest_queue = 9999;
  int smallest_elev;

  for(int i = 0; i < lengthOfElevatorArray; i++){
    //Check if the elevator is disabled, jump over it if it is.
    i = i + isElevatorDisabled(E[i].status);

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
    if((i == lengthOfElevatorArray - 1) && (closest_elev != -1)){

      insert_item(E[closest_elev].queue, 0, button_order);
      updateElevatorStruct(E[closest_elev].queue, &E[closest_elev].status, &E[closest_elev].queueSize, E[closest_elev].currentFloor);
        }
    else if((i == lengthOfElevatorArray - 1) && (closest_elev == -1)){

      place_order_not_on_the_way(E[smallest_elev].queue, &E[smallest_elev].status, button_order);
      updateElevatorStruct(E[smallest_elev].queue, &E[smallest_elev].status, &E[smallest_elev].queueSize, E[smallest_elev].currentFloor);
    }
  }
}


void addNewOrderToQueue( Elevator_data E[N_ELEVATORS], int desired_floor, int buttonType, int elevator, int lengthOfElevatorArray){

  if (buttonType == 0) {

    place_bt0_order(E, desired_floor, lengthOfElevatorArray);

  }
  else if (buttonType == 1) {

    place_bt1_order(E, desired_floor, lengthOfElevatorArray);

  }
  else if (buttonType == 2) {

    place_bt2_order(&E[elevator], desired_floor);

  }
}

int isElevatorOnCorrectFloor( Elevator_data * E, int MsgFloor){

  E->currentFloor = MsgFloor;

  if(E->queue[0] == MsgFloor){
    removeItemFromQueue(E->queue);
    updateElevatorStruct(E->queue, &E->status, &E->queueSize, E->currentFloor);

    return 1;
  }
  return 0;
}

void unpackMessageToVariables(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor) {


  int tempMsgType = -1, tempMsgEl = -1, tempMsgFloor = -1, tempMsgButton = -1;

  tempMsgType = str[1] - '0';


  if (tempMsgType == 1) {

    sscanf(str, "<1E%dF%d>", &tempMsgEl, &tempMsgFloor);
  }

  else if (tempMsgType == 2) {

    sscanf(str, "<2E%dBT%dF%d>", &tempMsgEl, &tempMsgButton, &tempMsgFloor);

  }

  *msgType = tempMsgType;
  *elevatorNumber = tempMsgEl;
  *buttonType = tempMsgButton;
  *elevatorFloor = tempMsgFloor;
}

char *actOnMessageFromMaster(Elevator_data E[N_ELEVATORS], char *messageFromElevator, int lengthOfElevatorArray){

  int msgType = 0, msgElevatorNumber = 0, msgButtonType = 0, msgElevatorFloor = 0, isElevatorOnCorrectFloor;
  unpackMessageToVariables(messageFromElevator, &msgType, &msgElevatorNumber, &msgButtonType, &msgElevatorFloor);
  //pthread_mutex_lock(&lock);

  if (msgType == 1){

    E[msgElevatorNumber].currentFloor = msgElevatorFloor;

    isElevatorOnCorrectFloor = (E[msgElevatorNumber].queue[0] == msgElevatorFloor);

    if (isElevatorOnCorrectFloor == 1){

      removeItemFromQueue(E[msgElevatorNumber].queue);

      if (E[msgElevatorNumber].queue[0] == 0){

        E[msgElevatorNumber].status = 2;
        return "0";
      }

        updateElevatorStruct(E->queue, &E->status, &E->queueSize, E->currentFloor);

        char *newMessage = (char*) malloc(16 * sizeof(int));

        sprintf(newMessage, "<1E%dF%d>", msgElevatorNumber, E[msgElevatorNumber].queue[0]);

        return newMessage;
      }
      else if (isElevatorOnCorrectFloor == 0) {

      char *newMessage = (char*) malloc(16 * sizeof(int));

      }
    } else if (msgType == 2) {
      addNewOrderToQueue(E, msgElevatorFloor, msgButtonType, msgElevatorNumber, lengthOfElevatorArray);
      //pthread_mutex_unlock(&lock);
      return "2";
    }  else {
    //  //pthread_mutex_unlock(&lock);

      //HER SKAL VI SENDE SAMME ORDRE PÅ NYTT
      return "fault";
    }
    return "error";
}
