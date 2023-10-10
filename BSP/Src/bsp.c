/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp.c
 *  모듈 설명    : BSP(Board Support Package) - STM32H743 GCU 최상위 헤더
 *
 *  작성자       : 김정준
 *  작성일       : 2022/03/15
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "croutine.h"
//#include "semphr.h"
//#include "event_groups.h"

#include "bsp.h"

/*
*******************************************************************************
*			함수 선언
*******************************************************************************
*/
__attribute__((weak)) void SystemClock_Config(void);
/*
*******************************************************************************
*			함수 정의
*******************************************************************************
*/



void System_Halt(char *file, uint32_t line)
{
    // TODO: 모든 램프 On
	// printf("Wrong parameters value: file %s on line %ld\r\n", file, line);

	if (line == 0)
	{
		return;
	}

	/* 사용자가 오류를 확인할 수 여기서 무한 대기 */
	while (1)
	{
	}
}
