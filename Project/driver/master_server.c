#include "Network.h"

#define PORT "3492"

#define BACKLOG 10

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
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    printf("got connection\n");

    char *buf ="Hello world d sdsjkfnd!K\0";
    int len = strlen(buf);


    sendall(new_fd, buf, &len);
  }

}
