#include "Network.h"
#include "elev.h"



int main_server () {

  Network_status net_status;

  pthread_t listen_for_clients;

  memset(&net_status, 0, sizeof net_status);

  net_status.server_socket = initialize_server_socket();



  pthread_create(&listen_for_clients, NULL, thread_listen_for_clients, (void *) &net_status);
  pthread_join(listen_for_clients, NULL);



  return 0;

}

void *thread_listen_for_clients(void *net_status) {

  Network_status* cast_net_status = (Network_status *) net_status;
  int incoming_connection;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;

  while(1) {

    printf("server: waiting for connections...\n");
  if(listen(cast_net_status->server_socket, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
    printf("server: waiting for connections...\n");

    sin_size = sizeof their_addr;

    incoming_connection = accept(cast_net_status->server_socket, (struct sockaddr*) &their_addr, &sin_size);
    if (incoming_connection == -1) {
      perror("accept");
      continue;
    }
  }



}



int initialize_server_socket() {

  int rv, sockfd;
  int yes = 1;
  struct addrinfo hints, *servinfo, *p;


  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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

  return sockfd;
}



int sendall(int s, char *buf, int *len)
{
  int total = 0;
  int bytesleft = *len;
  int n;

  while(total < *len) {
    n = send(s, buf+total, bytesleft,0);
    if (n == -1) {
      printf("Failed to send\n");
      break;
    }
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
    if ((bytes_received = recv(*(int*)sockfd, Server_reply, length, 0)) != 0)
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

    sprintf(msg, "<%dE%dBT%dF%d>", msgType, E.Num, E.ButtonType, E.ButtonFloor);

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

int initialize_listen(char* server_ip){

  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo("78.91.2.218", PORT, &hints, &servinfo)) != 0) {
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
    return sockfd;
}
