/*
 * Init.h
 *
 *  Created on: 2023. 1. 25.
 *      Author: projun
 */

#ifndef INC_INITHW_H_
#define INC_INITHW_H_


extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim7;

extern SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern SRAM_HandleTypeDef hsram1;
extern SRAM_HandleTypeDef hsram2;
extern NAND_HandleTypeDef hnand1;
extern SDRAM_HandleTypeDef hsdram1;

extern CRC_HandleTypeDef hcrc;


void MPU_Config();
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void InitPeripherals(void);



#endif /* INC_INITHW_H_ */
