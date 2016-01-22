#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

int main(){

struct addrinfo hints;
struct addrinfo *servinfo;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("10.20.55.255", "30000", &hints, &serverinfo) =! 0){
  printf("failed to connect\n");
}
else{
  printf("succeded to get adress info\n")
}
