#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

#define HOSTNAME "10.20.55.255"
#define PORTNAME "13539"

int main(){



struct addrinfo hints;

memset(&hints,0,sizeof(hints));

char *msg = "1";
int len = strlen(msg);

hints.ai_family=AF_UNSPEC;
hints.ai_socktype=SOCK_DGRAM;
struct addrinfo* res=0;


int err=getaddrinfo(HOSTNAME,PORTNAME,&hints,&res);
  if (err!=0)
    {
      printf("failed to resolve remote socket address (err=%d)",err);
    }
printf("SEND Ferdig med getaddrinfo\n");


int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);

  if (fd==-1)
    {
      printf("send: %s",strerror(errno));
    }
printf("SEND Ferdig med socket\n");

int enabled = 1;
setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled));

freeaddrinfo(res);

struct sockaddr_storage their_addr;

int addr_len = sizeof their_addr;
int numbytes;

system("./Rec");

while(1){
  if ((numbytes = sendto(fd, msg, len , 0, res->ai_addr, res->ai_addrlen)) == -1) {
      perror("sendto");
      exit(1);

  }
  printf("hei\n");

}
}
