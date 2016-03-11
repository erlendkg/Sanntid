

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

#define BACKLOG 10

int printmsg(char *str) {


  int MsgType, MsgEl, MsgFloor, MsgButton;

  MsgType = str[1] - '0';
  printf("Message type: %d \n", MsgType);

  if (MsgType == 1) {

    sscanf(str, "<1E%dF%d>", &MsgEl, &MsgFloor);
    printf("Elevator %i\nIs on floor %i\n\n", MsgEl, MsgFloor);
  }

  else if (MsgType == 2) {

    sscanf(str, "<2E%dBT%dF%d>", &MsgEl, &MsgButton, &MsgFloor);
    printf("Elevator %i\nButton to floor %i\nButton type: %i\n\n", MsgEl, MsgFloor, MsgButton);

  }


}


int main(void)
{

  int sockfd, new_fd;
  int rv;
  int yes = 1;
  socklen_t sin_size;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;



  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; //

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if(listen(sockfd, BACKLOG) == -1 ) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while(1) {
    sin_size = sizeof their_addr;
    printf("lal\n");
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    printf("lal\n");
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    printf("got connection\n");


    char msg[100];
    int len = strlen(msg);
    int bytesrec;

  int MsgType, MsgEl, MsgFloor, MsgButton;



      while(1){
        bytesrec = 0;
        bytesrec = recv(new_fd, msg, len-1, 0);

        if (bytesrec != -1){
          msg[bytesrec] = '\0';

          printf("The message is: %s\n\n",msg);

          printmsg(msg);

       }
      }
    }
}
