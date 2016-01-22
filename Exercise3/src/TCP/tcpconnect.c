#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


int main()
{

struct addrinfo hints;
struct addrinfo *servinfo;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("78.91.69.119", "34000", &hints, &servinfo) != 0){
  printf("failed to connect\n");
}
else
{
  printf("succeded to get getaddrinfo for: 78.91.71.255\n");
}

/* Here we create sockets*/
int sockfd;

sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);


if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) !=0)
{
  printf("error connecting\n");
}
else
{
  printf("Successful connection\n");
}
}
