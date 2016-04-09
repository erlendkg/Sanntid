#ifndef QUEUE_FUNCTIONS_H_DEF
#define QUEUE_FUNCTIONS_H_DEF

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 10
#define MAX_NUMBER_OF_ELEVATORS 100
#define N_ELEVATORS 100

typedef struct {
  int queue[MAX_QUEUE_SIZE];
  int queueSize;
  int status; // 0 Is up, 1 is down, 2 is idle. -1 is elevator not connected
  int currentFloor;
  int last_order;
  int socket;
  int lengthOfElevatorArray; //elev 99 contains this info
} Elevator_data;

pthread_mutex_t lock;

//Basic array functions
void print_queue(int order_queue[MAX_QUEUE_SIZE]);
int flush_order_queue(int order_queue[MAX_QUEUE_SIZE], size_t size_of_order_queue);
void removeItemFromQueue(int order_queue[MAX_QUEUE_SIZE]);
void insert_item(int order_queue[MAX_QUEUE_SIZE], int pos, int num);

//logic and browsing functions
int isElevatorDisabled(int status);
int is_order_on_the_way(int currentFloor, int status, int button_order);

void updateElevatorStruct(int order_queue[MAX_QUEUE_SIZE], int * status, int * queueSize, int currentFloor);
int isElevatorOnCorrectFloor( Elevator_data * E, int MsgFloor);

//Placing functions
void place_order_on_the_way(int order_queue[MAX_QUEUE_SIZE], int *status, int button_order);
void place_order_not_on_the_way(int order_queue[MAX_QUEUE_SIZE], int * status, int button_order);
void place_bt2_order( Elevator_data * E, int button_order);
void place_bt1_order( Elevator_data E[N_ELEVATORS-1], int button_order, int lengthOfElevatorArray);
void place_bt0_order( Elevator_data E[N_ELEVATORS-1], int button_order, int lengthOfElevatorArray);
void addNewOrderToQueue( Elevator_data E[N_ELEVATORS], int desired_floor, int buttonType, int elevator, int lengthOfElevatorArray);

//Crash handling Functions
void disableElevatorAndDistributeQueueToOtherElevators( Elevator_data E[N_ELEVATORS], int disconnectedSocket);

//Message handling functions
void unpackMessageToVariables(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor);

//Startup functions
void initiateQueues( Elevator_data E[N_ELEVATORS]);

//New elevator connected functions
int assignNumberToNewElevator( Elevator_data E[MAX_NUMBER_OF_ELEVATORS], int numberOfElevators);
void activateSingleQueue( Elevator_data E[N_ELEVATORS], int elevatorNumber);

//When recieving a message from master
char *actOnMessageFromMaster( Elevator_data E[N_ELEVATORS], char *messageFromElevator, int lengthOfElevatorArray);

#endif
