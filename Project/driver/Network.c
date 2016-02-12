#include "elev.h"
#include "channels.h"
#include "io.h"
#include "Network.h"

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


void* listen_for_orders(void *sockfd)
{
  int bytes_received;
  int length = 32;
  char Server_reply[32];
  int MsgEl, MsgFloor;

  while(1)
  {
    if (bytes_received = recv(*(int*)sockfd, Server_reply, length, 0) != 0)
      {
        sscanf(Server_reply, "<E%dF%d>", &MsgEl, &MsgFloor);

        if (MsgEl ==  E.Num)
        {
          E.DesiredFloor = MsgFloor;
          E.TaskComplete = 0;
          printf("Pesant #%d, Move your ass to coal sector %d\n", MsgEl, MsgFloor);

        }
        else
        {
          printf("%d\n",bytes_received);
        }
      }
  }
}


int initialize_listen(){

struct addrinfo hints;
struct addrinfo *servinfo;
pthread_t receive;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("78.91.68.70", "312326", &hints, &servinfo) != 0){
  printf("failed to connect\n");
}
else
{
  printf("succeded to get getaddrinfo for: 78.91.69.119\n");
}

/* Here we create sockets*/
int sockfd;

sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

printf("sock %d\n", sockfd);

if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
{
  printf("connection failed+\n");
  return 0;
}
else
{
  printf("connection successful\n");
}

E.Num = 2;

printf("entering thread\n");
pthread_create(&receive, NULL, listen_for_orders, &sockfd);
pthread_join(receive, NULL);
close(sockfd);

return 0;


}
