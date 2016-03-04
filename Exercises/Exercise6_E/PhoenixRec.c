#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

#define HOSTNAME "10.20.52.36"
#define PORTNAME "13539"

int main(){

struct addrinfo hints;

memset(&hints,0,sizeof(hints));


hints.ai_family=AF_UNSPEC;
hints.ai_socktype=SOCK_DGRAM;
hints.ai_protocol=0;
hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
struct addrinfo* res=0;

int err=getaddrinfo(NULL,PORTNAME,&hints,&res);

  if (err!=0)
    {
    printf("failed to resolve local socket address (err=%d)",err);
    }
  printf("REC Ferdig med getaddrinfo\n");


int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);

  if (fd==-1)
    {
      printf("%s",strerror(errno));
    }
  printf("REC Ferdig med socket\n");

  int enabled = 1;
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled));

if (bind(fd,res->ai_addr,res->ai_addrlen)==-1)
  {
      printf("rec: %s",strerror(errno));
  }
printf("REC Ferdig med bind\n");

freeaddrinfo(res);

struct sockaddr_storage their_addr;
char *buf[10];
int addr_len = sizeof their_addr;
int numbytes;
  if ((numbytes = recvfrom(fd, buf, sizeof(buf)-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
      perror("recvfrom");
      exit(1);
  }

printf("%s",buf);

}
