
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

char *get_string(int msgType){

char *msg = (char*) malloc(10 * sizeof(int));

  if (msgType == 1){
    sprintf(msg, "<%dE%dF%d>\0", msgType, 12, 3);
  }
  else if(msgType == 2){
    sprintf(msg, "<%dE%dF%d>\0", msgType, 12, 3);
  }
  return msg;
}

int main(){

  char *msg;
  msg = get_string(1);
  printf("%s\n", msg);
  free(msg);
  return;
}
