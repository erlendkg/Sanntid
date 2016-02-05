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


void* receiveThread(void *sockfd)
{
  char msg[1024];
  int bytes_received;
  int len;

while(1)
{

printf("Write a message:\n");
scanf("%s", msg);
len = strlen(msg);
send( *(int*)sockfd, msg, len+1, 0);

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
//"34933"
if (getaddrinfo("129.241.187.23", "33546", &hints, &servinfo) != 0){
  printf("failed to getaddr\n");
}
else
{
  printf("succeded to get getaddrinfo.\n");
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
