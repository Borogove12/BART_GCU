/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "bsp.h"
#include "stm32_hal_legacy.h"
/* USER CODE BEGIN 0 */
#include "global.h"

#define tobcd(v)	(((v / 10) << 4) | (v % 10))
#define frombcd(v)	((v >> 4) * 10 + (v & 0x0f))


/* USER CODE END 0 */

/* RTC init function */
/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */

/* USER CODE BEGIN 1 */
void RtcConfig(void)
{

}
uint32_t testval = 0;
void RtcBackupChk(void)
{

	time_t 	sec = 0;

	config.rBackup = 0;
	SystemInitTime();	// version of system time
	sec = GetRtc();
	if(!config.rBackup) {
		/* Configure the time register */
		SetRtc(1);
		config.RtcType = 0x01;
	}
	else {
		if(sec < SysInitTime) {
			/* RTC configuration  */
			bsp_InitRTC();
			SetRtc(1);	// sec = SysInitTime
			config.RtcType = 0x03;
		}
		else {
			config.sec = sec;
			config.RtcType = 0x00;
		}
	}

	testval = bsp_ReadBackupReg(1);

	 printf("RTC Backup Reg%02d : %08lX\r\n", 0, bsp_ReadBackupReg(0));
	 printf("RTC Backup Reg%02d : %08lX\r\n", 1, bsp_ReadBackupReg(1));
	 printf("RTC Backup Reg%02d : %08lX\r\n", 2, bsp_ReadBackupReg(2));
	 printf("RTC Backup Reg%02d : %08lX\r\n", 3, bsp_ReadBackupReg(3));
	 printf("RTC Backup Reg%02d : %08lX\r\n", 4, bsp_ReadBackupReg(4));

	if(bsp_ReadBackupReg(RTC_BKP_DR1) == RTCSET_ECU)
		bsp_WriteBackupReg(RTC_BKP_DR1,NOMAL_GCU);

	testval = bsp_ReadBackupReg(1);

}

uint32_t rtc_read_backup_reg(uint32_t BackupRegister)
{
        RTC_HandleTypeDef RtcHandle;
        RtcHandle.Instance = RTC;
        return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);

}


void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data)
{
        RTC_HandleTypeDef RtcHandle;
        RtcHandle.Instance = RTC;
        HAL_PWR_EnableBkUpAccess();
        HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
        HAL_PWR_DisableBkUpAccess();
}


/* USER CODE END 1 */
