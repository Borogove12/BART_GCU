/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_rtc.h
 *  모듈 설명    : Real Time Clock 관련 루틴
 *
 *  작성자       : 김정준
 *  작성일       : 2022/05/02
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_


#include "bsp_types.h"


#define RTCSET_MAGIC    0x55AA32F4


void bsp_InitRTC(void);

//----- 추가 함수 ---------------------
// BootLoader에서 RTC가 이미 초기화 된 상태이므로 여기서 또 초기화하면
// Register 값이 초기화 됨. MX_RTC_Init()를 호출하면 안돼고 대신 bsp_InitRTCforApp()를 호출 한다.
void bsp_InitRTCforApp(void);
void bsp_SetDefaultTime(void);
//-------------------------------------

time_t bsp_BcdDtimeToTime(PBYTE pbcdDTime);
time_t bsp_SystemTimeToTime(PSYSTEMTIME pSysTime);
time_t bsp_RTCDTimeToTime(RTC_DateTypeDef* pDate, RTC_TimeTypeDef *pTime);
void bsp_SetSystemTimeInEpocTime(time_t cur);
void bsp_SetSystemTimeInBcdDTime(PBYTE pbcdDTime);
void bsp_SetSystemTimeInTm(struct tm* tm);

time_t bsp_GetSystemTimeInEpoc(void);
void bsp_GetSystemTimeInBcdDtime(PBYTE pBcdDTime);
void bsp_GetSystemTimeInTm(struct tm* pRetTm);

uint32_t bsp_ReadBackupReg(uint32_t BackupRegister);
void bsp_WriteBackupReg(uint32_t BackupRegister, uint32_t Data);

struct tm *GmTime_r(const time_t *timer, struct tm *tmbuf);
struct tm *GmTime(const time_t *timer);

void Test_SetSystemTime(void);
void Dump_AllTimeValue(void);
void Dump_RtcBackupRegs(void);

BOOL IsValidRtcDate(RTC_DateTypeDef* pDate);
BOOL IsValidTime(RTC_TimeTypeDef* pTime);
BOOL ConvStrToDate(PCHAR szDate, int* pnYear, int* pnMonth, int* pnDay);
BOOL ConvStrToTime(PCHAR szTime, int* pnHour, int* pnMin, int* pnSec);
void Cmd_SetDateTime(void);


#endif  //  INC_BSP_RTC_H_
