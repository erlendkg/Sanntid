#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BACKLOG 10

char *smsg = "Hide yo wife!";
char *recmsg;
int len, bytes_sent;
int main(int argc, char const *argv[]) {

struct addrinfo hints;
struct addrinfo *serverinfo;
int s;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

if(getaddrinfo("78.91.71.255", "34933", &hints, &serverinfo) != 0)
{
  printf("getaddrinfo error\n");
  exit(0);
}
else
{
  printf("Yes!\n");
}

s = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);

if(bind(s, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
{
  printf("Could not in the darkness bind them\n");
}
else
{
  printf("Everyone bound in darkness\n");
}

if(connect(s, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
{
  printf("I am a disgrace to all\n");
}
else
{
  printf("I has connection\n");
}
len = strlen(smsg);
bytes_sent =send(s,smsg,len,0);
recv(s, recmsg, len,0);

printf("Hello");


close(s);





  return 0;
}
