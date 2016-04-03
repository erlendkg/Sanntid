#include "Network.h"
#include "elev.h"

int main_server() {
  Network_status *net_status = malloc(sizeof(Network_status));
  pthread_t listen_for_clients;

  net_status->server_socket = initialize_server_socket();
  pthread_mutex_init(&net_stat_lock, NULL);


  pthread_create(&listen_for_clients, NULL, thread_listen_for_clients, (void *) net_status);

  pthread_join(listen_for_clients, NULL);


  return 0;
}

int main_client(char const *server_ip) {

  int server_socket;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));

  elev_init();
  elev_set_motor_direction(DIRN_STOP);
  run_down_until_hit_floor();

  this_elevator->current_floor = elev_get_floor_sensor_signal();

  this_elevator->is_busy = 0;

  if((server_socket = initialize_client_socket(server_ip)) == 2) {
    printf("Failed to create socket\n");
    this_elevator->is_connected_to_network = 0;
  }


  while(1) {

    if ((this_elevator->is_connected_to_network) == 0) {
      printf("No network connection could be established\n");
      printf("Currently Running in single elevator mode\n");
      single_elevator_mode(this_elevator, &server_socket, server_ip);
      this_elevator->is_connected_to_network = 1;
      printf("Network connection established\n");
      printf("Switching to network mode\n");
    }


    if (this_elevator->is_busy == 0 && this_elevator->is_connected_to_network == 1)
    {
      //Lytter etter ordre
      //Hvis får ordre sender til server
      //Mottar ordre
      //Utfører Oppgave
      //Oppgave utført
    }
  }

}

int wait_for_orders_from_server(int server_socket) {
  return 0;

}


void *thread_listen_for_clients(void *net_status) {

  Network_status* my_net_status = (Network_status *) net_status;
  int incoming_connection;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;

  while(1) {

  if(listen(my_net_status->server_socket, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

    printf("server: waiting for connections...\n");

    sin_size = sizeof their_addr;

    incoming_connection = accept(my_net_status->server_socket, (struct sockaddr*) &their_addr, &sin_size);
    if (incoming_connection == -1) {
      perror("accept");
      continue;
    }

    pthread_mutex_lock(&net_stat_lock);
    my_net_status->active_connections += 1;
    my_net_status->client_sockets[my_net_status->active_connections] = incoming_connection;
    close(incoming_connection);
    pthread_mutex_unlock(&net_stat_lock);


    printf("Connection accepted\n");
    printf("Number of active connections: %d\n", my_net_status->active_connections);


  }
}

void *thread_maintain_active_connections(void *net_status) {
    Network_status* cast_net_status = (Network_status *) net_status;

}

void *thread_recieve_orders_from_elevators(void *net_status) {
    Network_status *my_net_status = (Network_status *) net_status;

    int bytes_received, i;
    int max_data_size = sizeof(Elev_info);
    char buf[max_data_size];

    while(1){
      for(i = 0; i = (my_net_status->active_connections -1); i++) {
        bytes_received = recv(my_net_status->client_sockets[i], buf, max_data_size -1, 0);
        update_elevator_status();
        memset(&buf, 0, sizeof(buf));
    }
  }



}

int update_elevator_status() {
  return 1;
}


int initialize_client_socket(char const* server_ip) {

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(server_ip, PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 2;
    }
    for(p = servinfo; p != NULL; p = p->ai_next) {
          if ((sockfd = socket(p->ai_family, p->ai_socktype,
                  p->ai_protocol)) == -1) {
              perror("client: socket");
              continue;
          }

          if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
              freeaddrinfo(servinfo);
              close(sockfd);
              return 2;
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
      return -1;
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

/*void* listen_for_orders(void *sockfd) {
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
*
char *get_string(int msgType) {

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
void* send_message(void *sockfd) {

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
*/
