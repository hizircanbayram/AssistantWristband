/*
 * mma8452q.c
 *
 *  Created on: 15 Haz 2020
 *      Author: hizirov
 */

#include "mma8452q.h"

/*
 * This function writes value to address reg using the assigned peripheral address
 * hi2c : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * reg : address of register whose value is set inside the censor
 * value : reg's new content
 * */
HAL_StatusTypeDef mma8452qWrite(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t value) {
	uint8_t data[2];
	data[0] = reg;
	data[1] = value;
	return HAL_I2C_Master_Transmit(hi2c, MMA8452Q_DEVICE_ADDR, data, 2, 10);
}



/*
 * This function read value starting from the address reg and stores them into data_rec using the assigned peripheral address
 * hi2c  : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * reg   : address of register where to start reading data from
 * numberofbytes : amount of data to be read from the registers of the censor
 * data_rec : address of buffer where the data from the censor will be stored into
 * */
HAL_StatusTypeDef mma8452qRead(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t numberofbytes, uint8_t* data_rec) {
	return HAL_I2C_Mem_Read(hi2c, MMA8452Q_DEVICE_ADDR, reg, 1, data_rec, numberofbytes, 100);
}



/*
 * This function initializes all the necessary censor registers in order to read data from it
 * hi2c  : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * */
HAL_StatusTypeDef mma8452qInit(I2C_HandleTypeDef* hi2c) {
	  HAL_StatusTypeDef ret1 = mma8452qWrite(hi2c, MMA8452Q_REG_ADDR_1, 0x00);
	  HAL_StatusTypeDef ret2 = mma8452qWrite(hi2c, MMA8452Q_REG_ADDR_1, 0x01);
	  HAL_StatusTypeDef ret3 = mma8452qWrite(hi2c, MMA8452Q_REG_ADDR_2, 0x00);
	  if (ret1 == HAL_OK && ret2 == HAL_OK && ret3 == HAL_OK)
		  return HAL_OK;
	  else
		  return HAL_BUSY;
}



/*
 * This function takes 7 bytes coming from censor and converts them into meaningfull acceloremeter values
 * acc_info : the array(7 bytes) that contains status, MSB of X acc, LSB of X acc, MSB of Y acc, LSB of Y acc, MSB of Z acc, LSB of Z acc
 * acc_xyz : new array(6 bytes) that contains acc x, acc y, acc z
 * */
void getAccXYZ(const uint8_t* acc_info, int16_t* acc_xyz) {
	int16_t xAccl = ((acc_info[1] * 256) + acc_info[2]) / 16;
	if (xAccl > 2047)
	{
	xAccl -= 4096;
	}

	int16_t yAccl = ((acc_info[3] * 256) + acc_info[4]) / 16;
	if (yAccl > 2047)
	{
	  yAccl -= 4096;
	}

	int16_t zAccl = ((acc_info[5] * 256) + acc_info[6]) / 16;
	if (zAccl > 2047)
	{
	  zAccl -= 4096;
	}

	acc_xyz[0] = xAccl;
	acc_xyz[1] = yAccl;
	acc_xyz[2] = zAccl;
}
