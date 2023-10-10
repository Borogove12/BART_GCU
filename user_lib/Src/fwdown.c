/*******************************************************************************
*          COPYRIGHT �� 2006 SAMSUNG SDS Co., Ltd. All rights reserved
********************************************************************************
* File Name    : fwdown.c
* Author       : Youn Weon Young
* Date Created : 2006/10/11
* Description  : YModem protocol implementation
********************************************************************************
* [ Revision History ]
* VER.	DATE.		NOTES.											SIGN.
*		2006/10/11	1st Created										YWY
*******************************************************************************/

//#include "71x_lib.h"
#include "gcu_def.h"
#include <string.h>
#include "fwdown.h"
#include "userlib.h"

UART_TypeDef *gDownUARTx;


void Delay(void)
{
	int nDelay = 500000;

	while (nDelay--);
}

/*******************************************************************************
* Name:     GetUartNo
* Function: Get UART No to communicate
* In:       nIndex	- UART Index
* Out:      None
* Return:   Pointer of UART to communicate
* comments: 
*******************************************************************************/
UART_TypeDef* GetUartNo(int nIndex)
{
	if (nIndex == 2)
		return UART2;
	else if (nIndex == 3)
		return UART3;
	else
		return UART0;
}

/*******************************************************************************
* Name:     SendByte
* Function: Send a byte data
* In:       UARTx	- UART port
*			c		- Data
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
inline void SendByte(u8 c)
{
	UART_ByteSend(gDownUARTx, &c);
}

/*******************************************************************************
* Name:     ReceiveByte
* Function: Receive a byte data
* In:       UARTx	- UART port
*			timeout	- data wait time
* Out:      pData	- Received data
* Return:   0		- SUCCESS
*			else	- FAIL
* comments: 
*******************************************************************************/
int ReceiveByte(u8 *pData)
{
	u32 timeout = PACKET_TIMEOUT;

	while (timeout--)
	{
		if (gDownUARTx->SR & UART_RxBufFull)	// If data received
		{
			*pData = (u8)gDownUARTx->RxBUFR;	// Get the received byte
			return 0;
		}
	}

	return -1;
}

/*******************************************************************************
* Name:     CheckSectorErase
* Function: Check flash address whether it is start address of sector or not.
* In:       UARTx	- UART port
*			c		- Data
* Out:      None
* Return:   void
* comments: If the flash address is start address of a sector, then this function 
*			will erase the sector.
*******************************************************************************/
void CheckSectorErase(u32 FlashOffset)
{
	u32 SectorIndex;

	switch(FlashOffset)
	{
	case 0x00000000:	SectorIndex = FLASH_B0F0;	break;
	case 0x00002000:	SectorIndex = FLASH_B0F1;	break;
	case 0x00004000:	SectorIndex = FLASH_B0F2;	break;
	case 0x00006000:	SectorIndex = FLASH_B0F3;	break;
	case 0x00008000:	SectorIndex = FLASH_B0F4;	break;
	case 0x00010000:	SectorIndex = FLASH_B0F5;	break;
	case 0x00020000:	SectorIndex = FLASH_B0F6;	break;
	case 0x00030000:	SectorIndex = FLASH_B0F7;	break;
	default: return;
	}

	FLASH_Init();
	FLASH_WritePrConfig(SectorIndex, DISABLE);	// Disable Write Protection
	FLASH_Init();
	FLASH_SectorErase(SectorIndex);				// Erase Sector
	FLASH_Init();
	
	return;
}

/*******************************************************************************
* Name:     ProgramPacket
* Function: Program a packet data to assigned application offset address
* In:       AppOffsetAddr	- Application offset address
*			buff	- Program data buffer
*			nLen	- Length of program data buffer
* Out:      None
* Return:   0		- SUCCESS
*           else	- FAIL
* comments: 
*******************************************************************************/
int ProgramPacket(u32 AppOffsetAddr, u8 *buff, int nLen)
{
	u32 i, ProgramOffset, dwData;

	ProgramOffset = FLASH_APP_OFFSET + AppOffsetAddr;	// get program offset address

	CheckSectorErase(ProgramOffset);	// check sector has to be erased or not

	for (i=0; i<nLen; i+=4)
	{
		// Make WORD(32bit) data to write
		dwData = buff[i+3];
		dwData <<= 8;
		dwData += buff[i+2];
		dwData <<= 8;
		dwData += buff[i+1];
		dwData <<= 8;
		dwData += buff[i];
		
		FLASH_WordWrite(ProgramOffset+i, dwData);		// program data
		if(dwData != FLASH_WordRead(ProgramOffset+i))	// verify data
			return -1;
	}

	return 0;
}

