#include "Network.h"

int sendall(int s, char *buf, int *len)
{
  int total = 0;
  int bytesleft = *len;
  int n;

  while(total < *len) {
    n = send(s, buf+total, bytesleft,0);
    if (n == -1) {break;}
    total += n;
    bytesleft -=n;
  }

  *len = total;

  return n==-1?-1:0;
}

void* listen_for_orders(void *sockfd)
{
  int bytes_received;
  int length = 32;
  char Server_reply[32];
  int MsgEl, MsgFloor;

  while(1)
  {
    if (bytes_received = recv(*(int*)sockfd, Server_reply, length, 0) != 0)
      {
        sscanf(Server_reply, "<E%dF%d>", &MsgEl, &MsgFloor);

        if (MsgEl ==  E.Num)
        {
          E.DesiredFloor = MsgFloor;
          E.TaskComplete = 0;
          printf("Pesant #%d, Move your ass to coal sector %d\n", MsgEl, MsgFloor);

        }
        else
        {
          printf("%d\n",bytes_received);
        }
      }
  }
}


char *get_string(int msgType){

char *msg = (char*) malloc(10 * sizeof(int));

  if (msgType == 1){
    sprintf(msg, "<%dE%dF%d>", msgType, E.Num, E.CurrentFloor);
  }
  else if(msgType == 2){
    sprintf(msg, "<%dE%dF%d>", msgType, E.ButtonType, E.ButtonFloor);
  }
  return msg;
}

//SendMsg(int socket, int msgType), msgType 1 = job done, msgType 2 = button press.
void* send_message(void *sockfd){

  int msgType;
  char *msg;
  int len;
  int bytes_sent;

  while (1) {
    if(E.TaskComplete == 1 || E.ButtonClick ==1 ){

      if (E.TaskComplete == 1) {
        msgType = 1;
      }
      else if (E.ButtonClick == 1) {
        msgType = 2;
        E.ButtonClick = 0;
      }
      msg = get_string(msgType);
      len = strlen(msg);
      bytes_sent = send(*(int *)sockfd, msg, len, 0);
      free(msg);
    }
  }
}

int initialize_listen(){

struct addrinfo hints;
struct addrinfo *servinfo;
pthread_t receive, send;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//hints.ai_flags = AI_PASSIVE;

if (getaddrinfo("78.91.68.70", "312326", &hints, &servinfo) != 0){
  printf("failed to connect\n");
}
else
{
  printf("succeded to get getaddrinfo for: 78.91.69.119\n");
}

/* Here we create sockets*/
int sockfd;

sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

printf("sock %d\n", sockfd);

if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
{
  printf("connection failed+\n");
  return 0;
}
else
{
  printf("connection successful\n");
}

printf("entering thread\n");
pthread_create(&receive, NULL, listen_for_orders, &sockfd);
pthread_create(&send, NULL, send_message, &sockfd);

pthread_join(receive, NULL);
close(sockfd);

return 0;


}
