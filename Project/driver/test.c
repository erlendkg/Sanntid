
#include <assert.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "queue_functions.h"



int main(){


  test_queue_functions();
  return 1;
}