/*******************************************************************************
* Name:     ReceiveYModemPacket
* Function: Receive YModem Packet
* In:       UARTx - UART pointer to receive packet
* Out:      buff - receive buffer
*                  1st byte contains the packet header
* Return:   0 - SUCCESS
*           else - FAIL
* comments: 
*******************************************************************************/
static int ReceiveYModemPacket(u8 *buff)
{
	int nLenRemain;

	if (ReceiveByte(buff))
		return -1;

	switch (*buff)
	{
	case SOH:	nLenRemain = SOH_PACKET_SIZE;
		break;
	case STX:	nLenRemain = STX_PACKET_SIZE;
		break;
	case EOT:	return 0;
	case CAN:	if (!ReceiveByte(buff) && (*buff == CAN))
					return 0;
				else 
					return -1;
	default:	return -1;
	}

	buff++;
	nLenRemain--;

	while (nLenRemain--)
	{
		if (ReceiveByte(buff))
			return -1;

		buff++;
	}

	return 0;
}

/*******************************************************************************
* Name:     YmodemDownload
* Function: Download an application to update
* In:       None
* Out:      None
* Return:   void
* comments: Download uses YModem protocol. It only receives one files at a time.
*           While downloading 
*******************************************************************************/
void YmodemDownload(void)
{
	//UART_TypeDef *UARTx;
	int i, nRetry=PACKET_RETRY, nPacketSeq=0, nResult, nFinished=0, nError=0;
	int fRequest[4];
	u32 ProgramOffset = 0;
	u8  RcvBuff[MAX_DATA_SIZE];

	while (nRetry--)
	{
		for (i=0; i<4; i++)
		{
			if (i==1)	continue;
			gDownUARTx = GetUartNo(i);
			fRequest[i] = 0;

			for (;;)
			{
				*RcvBuff = 0;	// Initialize first data of buffer

				nResult = ReceiveYModemPacket(RcvBuff);
				if (!nResult)
				{	// SUCCESS
					switch (*RcvBuff)
					{
					case SOH:	// 128 bytes data
						{
							if (!RcvBuff[1] && (RcvBuff[1] == (RcvBuff[2]^0xFF)))	// Seq is 0, so 1st file name packet
							{
								// Not check file name and size
								nPacketSeq = 1;
								ProgramOffset = 0;
								SendByte(ACK);
								if (nFinished)
								{
									Delay();
									return;
								}
								SendByte('C');
							}
							else if ((RcvBuff[1] == nPacketSeq) && (RcvBuff[1] == (RcvBuff[2]^0xFF)))	// packet downloading 
							{
								nResult = ProgramPacket(ProgramOffset, &RcvBuff[3], SOH_DATA_SIZE);
								if (!nResult)
								{
									nPacketSeq++;
									ProgramOffset += SOH_DATA_SIZE;
									SendByte(ACK);
								}
							}
							else
							{
								nResult = -1;
							}
						}
						break;
					case STX:
						{
							if ((RcvBuff[1] == nPacketSeq) && (RcvBuff[1] == (RcvBuff[2]^0xFF)))	// packet downloading 
							{
								nResult = ProgramPacket(ProgramOffset, &RcvBuff[3], STX_DATA_SIZE);
								if (!nResult)
								{
									nPacketSeq++;
									ProgramOffset += STX_DATA_SIZE;
									SendByte(ACK);
								}
							}
							else
								nResult = -1;
						}
						break;
					case EOT:	SendByte(ACK);
								SendByte('C');
								nFinished = 1;
								break;;	// completely downloading finished
					case CAN:	SendByte(ACK);	// Abort by sender
								Delay();
								return;	// Abort packet downloading
					default:	break;
					}

					if (nResult)
					{
						SendByte(CAN);	// Abort by Receiver
						SendByte(CAN);
						Delay();
						return;	// Abort packet downloading, check next UART port
					}
				}
				else
				{	// ReceiveYModemPacket Failed
					if (!fRequest[i] && !(*RcvBuff))
					{
						fRequest[i] = 1;
						SendByte('C');	// Request file transfer
					}
					else if (nPacketSeq)
					{
						if (nError++ >= 100)
							return;
					}
					else 
						break;
				}
			}
		}
	}

	return;
}

/****END OF FILE****/
