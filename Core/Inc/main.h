/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UPS_CHK_Pin GPIO_PIN_6
#define UPS_SHDN_Pin GPIO_PIN_7
#define nUPS_PWR_FAIL_Pin GPIO_PIN_8
#define nUPS_LOW_BAT_Pin GPIO_PIN_9
#define nUPS_CONN_Pin GPIO_PIN_10
#define nUPS_GPIO_Port GPIOF

#define SP_SW1_Pin GPIO_PIN_0
#define SP_SW2_Pin GPIO_PIN_1
#define SP_SW_GPIO_Port GPIOC

#define Serial_flash_CS_Pin GPIO_PIN_4
#define Serial_flash_CS_GPIO_Port GPIOA

#define DipSW1_Pin GPIO_PIN_0
#define DipSW2_Pin GPIO_PIN_1
#define DipSW3_Pin GPIO_PIN_2
#define DipSW4_Pin GPIO_PIN_3
#define DipSW5_Pin GPIO_PIN_4
#define DipSW6_Pin GPIO_PIN_5
#define DipSW7_Pin GPIO_PIN_6
#define DipSW8_Pin GPIO_PIN_7
#define DipSW_GPIO_Port GPIOB

#define STS_LED1_Pin GPIO_PIN_4
#define STS_LED2_Pin GPIO_PIN_5
#define STS_LED3_Pin GPIO_PIN_6
#define STS_LED4_Pin GPIO_PIN_9
#define STSLED_GPIO_Port GPIOC

#define SP_SEN1_Pin GPIO_PIN_10
#define SP_SEN2_Pin GPIO_PIN_11
#define SP_SEN_GPIO_Port GPIOA

#define BUZZ_CON_Pin GPIO_PIN_12
#define BUZZ_CON_GPIO_Port GPIOA

#define WDT_Pin GPIO_PIN_2
#define WDT_GPIO_Port GPIOD

#define EMG_Pin GPIO_PIN_9
#define EMG_GPIO_Port GPIOB


#define WDT_CONTROL_Pin GPIO_PIN_8
#define WDT_CONTROL_GPIO_Port GPIOA


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


//serial flash (8Mbyte) 사용 방밥 안내 		pms 20231027

// wirte  : 	int flash_write(siz_t addr, byte *buf, siz_t size) 이용

//예시 			BYTE buf1[2] = {2,0}; 선언
// 				flash_write(0x4000, (byte *)&buf1, 2);					address는 0x0 ~ 0x800000-1 범위 까지 사용 가능

// read  : 		int flash_read(siz_t addr, byte *buf, siz_t size) 이용

//예시			BYTE buf2[3] = {0,0,0}; 선언
//  			flash_read(0x4000, (byte *)&buf2, 3);
//				printf("buf2[%08lX][%08lX][%08lX] \n", buf2[0],buf2[1],buf2[2]);		//buf2[2][0][0] 출력

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
