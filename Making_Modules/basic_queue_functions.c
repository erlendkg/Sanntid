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

void remove_item_from_queue(int order_queue[QUEUE_SIZE]){
        for (int i = 0; i < QUEUE_SIZE-1; i++) {
                order_queue[i] = order_queue[i+1];
        }
        order_queue[QUEUE_SIZE-1] = 0;
}

void insert_item(int order_queue[QUEUE_SIZE], int pos, int num){

        for (int i = QUEUE_SIZE; i > pos; i--) {
                order_queue[i] = order_queue[i-1];
        }
        order_queue[pos] = num;
}
