#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

typedef struct{

  int Num;
  int DesiredFloor;
  int CurrentFloor;
  int Done;

} El_Status;

int sendall(int s, char *buf, int *len);
