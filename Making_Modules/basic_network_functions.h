#ifndef BASIC_NETWORK_FUNCTIONS_H_DEF
#define BASIC_NETWORK_FUNCTIONS_H_DEF

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
#define MAX_MESSAGE_SIZE 512
#define BACKLOG 10

typedef struct {
        int active_connections;
        int client_sockets[MAX_NUMBER_OF_ELEVS];
        int master_socket;
} Network_status;

int initialize_server_socket();
int initialize_client_socket(char const *server_ip);
int add_all_socks_to_fdlist(fd_set *readfds, Network_status *net_status);
int accept_client(Network_status *net_status, int *new_socket_pointer);
int read_message_from_client(int client_sockets, char* buf);
int send_all(int recipient_socket, char *buf, int *len);
int listen_for_message_from_master(char *buffer, int master_socket, int buffer_size); //returns -1 if master disconnects

#endif
