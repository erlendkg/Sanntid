
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
  int active_connections;
  int client_sockets[MAX_NUMBER_OF_ELEVS];
  int server_socket;
} Network_status;

pthread_mutex_t net_stat_lock;


int main_server();
int main_client(char const *server_ip);
int initialize_server_socket();
int initialize_client_socket(char const *server_ip);
int wait_for_orders_from_server(int server_socket);
int update_elevator_status();

//void* listen_for_orders(void *sockfd);
//char *get_string(int msgType);
//void Send_message(void *sockfd);
int sendall(int s, char *buf, int *len);
void *thread_listen_for_clients(void *net_status);
void *thread_maintain_active_connections(void *net_status);
void *thread_recieve_orders_from_elevator(void *net_status);
