#include <stdlib.h>
#include <stdio.h>


int printmsg(char *str, int *msgType, int *elevatorNumber, int *buttonType, int * elevatorFloor) {


  int tempMsgType = 0, tempMsgEl = 0, tempMsgFloor = 0, tempMsgButton = 0;

  tempMsgType = str[1] - '0';
  printf("Message type: %d \n", tempMsgType);

  if (tempMsgType == 1) {

    sscanf(str, "<1E%dF%d>", &tempMsgEl, &tempMsgFloor);
    printf("Elevator %i\nIs on floor %i\n\n", tempMsgEl, tempMsgFloor);
  }

  else if (tempMsgType == 2) {

    sscanf(str, "<2E%dBT%dF%d>", &tempMsgEl, &tempMsgButton, &tempMsgFloor);
    printf("Elevator %i\nButton to floor %i\nButton type: %i\n\n", tempMsgEl, tempMsgFloor, tempMsgButton);

  }

  *msgType = tempMsgType;
  *elevatorNumber = tempMsgEl;
  *buttonType = tempMsgButton;
  *elevatorFloor = tempMsgFloor;
}


int main() {

  char str[10] = "<2E2BT1F1>";
  int msgType, msgElevatorNumber, msgButtonType, msgElevatorFloor;

  printmsg(str, &msgType, &msgElevatorNumber, &msgButtonType, &msgElevatorFloor);

    printf("MsgType %i\nElevator %i\nButton to floor %i\nButton type: %i\n\n",msgType, msgElevatorNumber, msgElevatorFloor, msgButtonType);
}
