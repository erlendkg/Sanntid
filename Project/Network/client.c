#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

El_Status E;

void* receive_thread(void *sockfd)
{
  char message[32];
  int bytes_received;
  int length = 32;
  char Server_reply[32];
  int MsgEl, MsgFloor;

  while(1)
  {
    if (bytes_received = (bytes_received = recv(*(int*)sockfd, Server_reply, length, 0)) != 0)
      {
        printf("%d\n",bytes_received);
        sscanf(Server_reply, "<E%dF%d>", &MsgEl, &MsgFloor);

        if (MsgEl ==  E.Num)
        {
          E.DesiredFloor = MsgFloor;
          E.Done = 0;
          printf("Pesant #%d, Move your ass to coal sector %d\n", MsgEl, MsgFloor);

        }
        else
        {
          printf("%d\n",bytes_received);
        }
      }
  }
}
/*
void elev_go_to_floor()
{
  while(1){
      if (E.CurrentFloor > E.DesiredFloor){
        elev_set_motor_direction(DIRN_UP);
      }  else if (E.CurrentFloor > E.DesiredFloor){
        elev_set_motor_direction(DIRN_Down);
      }
      if (E.CurrentFloor == E.DesiredFloor)
  }
}**/


int main()
{

struct addrinfo hints;
struct addrinfo *servinfo;
pthread_t receiveThread;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("78.91.70.37", "312326", &hints, &servinfo) != 0){
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
pthread_create(&receive_thread, NULL, receiveThread, &sockfd);
pthread_join(receiveThread, NULL);
close(sockfd);


// char *msg = "A great disturbance i sense, young padawan";
// int len, bytes_sent;
//
// len = strlen(msg);
// bytes_sent = send(sockfd, msg, len, 0);



}
