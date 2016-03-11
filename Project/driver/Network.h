

#include <assert.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


#define PORT "3492"

void* listen_for_orders(void *sockfd);
char *get_string(int msgType);


void Send_message(void *sockfd);
int initialize_listen(char* server_ip);

int sendall(int s, char *buf, int *len);
