#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>

void* recieveFromClient(void *news)
{
  printf("Connection Recieved\n");
  char *msg = "Hide yo wife!";
  int len, bytes_sent, i;

  len = strlen(msg);
  printf("I am thread\n");

  while(i < 10)
  {

  if((bytes_sent = send( *(int*)news, msg, len, 0)) == -1)
  {
    printf("Failed to send Message");
  }
  else
  {
    printf("Sending Message %d\n", (i+1));
  }

  sleep(1);
  i++;

  }

  close(*(int*) news);

  return NULL;
}


#define BACKLOG 10

int main(int argc, char const *argv[]) {

  pthread_t recieveThread;
  struct sockaddr_storage their_addr;
  struct addrinfo hints;
  struct addrinfo *serverinfo;
  int s, news;
  int status;
  socklen_t addr_size;

  memset(&hints, 0, sizeof hints);


  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, "35178", &hints, &serverinfo)) != 0)
  {
    printf("getaddrinfo error\n");
  }


  if((s = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1)
  {
    printf("could not create socket descriptor\n");
  };

  bind(s, serverinfo->ai_addr, serverinfo->ai_addrlen);

  while(1)
  {

  printf("Listening for connections...\n");
  listen(s, BACKLOG);

  addr_size = sizeof their_addr;
  news = accept(s, (struct sockaddr *)&their_addr, &addr_size);

  pthread_create(&recieveThread, NULL , recieveFromClient, &news);

  }


  return 0;

}
