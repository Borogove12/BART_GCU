/*******************************************************************************
*       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : serial.h
* Author       : Joseph Yosep Choi
* Date Created : 2022/09/14
* Description  : Definition of the serial interface
*******************************************************************************/

#ifndef __SERIAL_H_
#define __SERIAL_H_

#include "gcu_def.h"
#include "bsp_types.h"
#if 0
enum {
	STX	= 0x02,
	ETX = 0x03,
	DLE = 0x10,
	ACK = 0x06,
	NAK = 0x15,

	DLE_OP  = 0x40,
	DLE_STX = 0x42,
	DLE_ETX = 0x43,
	DLE_DLE = 0x50
};
#endif

enum {
	FIFO_SIZE_64	= 64,
	FIFO_SIZE_256	= 256
};

#define U0_FIFO_SIZE	FIFO_SIZE_64
#define U1_FIFO_SIZE 	FIFO_SIZE_256
#define U2_FIFO_SIZE	FIFO_SIZE_256
#define U3_FIFO_SIZE 	FIFO_SIZE_256

/* Global Variables ----------------------------------------------------------*/
extern BYTE	U0_rxfifo[];	// SW FIFO for UART0
extern VINT	U0_rxrp;
extern VINT	U0_rxwp;

extern BYTE U1_rxfifo[];	// SW FIFO for UART1
extern VINT U1_rxrp;
extern VINT U1_rxwp;

extern BYTE	U2_rxfifo[];	// SW FIFO for UART2
extern VINT	U2_rxrp;
extern VINT	U2_rxwp;

extern BYTE	U3_rxfifo[];	// SW FIFO for UART3
extern VINT	U3_rxrp;
extern VINT	U3_rxwp;

/* Functions Prototypes ------------------------------------------------------*/
void U0_TxWait(void);
void U0_Putch(BYTE c);
void U0_Puts(BYTE *pData, int nLen);
void U0_PutString(BYTE *pData);
bool U0_Getch(BYTE *pData);


void U3_TxWait(void);
void U3_Putch(BYTE c);
void U3_Puts(BYTE *pData, int nLen);
void U3_PutString(BYTE *pData);
bool U3_Getch(BYTE *pData);


void U4_TxWait(void);				//Primary barrier
void U4_Putch(BYTE c);
void U4_Puts(BYTE *pData, int nLen);
void U4_PutString(BYTE *pData);
bool U4_Getch(BYTE *pData);



void U5_TxWait(void);			//Secondary barrier
void U5_Putch(BYTE c);
void U5_Puts(BYTE *pData, int nLen);
void U5_PutString(BYTE *pData);
bool U5_Getch(BYTE *pData);
void U5_Flush(void);

#endif  // __SERIAL_H_

/******* COPYRIGHT �� 2022 STraffic Co., Ltd.  ********END OF FILE****/
