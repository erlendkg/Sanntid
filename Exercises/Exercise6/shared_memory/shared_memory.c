#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char const *argv[]) {

  key_t shmkey;
  int shmid;
  char* shm;
  char* msg;
  char c;
  int i;
  int j = 1;
  int temp = 1;
  int r;

  r = (rand() % 4) + 1;

  int status = system("./shmem");
  shmkey = 5678;

  if ((shmid = shmget(shmkey, 1024, IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  if ((shm = shmat(shmid, NULL, 0)) == (char *) (-1)) {
       perror("shmat");
       exit(1);
   }

   while(1) {
     *shm = j;
     j++;
     if(j == r){
       exit(1);
     }
     printf("%d\n", *shm);
     sleep(1);
   }
   return 0;
 }
