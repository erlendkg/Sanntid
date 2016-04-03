
#include <stdio.h>
#include <stdlib.h>

#include "elev.h"

#include <unistd.h>
#include<pthread.h>
#include "Network.h"


 int main(int argc, char const *argv[]) {

   if (atoi(argv[1]) == 1) {
     main_server();
   }
   else if (atoi(argv[1]) == 2 ) {
     if(argc != 3) {
       printf("Not enough input arguments\n");
       exit(1);
     }

     main_client(argv[2]);
   }
}
