#include <stdlib.h>
#include <stdio.h>


int printmsg(char *str) {


  int MsgType, MsgEl, MsgFloor, MsgButton;

  MsgType = str[1] - '0';
  printf("Message type: %d \n", MsgType);

  if (MsgType == 1) {

    sscanf(str, "<1E%dF%d>", &MsgEl, &MsgFloor);
    printf("Elevator %i\nIs on floor %i\n\n", MsgEl, MsgFloor);
  }

  else if (MsgType == 2) {

    sscanf(str, "<2E%dBT%dF%d>", &MsgEl, &MsgButton, &MsgFloor);
    printf("Elevator %i\nButton to floor %i\nButton type: %i\n\n", MsgEl, MsgFloor, MsgButton);

  }


}


int main() {
  char str[10] = "<1E2F1>";

  printmsg(str);


}
