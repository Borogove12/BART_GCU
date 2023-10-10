/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_timer.h
 *  모듈 설명    : Timer Interrupt 관련 루틴 - TIM7 기본 타임머 사용
 *
 *  작성자       : 김정준
 *  작성일       : 2022/05/02
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#include "bsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define TIMEOUT_INFINITE    0xFFFFFFFF

void bsp_InitTimer(void);

DWORD bsp_GetTickCount(void);
time_t bsp_GetTime(void);
UINT64 bsp_GetTimeEx(void);
void bsp_SetTime(time_t uEpoch);
void bsp_TimCallback(void);

void bsp_Set1msCallback(void (*Callback)(void));
void bsp_Set10msCallback(void (*Callback)(void));
void bsp_Set100msCallback(void (*Callback)(void));
void bsp_Set1SecCallback(void (*Callback)(void));

//----- 추가 함수 ---------------------
// TFT LCD 밝기 조절 함수
void bsp_SetLcdBrightness(uint8_t nSet);
uint8_t bsp_GetLcdBrightness(void);
//-------------------------------------
void delay_us(uint16_t us);
BOOL IsTimeoutMs(DWORD *base, DWORD toMs);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  // _BSP_TIMER_H_
