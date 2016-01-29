#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define MYPORT "312326"
#define BACKLOG 5


void* listening_thread_function(void* server_socket) {

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int new_sock, len;
  char *msg = "<E2F9>";

  len = strlen(msg);

while(1) {
  printf("Listening for incoming connections...\n");

  new_sock = listen( *(int *)server_socket, BACKLOG);

  printf("Connection accepted from:\n");



  addr_size = sizeof their_addr;
  new_sock = accept(*(int *) server_socket, (struct sockaddr *)&their_addr, &addr_size);
  send(new_sock, msg, len, 0);

  return NULL;
  }
}

int main (int argc, char const *argv[]) {


  int i;


  int addrinfo_err, server_socket, *client_sockets;
  struct addrinfo hints, *serverinfo, *p;
  pthread_t listening_thread;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;     //IPV4 or IPV6
  hints.ai_socktype = SOCK_STREAM; //TCP socket
  hints.ai_flags = AI_PASSIVE;

  if ( (addrinfo_err = getaddrinfo(NULL, MYPORT, &hints, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(addrinfo_err));
    exit(1);
  }

  if((server_socket = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1) {
    perror("server socket");
  }

  if(bind(server_socket, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1) {
    perror("binding");
  }

    pthread_create(&listening_thread, NULL, listening_thread_function, (void *) &server_socket);

    pthread_join(listening_thread,NULL);

    return 0;

}
