#include "Network.h"
#include "elev.h"
#include "QueueModule/queue_functions.h"

int add_all_socks_to_fdlist(fd_set *readfds, Network_status *net_status) {

  int i;
  int max_sd, sd;

  FD_ZERO(readfds);

  FD_SET(net_status->master_socket, readfds);
  max_sd = net_status->master_socket;

  for (i = 0; i < MAX_NUMBER_OF_ELEVS; i++) {
    sd = net_status->client_sockets[i];

    if(sd > 0) {
      FD_SET(sd, readfds);
    }
    if(sd > max_sd) {
      max_sd = sd;
    }
  }

  return max_sd;
}

<<<<<<< HEAD
int accept_client(Network_status *net_status, Elevator_data dataElevators[MAX_NUMBER_OF_ELEVATORS]) {
=======
int accept_clinet(Network_status *net_status) {
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0
  int new_socket;
  int i;
  struct sockaddr_in address;
  int addrlen = sizeof address;
<<<<<<< HEAD
  int queueNumber;
=======
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0

  if((new_socket = accept(net_status->master_socket, (struct sockaddr *) &address, (socklen_t *)&addrlen)) <0) {
    perror("accept");
    return -1;
  }
  printf("New elevator connected, socket fd is %d, ip is: %s, port %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

  for (i = 0; i < MAX_NUMBER_OF_ELEVS; i++) {
    if( net_status->client_sockets[i] == 0 ) {
          net_status->client_sockets[i] = new_socket;
          net_status->active_connections += 1;
          printf("Adding to list of sockets as %d\n" , i);
          break;
        }
    }
<<<<<<< HEAD

    //QUEUESTUFF2*******************************************************
    queueNumber = assignNumberToNewElevator(dataElevators, dataElevators[99].lengthOfElevatorArray);
    activateSingleQueue(dataElevators, queueNumber);
    dataElevators[99].lengthOfElevatorArray ++;
    dataElevators[queueNumber].socket = new_socket;
    send(new_socket, &queueNumber, sizeof(queueNumber), 0);

    //QUEUESTUFF2*******************************************************
}


void* listen_for_incoming_connections(void* net_status) {
  Network_status *my_net_status = (Network_status *) net_status;
  struct sockaddr_in address;
  int max_sd, sd, i, activity;
  int valread;
=======
}

int read_message_from_client(int client_socket, char* buf) {

}

void *listen_for_incoming_connections(void* net_status) {
  Network_status *my_net_status = (Network_status *) net_status;
  struct sockaddr_in address;
  int max_sd, sd, activity;
  int new_socket;
  int valread, i;
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0
  fd_set readfds;
  int addrlen = sizeof address;

  char buffer[512];

  //QUEUESTUFF1*******************************************************
  char returnMessage[512];
  char *temp;
  Elevator_data dataElevators[MAX_NUMBER_OF_ELEVATORS];
  initiateQueues(dataElevators);


  pthread_t sendOrderToElevator;
  pthread_create(&sendOrderToElevator, NULL, thread_sendOrdersToIdleElevators, dataElevators);
  // //QUEUESTUFF1*******************************************************



  if(listen(my_net_status->master_socket, MAX_NUMBER_OF_ELEVS) < 0) {
    perror("listen");
    exit(1);
  }

  while(1) {

<<<<<<< HEAD


    max_sd = add_all_socks_to_fdlist(&readfds, net_status);
=======
    max_sd = add_all_socks_to_fdlist(&readfds, my_net_status);
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0

    activity = select(max_sd +1, &readfds, NULL, NULL, NULL);

    if(FD_ISSET(my_net_status->master_socket, &readfds)) {
<<<<<<< HEAD
        accept_client(my_net_status, dataElevators);

=======

      if (accept_clinet(my_net_status) == -1) {
        printf("Failed to accept client\n");
      }
        // //QUEUESTUFF2*******************************************************
        // printf("prøver å sende\n");
        // queueNumber = assignNumberToNewElevator(dataElevators, lengthOfElevatorArray);
        // activateSingleQueue(dataElevators, queueNumber);
        // lengthOfElevatorArray ++;
        // printf("prøver å sende her er queue number : %d\n", queueNumber);
        // printf("prøver å sendLALDSALDSe\n");
        // send(new_socket, queueNumber, 32, 0);
        //   printf("prøver å sendTT\n");
        // //QUEUESTUFF2*******************************************************
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0
    }
    for(i = 0; i < MAX_NUMBER_OF_ELEVS; i++) {
      sd = my_net_status->client_sockets[i];

      if(FD_ISSET(sd, &readfds)) {
        if((valread = read(sd, &buffer, sizeof(buffer))) == 0) {
          getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
          printf("Client disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse

          disableElevatorAndDistributeQueueToOtherElevators(dataElevators, sd);

          close(sd);
          my_net_status->client_sockets[i] = 0;
          my_net_status->active_connections -= 1;





        } else {
          //QUEUESTUFF3*******************************************************
            printf("\n\nMottatt meldingen %s på socket %d\n",buffer, sd );

            temp = actOnMessageFromMaster(dataElevators, buffer, dataElevators[99].lengthOfElevatorArray);

            strcpy(returnMessage, temp);

              printf("Returnmessage = %s\n", returnMessage);



            if (strcmp(returnMessage, "2") == 0) {

              printf("Order has been added to queue\n");

            }
            else if (strcmp(returnMessage, "0") == 0){

              printf("Den var null gitt\n");

            }
            else if (strchr(returnMessage, '<') != 0){

              printf("her sender jeg: %s \n", returnMessage);
              send(sd, returnMessage, sizeof(returnMessage), 0); //HER SKJER EN SEG FAULT
            }

            printf("lengthOfElevArray: %d", dataElevators[99].lengthOfElevatorArray);

            for(int i = 0; i < dataElevators[99].lengthOfElevatorArray; i++){
              if (dataElevators[i].status != -1){
                printf("\n\n******************************************\n");
                printf("Queue number: %d\t", i);
                printf("Status: %d\n", dataElevators[i].status);
                print_queue(dataElevators[i].queue);
                printf("******************************************\n\n");
              }
            }


            memset(buffer, 0, sizeof(buffer));
            memset(returnMessage, 0, sizeof(returnMessage));

          //QUEUESTUFF3*******************************************************
          }
        }
      }
    }

  return NULL;
}

int main_server() {
  Network_status *net_status = malloc(sizeof(Network_status));
  pthread_t listen_for_clients;



  net_status->master_socket = initialize_server_socket();
  pthread_mutex_init(&net_stat_lock, NULL);

  pthread_create(&listen_for_clients, NULL, listen_for_incoming_connections, (void *) net_status);


  pthread_join(listen_for_clients, NULL);

  return 0;
}

int main_client(char const *server_ip) {

  int server_socket, networkModeActive = 0;
  Elev_info *this_elevator = malloc(sizeof(Elev_info));

  pthread_mutex_init(&elev_info_lock, NULL);

  elev_init();
  elev_set_motor_direction(DIRN_STOP);

  this_elevator->current_floor = elev_get_floor_sensor_signal();

  this_elevator->is_busy = 0;
  this_elevator->is_connected_to_network = 0;


  while(1) {

    if ((this_elevator->is_connected_to_network) == 0) {

      single_elevator_mode(this_elevator, &server_socket, server_ip);
      this_elevator->is_connected_to_network = 1;

    }

    if (this_elevator->is_connected_to_network == 1)
    {

      printf("Elevator is now i network mode");
      if (networkModeActive == 0){
        network_elevator_mode(this_elevator, server_socket, server_ip);
        networkModeActive = 1;
      }
    }
  }
}

<<<<<<< HEAD


=======
int wait_for_orders_from_server(int server_socket) {
  return 0;
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0


<<<<<<< HEAD
void* thread_sendOrdersToIdleElevators(void *elevatorInfo){
    printf("Entering send to elevators thread\n");
  Elevator_data *myElevatorInfo = (Elevator_data *)elevatorInfo;
  char messageToElevator[512];
  int i;

  while(1){

    for( i = 0; i <= myElevatorInfo[99].lengthOfElevatorArray; i++ ){

      if (myElevatorInfo[i].status == 2 && myElevatorInfo[i].queue[0] != 0){
=======
void *thread_recieve_orders_from_elevators(void *net_status) {
    Network_status *my_net_status = (Network_status *) net_status;
>>>>>>> 17c73c19ba574cc3018f60c5ef4de937070271c0

        printf("First pos in queue is: %d: \n", myElevatorInfo[i].queue[0]);

        sprintf(messageToElevator, "<1E%dF%d>", i, myElevatorInfo[i].queue[0]);

        printf("Sending msg to elev %d: %s, on socket %d\n", i, messageToElevator, myElevatorInfo[i].socket);

        send(myElevatorInfo[i].socket, messageToElevator, sizeof(messageToElevator), 0);

        updateElevatorStruct(myElevatorInfo[i].queue, &myElevatorInfo[i].status, &myElevatorInfo[i].queueSize, myElevatorInfo[i].currentFloor);

      }
    }
  }

  return NULL;
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
