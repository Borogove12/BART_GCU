/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_config.h
 *  모듈 설명    : BSP 설정 관련 정의
 *
 *  작성자       : 김정준
 *  작성일       : 2022/03/08
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_CONFIG_H_
#define _BSP_CONFIG_H_



#if defined(STM32F411xE) || defined(STM32F429xx)
	#include "stm32f4xx_hal.h"
#elif defined(STM32F767xx)
	#include "stm32f7xx_hal.h"
#elif defined(STM32H743xx)
    #include "stm32h7xx_hal.h"
#else
    #error "Please select first the target STM32 device or MCU used in your application"
#endif

// BOARD
#define STRAFFIC_GCU	1

#define USE_FreeRTOS    0
#define USE_FATFS       1


#endif /* _BSP_CONFIG_H_ */
