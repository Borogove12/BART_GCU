/* * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_macro.h
 *  모듈 설명    : 각종 매크로 선언 및 정의
 *
 *  작성자       : 김정준
 *  작성일       : 2022/05/24
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_MACRO_H_
#define _BSP_MACRO_H_


// #define _USE_LONG_TIME_T    1

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
    #include "cmsis_os.h"
	#define DISABLE_INT()	taskENTER_CRITICAL()
	#define ENABLE_INT()	taskEXIT_CRITICAL()
#else
//	#define DISABLE_INT()	__set_PRIMASK(0)    // 전역 인터럽트 활성화
//	#define ENABLE_INT()	__set_PRIMASK(1)    // 전역 인터럽트 비활성화
	#define DISABLE_INT()
	#define ENABLE_INT()
#endif


#define ERROR_HANDLER()		System_Halt(__FILE__, __LINE__);


#define UlongToPtr(u)   ((PVOID)((ULONG)(u)))
#define PtrToUlong(u)   ((ULONG)((PVOID)(u)))


#define READ_REGISTER_UCHAR(r)      (*(volatile UCHAR * const)(r))
#define READ_REGISTER_USHORT(r)     (*(volatile USHORT * const)(r))
#define READ_REGISTER_ULONG(r)      (*(volatile ULONG * const)(r))
#define WRITE_REGISTER_UCHAR(r, v)  (*(volatile UCHAR * const)(r) = (v))
#define WRITE_REGISTER_USHORT(r, v) (*(volatile USHORT * const)(r) = (v))
#define WRITE_REGISTER_ULONG(r, v)  (*(volatile ULONG * const)(r) = (v))
#define READ_PORT_UCHAR(r)          READ_REGISTER_UCHAR(r)
#define READ_PORT_USHORT(r)         READ_REGISTER_USHORT(r)
#define READ_PORT_ULONG(r)          READ_REGISTER_ULONG(r)
#define WRITE_PORT_UCHAR(p, v)      WRITE_REGISTER_UCHAR(p, (UCHAR) (v))
#define WRITE_PORT_USHORT(p, v)     WRITE_REGISTER_USHORT(p, (USHORT) (v))
#define WRITE_PORT_ULONG(p, v)      WRITE_REGISTER_ULONG(p, (ULONG) (v))

/* 인터럽트 우선순위 그룹화 정의 */
#define NVIC_PREEMPT_PRIORITY 4


#define PI              3.1415926535897932384626433832795
#define HALF_PI         1.5707963267948966192313216916398
#define TWO_PI          6.283185307179586476925286766559
#define DEG_TO_RAD      0.017453292519943295769236907684886
#define RAD_TO_DEG      57.295779513082320876798154814105
#define EULER           2.718281828459045235360287471352

// low, high 벋어나지 않은 값을 리턴
#define CONSTRAIN(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


/* TODO: buffered logging 함수로 대체해야함 */
#define DbgPrintf(fmt, ...)     printf(fmt, ##__VA_ARGS__)

#define clrscr()	printf("\033[2J")       // clear screen
#define clreol()	printf("\033[K")        // clear end of line
#define gotoxy(x,y)	printf("\033[%d;%dH", x, y)     // set new cursor position
#define cuu(y)		printf("\033[%dA", y)   // cursor move up
#define cud(y)		printf("\033[%dB", y)   // cursor move down
#define cuf(x)		printf("\033[%dC", x)   // cursor move forword
#define cub(x)		printf("\033[%dD", x)   // cursor move backword
#define scp()		printf("\033[s")        // save current cursor position
#define rcp()		printf("\033[u")        // restore last saved cursor position
#define _delline()	{cub(80); clreol();}	// delete currently line and x position is 1
#define delline()	{scp(); _delline(); rcp();}	// delete currently line without position change
#define dsr()		printf("\033[6n")       // device status report

#define dispmsg(T, fmt, ...) if (T) printf(fmt, ##__VA_ARGS__)

#endif /* INC_BSP_MACRO_H_ */
