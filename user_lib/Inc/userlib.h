/*******************************************************************************
*       COPYRIGHT �� 2020 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : userlib.h
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : GCU application for interface with host
*******************************************************************************/

#ifndef __USERLIB_H_
#define __USERLIB_H_

//#include "71x_lib.h"
#include "gcu_def.h"
#include "global.h"

#define DELAY_PRESCALER	192

typedef struct {
	bool	fStart;
	DWORD	dwStartTick;
	DWORD	dwTimeoutTick;
} TTIMER;

//////////////////////////////////////////////////////////////////////////
// DELAY_CLOCK	= PCLK2/DELAY_PRESCALER
// PCLK			= RCLK/PCLK-Divider
// RCLK			= RCCU_Main_Osc/Pre-Divider*Multiplier/Divider
//////////////////////////////////////////////////////////////////////////
#define DELAY_CLOCK		115200		// (22118400/DELAY_PRESCALER), 22118400 -> PCLK2

#define PRINT	DEBUG_Printf	//DEBUG_Printf	//DebugPrintf	//
#define PRINTL	DEBUG_PrintfL	//DEBUG_PrintfL	//DebugPrintfL	//

/* Functions prototypes ------------------------------------------------------*/
//void Jump_Flash(u32 Addr);
void DebugPrintf(char *fmt, ...);
void DebugPrintfL(char *fmt, ...);
void Print2Hex(char* szTitle, void* pBuff, int nLen);
void Print2HexL(char* szTitle, void* pBuff, int nLen);
void delay_ms(int nDelay);
void SetTimer(TTIMER *pTimer);
void ResetTimer(TTIMER *pTimer);
bool IsTimeout(TTIMER *pTimer, DWORD dwTimeout);

int  strntoi(char *sz, int nLen);

void DEBUG_Printf( const char *pbformat, ... );
void DEBUG_PrintfL(char *fmt, ...);
void SetPrintData(char* szData);
void PrintData(void);
DWORD GetTick(void);
#endif  // __USERLIB_H_

/****END OF FILE****/
