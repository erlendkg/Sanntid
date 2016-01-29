#include "headerClient.h"

El_Status E;

void function()
{
  char message[32];
  int bytes_received;
  int length = 32;
  char Server_reply[32];
  int MsgEl, MsgFloor;

  strcpy( Server_reply, "<E2F98>");




    if (1)
      {
        sscanf(Server_reply, "<E%dF%d>", &MsgEl, &MsgFloor);

        if (MsgEl ==  E.Num)
        {
          E.DesiredFloor = MsgFloor;
          E.Done = 0;
          printf("Pesant #%d, Move your ass to coal sector %d\n\n\n\n", MsgEl, MsgFloor);

          printf("This print is from a the struct:\n\n");
          printf("ElevatorNumber: %d\n", E.Num);
          printf("DesiredFloor: %d\n", E.DesiredFloor);
          printf("CurrentFloor: %d\n", E.Num);
          printf("Done: %d\n", E.Done);
        }
      }
}
void main(){


  E.Num = 2;
  E.CurrentFloor = 4;

  function();



}
