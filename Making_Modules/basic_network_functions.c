#include "basic_network_functions.h"

int initialize_server_socket() {

  int rv, server_socket;
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
    if((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("server: socket");
        continue;
    }

    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      return -1;
    }

    if(bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(server_socket);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  return server_socket;
}

int initialize_client_socket(char const* server_ip) {

    int client_socket;
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
          if ((client_socket = socket(p->ai_family, p->ai_socktype,
                  p->ai_protocol)) == -1) {
              perror("client: socket");
              continue;
          }
          if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
              freeaddrinfo(servinfo);
              close(client_socket);
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
      return client_socket;
  }

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

int accept_client(Network_status *net_status) {
  int new_socket;
  int i;
  struct sockaddr_in address;
  int addrlen = sizeof address;

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
}

int send_all(int recipient_socket, char *buf, int *len) {
  int total = 0;
  int bytesleft = *len;
  int n;

  while(total < *len) {
    n = send(recipient_socket, buf+total, bytesleft,0);
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

char* recieve_message(int senders_socket) {

}
