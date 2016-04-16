#ifndef BASIC_QUEUE_FUNCTIONS_H_DEF
#define BASIC_QUEUE_FUNCTIONS_H_DEF

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "panel_lights.h"

#define QUEUE_SIZE 10
#define MAX_NUMBER_OF_ELEVATORS 100
#define N_FLOORS 4

void print_queue(int order_queue[QUEUE_SIZE]);
int flush_order_queue(int order_queue[QUEUE_SIZE], size_t size_of_order_queue);

void remove_item_from_queue(int order_queue[QUEUE_SIZE]);
void insert_item(int order_queue[QUEUE_SIZE], int pos, int num);

#endif
