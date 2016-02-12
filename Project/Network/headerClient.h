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

typedef struct{

  int Num;
  int DesiredFloor;
  int CurrentFloor;
  int Done;

} El_Status;
