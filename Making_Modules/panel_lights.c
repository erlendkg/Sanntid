#include "panel_lights.h"


int initialize_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
        int floor, button_type;

        for (floor = 0; floor < N_FLOORS; floor++) {
                for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
                        if((floor == 0) && (button_type == 1)) {
                                lamp_matrix[floor][button_type] = 2;
                        }
                        else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
                                lamp_matrix[floor][button_type] = 2;
                        } else {
                                lamp_matrix[floor][button_type] = 0;
                        }
                }
        }
        return 0;
}

int update_lamp_matrix(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int floor, int button_type, int light_status) {
        lamp_matrix[floor][button_type] = light_status;
        return 0;
}

int update_panel_lights(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
        int floor, button_type;

        for (floor = 0; floor < N_FLOORS; floor++) {
                for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
                        if((floor == 0) && (button_type == 1)) {
                                lamp_matrix[floor][button_type] = 2;
                        } else if((floor == (N_FLOORS-1)) && (button_type == 0)) {
                                lamp_matrix[floor][button_type] = 2;
                        } else {
                                update_button_lights(button_type, floor, lamp_matrix[floor][button_type]);
                        }
                }
        }
        return 0;
}

int bundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS]) {
        int floor, button_type, i;
        char current_char;
        int current_value;
        i = 1;
        message[0] = '3';

        for (button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
                for(floor = 0; floor < N_FLOORS; floor++) {
                        current_value = lamp_matrix[floor][button_type];
                        current_char = current_value + '0';
                        message[i] = current_char;
                        i++;
                }
        }
        message[i] = '\0';
        return 0;
}

int unbundle_lamp_matrix(char message[sizeof(int)*N_FLOORS*4], int lamp_matrix[N_FLOORS][2]) {
        int floor, button_type, i;
        char current_char;
        i = 1;

        for(button_type = 0; button_type < EXTERNAL_BUTTONS; button_type++) {
                for (floor = 0; floor < N_FLOORS; floor++) {
                        current_char = message[i];
                        lamp_matrix[floor][button_type] = current_char - '0';
                        i++;
                }
        }
        return 0;
}

void convert_lamp_matrix_to_orders(int lamp_matrix[N_FLOORS][EXTERNAL_BUTTONS], int elevator_queue[10]){

        int matrix_row_counter, matrix_column_counter, queue_counter;

        for (matrix_row_counter = 0; matrix_row_counter < N_FLOORS; matrix_row_counter++) {
                for (matrix_column_counter = 0; matrix_column_counter < 2; matrix_column_counter++) {

                        if ((matrix_row_counter == 0 && matrix_column_counter == 1) || (matrix_row_counter == N_FLOORS-1 && matrix_column_counter == 0 )) {
                                continue;
                        }
                        for (queue_counter = 0; queue_counter < 10; queue_counter++) {
                                if(lamp_matrix[matrix_row_counter][matrix_column_counter] == elevator_queue[queue_counter]) {
                                        continue;
                                }
                                else if(elevator_queue[queue_counter] == 0) {
                                        printf("stealing value from lamp[%d][%d]\n", matrix_row_counter, matrix_column_counter);
                                        printf("inserting %d on location %d\n", matrix_row_counter + 1, queue_counter);
                                        insert_item(elevator_queue, queue_counter, matrix_row_counter + 1);
                                        break;
                                }
                        }
                }
        }
        int j, k;
        for (j = 0; j<N_FLOORS; j++) {
                printf("\n");
                for (k = 0; k<2; k++) {
                        printf("|%d|", lamp_matrix[j][k]);
                }
        }
        printf("\n");

}

void* thread_update_lights() {
        int current_floor;

        while(1) {
                current_floor = return_current_floor();
                update_panel_lights(lamp_matrix);
                if(current_floor != -1) {
                        update_floor_indicator((current_floor)-1);
                }
        }
}
