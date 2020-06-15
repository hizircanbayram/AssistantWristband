/*
 * mma8452q.h
 *
 *  Created on: 15 Haz 2020
 *      Author: hizirov
 */

#ifndef INC_MMA8452Q_H_
#define INC_MMA8452Q_H_

/*
 * This header file contains necessary functions to read data from MMA8452Q 3D accelerometer censor.
 * */

#include "stm32f4xx_hal.h"


// the device address for I2C protocol
#define MMA8452Q_DEVICE_ADDR (0x1C << 1)
// first register address to be set its value
#define MMA8452Q_REG_ADDR_1 (0x2A)
// second register address to be set its value
#define MMA8452Q_REG_ADDR_2 (0x0E)



/*
 * This function writes value to address reg using the assigned peripheral address
 * hi2c : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * reg : address of register whose value is set inside the censor
 * value : reg's new content
 * */
HAL_StatusTypeDef mma8452qWrite(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t value);



/*
 * This function read value starting from the address reg and stores them into data_rec using the assigned peripheral address
 * hi2c  : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * reg   : address of register where to start reading data from
 * numberofbytes : amount of data to be read from the registers of the censor
 * data_rec : address of buffer where the data from the censor will be stored into
 * */
HAL_StatusTypeDef mma8452qRead(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t numberofbytes, uint8_t* data_rec);



/*
 * This function initializes all the necessary censor registers in order to read data from it
 * hi2c  : address of i2c peripheral device that will make communication between STM32 and MMA8452Q censor
 * */
HAL_StatusTypeDef mma8452qInit(I2C_HandleTypeDef* hi2c);



/*
 *
 * acc_info : the array that contains status, MSB of X acc, LSB of X acc, MSB of Y acc, LSB of Y acc, MSB of Z acc, LSB of Z acc
 * acc_xyz : new array that contains acc x, acc y, acc z
 * */
void getAccXYZ(const uint8_t* acc_info, int16_t* acc_xyz);

#endif /* INC_MMA8452Q_H_ */
