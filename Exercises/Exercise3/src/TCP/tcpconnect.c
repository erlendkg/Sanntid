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

char server_reply[100];

void* receiveThread(void *sockfd)
{
  char *message;
  int bytes_received;
  int length = 100;

while(1)
{
  bytes_received = recv(*(int*)sockfd, server_reply, length, 0);
  printf("bytes_recieved: %d\n", bytes_received);
  if (bytes_received == 0)
  {
    break;
  }
  puts(server_reply);
  usleep(1000);
}
}

int main()
{

struct addrinfo hints;
struct addrinfo *servinfo;
pthread_t receive;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("78.91.69.119", "35178", &hints, &servinfo) != 0){
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



pthread_create(&receive, NULL, receiveThread, &sockfd);

pthread_join(receive, NULL);
close(sockfd);


// char *msg = "A great disturbance i sense, young padawan";
// int len, bytes_sent;
//
// len = strlen(msg);
// bytes_sent = send(sockfd, msg, len, 0);



}
