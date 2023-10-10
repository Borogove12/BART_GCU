/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_iuart.h
 *  모듈 설명    : Internal UART 관련 루틴
 *
 *  작성자       : 김정준
 *  작성일       : 2022/05/02
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef INC_BSP_IUART_H_
#define INC_BSP_IUART_H_


#include "bsp_types.h"
#include "bsp_rbuf.h"
#include "bsp_uart.h"

//#include "usart.h"


#define IUART_EN    1

#if defined(STRAFFIC_GCU)
    #define IUART1_EN   1
    #define IUART2_EN   1
    #define IUART3_EN   1
    #define IUART4_EN   1
    #define IUART5_EN   1
#endif

#if IUART1_EN == 1
    #define IUART1_BAUD         115200
    #define IUART1_RX_BUF_SIZE  1024
#endif

#if IUART2_EN == 1
    #define IUART2_BAUD         115200
    #define IUART2_RX_BUF_SIZE  1024
#endif

#if IUART3_EN == 1
    #define IUART3_BAUD         57600
    #define IUART3_RX_BUF_SIZE  1024
#endif

#if IUART4_EN == 1
    #define IUART4_BAUD         38400
    #define IUART4_RX_BUF_SIZE  1024
#endif

#if IUART5_EN == 1
    #define IUART5_BAUD         38400
    #define IUART5_RX_BUF_SIZE  1024
#endif


typedef struct
{
	BOOLEAN		fIsOpen;
	UINT32		nBaud;
	BYTE		bRecvByte;
	RBUF_T*		pRxRingBuf;
    UART_HandleTypeDef  *hUart;
} IUART_T, *PIUART_T;



void bsp_InitIUart();

BOOL IUart_Open(int nComPort, UINT32 nBaud);
void IUart_Close(int nComPort);

int IUart_SendByte(int nComPort, BYTE bCh);
int IUart_SendBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen);



int IUart_RecvByte(int nComPort);
int IUart_RecvBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen);
int IUart_RecvByteBlocked(int nComPort);
int IUart_RecvBufferBlocked(int nComPort, PUCHAR pBuf, UINT16 usLen);
int IUart_RecvByteTimed(int nComPort, int nTimeout);
int IUart_RecvBufferTimed(int nComPort, PUCHAR pBuf, UINT16 usLen, int nTimeout);

BOOL IUart_IsRxEmpty(int nComPort);
void IUart_ClearRxBuffer(int nComPort);

PIUART_T IUart_GetPIUart(int nComPort);
int IUart_RecvByte_check(int nComPort, BYTE *pData);
void USART3_UART_Init(int nBaudRate);

#endif /* INC_BSP_IUART_H_ */
