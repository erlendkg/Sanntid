#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


void main() {

  char* str;
  int Elevator, Floor;

  str = "<E7F22222>";

  sscanf(str, "<E%dF%d>", &Elevator, &Floor);

  printf("Pesant #%d, Move your ass to coal sector %d\n", Elevator, Floor);

}
