
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
#define MAX_NUMBER_OF_ELEVS 3
#define BACKLOG 10

typedef struct {
  int active_connetions;
  int client_sockets[MAX_NUMBER_OF_ELEVS];
  int server_socket;
} Network_status;


int main_server ();
int initialize_server_socket();

void* listen_for_orders(void *sockfd);
char *get_string(int msgType);


void Send_message(void *sockfd);
int initialize_listen(char* server_ip);

int sendall(int s, char *buf, int *len);
void *thread_listen_for_clients(void *net_status);
