#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char const *argv[]) {

  struct addrinfo hints;
  struct addrinfo  *serverinfo;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if(getaddrinfo(NULL, "30000", &hints, &serverinfo) !=0)
  {
    printf("getaddrinfo error\n");
  }




  /* code */
  return 0;
}
