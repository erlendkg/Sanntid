#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define BACKLOG 10

int main(int argc, char const *argv[]) {

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints;
  struct addrinfo *serverinfo;
  int s, news;
  int status;

  memset(&hints, 0, sizeof hints);


  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, "34000", &hints, &serverinfo)) != 0)
  {
    printf("getaddrinfo error\n");
  }


  if((s = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1)
  {
    printf("could not create socket descriptor\n");
  };

  bind(s, serverinfo->ai_addr, serverinfo->ai_addrlen);

  printf("Hello\n");
  listen(s, BACKLOG);
  printf("I has connection");
  addr_size = sizeof their_addr;
  news = accept(s, (struct sockaddr *)&their_addr, &addr_size);

  return 0;

}
