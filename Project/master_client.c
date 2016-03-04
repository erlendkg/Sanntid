#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490"
#define IP "78.91.70.37"
#define MAXDATASIZE 100

int main() {

  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;


  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(IP, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if(p == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
    }

    printf("client: connecting\n");

    freeaddrinfo(servinfo);

    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(1);
    }

    printf("recieved: %s", buf);



    buf[numbytes] = '\0';


    printf("recieved: %s", buf);


    close(sockfd);

    return 0;


}
