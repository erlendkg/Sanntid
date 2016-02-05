
void elev_go_to_floor()
{
  while(1){

    if (elev_get_floor_sensor_signal() != -1) {
      E.CurrentFloor = elev_get_floor_sensor_signal();
    }

    if (E.CurrentFloor > E.DesiredFloor){
      elev_set_motor_direction(DIRN_UP);

    }  else if (E.CurrentFloor > E.DesiredFloor){
      elev_set_motor_direction(DIRN_DOWN);

    }  else if (E.CurrentFloor == E.DesiredFloor){
      elev_set_motor_direction(DIRN_STOP);
      E.Available = 1;
      elev_hold_door_open(DOOR_OPEN_TIME);
    }
  }
}
