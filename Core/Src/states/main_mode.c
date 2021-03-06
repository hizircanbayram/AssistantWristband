/*
 * main_mode.c
 *
 *  Created on: 28 Haz 2020
 *      Author: hizirov
 */


#include "states/main_mode.h"


extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart6;

extern uint32_t current_step;
extern uint32_t step_num;
extern uint32_t elapsed_time;
extern state current_state;
extern state mode_state;

extern Queue* window_acc_y;
extern uint32_t possible_step;
extern uint32_t elapsed_time;
extern uint8_t walk_permission;


void mainScreen(const CalorieInfo *person_cal_info) {
	char text1[25] = { 0 };
	sprintf(text1, "  %ld / %ld step", current_step, step_num);
	ST7735_WriteString(0, 50, text1, TEXT_FONT_MAIN_MODE, TEXT_COLOR_MAIN_MODE, TEXT_BACKGROUND_COLOR_MAIN_MODE);
	uint32_t eta_time = ((step_num - current_step) * elapsed_time) / current_step;
	if (current_step >= step_num) {
		ST7735_FillScreen(BACKGROUND_COLOR_MAIN_MODE);
		current_state = final_mode;
		return;
	}
	uint32_t hour = 0, min = 0, sec = 0;
	convertSecToTimeStamp(eta_time, &hour, &min, &sec);
	char text2[25] = { 0 };
	sprintf(text2, "   ETA:  %ld:%ld:%ld ", hour, min, sec); // black character at the end of the string here
	// is important. for example 59 sec is displayed as 0:0:59 at the format above. as the seconds decrease,
	// assume it will be 49, 39, .... even going further, it will be 9 seconds. in this situation
	// what we expect would be 0:0:9. however, since before 9 seconds, there was 19 seconds, the lsb digit
	// of 19, which is 9, stays on the screen. thus we see 0:0:99 instead of 0:0:9. to avoid that we use
	// extra black so that it covers the extra digit coming from early calculations.
	ST7735_WriteString(0, 100, text2, TEXT_FONT_MAIN_MODE, TEXT_COLOR_MAIN_MODE, TEXT_BACKGROUND_COLOR_MAIN_MODE);
	checkStepDetection(eta_time);
}


void convertSecToTimeStamp(uint32_t elapsed_time, uint32_t* hour_ptr, uint32_t* min_ptr, uint32_t* sec_ptr) {
	*hour_ptr = elapsed_time / 3600;
	*min_ptr = (elapsed_time - (3600 * *hour_ptr)) / 60;
	*sec_ptr = elapsed_time - (3600 * *hour_ptr) - (*min_ptr * 60);
}


void checkStepDetection(uint32_t eta_time) {
	AccData acc_3d;
	HAL_StatusTypeDef is_mma8452q_read_ok = mma8452qRead(&hi2c1, 0x00, 7, acc_3d.acc_info);
	if (is_mma8452q_read_ok == HAL_OK) {
		getAccXYZ(&acc_3d);
		char acc_message[100] = { 0 };
		int x_acc = acc_3d.x_acc ;
		int y_acc = acc_3d.y_acc ;
		int z_acc = acc_3d.z_acc ;

		enqueue(window_acc_y, y_acc);
		if (isFull(window_acc_y)) {
			dequeue(window_acc_y);
			if ((window_acc_y->array[2] - window_acc_y->array[0] < -40) &&
				(window_acc_y->array[4] - window_acc_y->array[2] > 40)) {
				if (possible_step == 0 && walk_permission) {
					possible_step = 1;
					walk_permission = 0;
					current_step += 1;
				}
				else {
					possible_step = 0;
				}
			}
			else {
				possible_step = 0;
			}
		}

		sprintf(acc_message, " %d %d %d %ld %ld %ld %ld\r\n", x_acc, y_acc, z_acc, eta_time, current_step, step_num);
		HAL_UART_Transmit(&huart6, (uint8_t *)acc_message, sizeof(acc_message), 100);
	}
}


