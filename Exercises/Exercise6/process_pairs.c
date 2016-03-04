#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<time.h>

#define COM_PORT "4951"
#define MAXBUFLEN 100


int main(int argc, char const *argv[]) {
  int sockfd, status;
  struct addrinfo hints, *serverinfo, *p;
  struct sockaddr_storage their_addr;
  int rv, j;
  int numbytes;
  char buf[MAXBUFLEN];
  socklen_t addr_len;
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  if(atoi(argv[1]) == 2) {
  sleep(5);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if((rv = getaddrinfo(NULL, COM_PORT, &hints, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = serverinfo; p !=NULL; p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("listener:socket");
      continue;
    }

setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("listener:bind");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener:failed to bind to socket\n");
  }

  freeaddrinfo(serverinfo);



  while(1) {


    addr_len = sizeof their_addr;

    if((numbytes = recvfrom(sockfd,buf, MAXBUFLEN-1, 0,(struct sockaddr*)&their_addr, &addr_len)) == -1 ) {
      perror("recvfrom");
      printf("Timeout of process one\n");

      close(sockfd);

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_DGRAM;

      if((rv = getaddrinfo("127.0.0.1", COM_PORT, &hints, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
      }

      for(p = serverinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
          perror("talker:socket");
          continue;
        }
        break;
      }

      if(p == NULL) {
        fprintf(stderr, "talker: failed to create socket");
        return 2;
      }

      status = system("gnome-terminal -x bash -c './process_pairs 2'");

      while(1) {
        printf("process one:%d\n",j);

        sprintf(buf, "%d", j);

        if((numbytes = sendto(sockfd, buf , strlen(buf), 0, p->ai_addr, p->ai_addrlen)) == -1) {
          perror("talker: sendto");
          exit(1);
        }

        j++;
        if(j > 4) {
          j = 1;
        }
        sleep(1);
      }

    }


    j = atoi(buf);
    printf("process two:%d\n",j);
    sleep(1);


}

  printf( "%s\n",buf);
  close(sockfd);


} else if(atoi(argv[1]) == 1) {

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if((rv = getaddrinfo("127.0.0.1", COM_PORT, &hints, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = serverinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("talker:socket");
      continue;
    }
    break;
  }

  if(p == NULL) {
    fprintf(stderr, "talker: failed to create socket");
    return 2;
  }
  j = 1;


  status = system("gnome-terminal -x bash -c './process_pairs 2'");

  while(1) {

  sprintf(buf, "%d", j);
  if((numbytes = sendto(sockfd, buf , strlen(buf), 0, p->ai_addr, p->ai_addrlen)) == -1) {
    perror("talker: sendto");
    exit(1);
  }

  printf("%d\n",j);
  j++;

  if(j > 4)
  {
    j = 1;
  }

  sleep(1);
  }

  freeaddrinfo(serverinfo);

  close(sockfd);


}


}
