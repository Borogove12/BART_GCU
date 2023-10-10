/*******************************************************************************
*       COPYRIGHT �� 2006,2007 SAMSUNG SDS Co., Ltd. All rights reserved
********************************************************************************
* File Name    : serial.c
* Author       : Youn Weon Young
* Date Created : 2006/10/18
* Description  : 
********************************************************************************
* [ Revision History ]
* VER.	DATE.		NOTES.											SIGN.
*		2006/10/18	1st Created										YWY
*******************************************************************************/

//#include "71x_lib.h"
#include "serial.h"

BYTE U0_rxfifo[U0_FIFO_SIZE]; // SW FIFO for UART0
VINT U0_rxrp = 0;
VINT U0_rxwp = 0;

BYTE U1_rxfifo[U1_FIFO_SIZE]; // SW FIFO for UART1
VINT U1_rxrp = 0;
VINT U1_rxwp = 0;

BYTE U2_rxfifo[U2_FIFO_SIZE]; // SW FIFO for UART2
VINT U2_rxrp = 0;
VINT U2_rxwp = 0;

BYTE U3_rxfifo[U3_FIFO_SIZE]; // SW FIFO for UART3
VINT U3_rxrp = 0;
VINT U3_rxwp = 0;

#if 0
/*******************************************************************************
* UART1 Implementation
*******************************************************************************/

/*******************************************************************************
* Name:     U0_TxWait
* Function: Wait until UART0 Tx line is empty
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U0_TxWait(void)
{
	while (!(UART0->SR & UART_TxEmpty));	// wait until TX is empty
}

/*******************************************************************************
* Name:     U0_Putch
* Function: Put a BYTE to UART0
* In:       c	- data to put in
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U0_Putch(BYTE c)
{
	while (!(UART0->SR & UART_TxEmpty));	// wait until TX is empty
	UART0->TxBUFR = (u8)c;					// send a BYTE
}

/*******************************************************************************
* Name:     U0_Puts
* Function: Put data to UART0
* In:       pData	- data buffer to send
*			nLen    - data length of buffer
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U0_Puts(BYTE *pData, int nLen)
{
	while (nLen--)
	{
		while (!(UART0->SR & UART_TxEmpty));	// wait until TX is empty
		UART0->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U0_PutString
* Function: Put String to UART0
* In:       pData	- string to send
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U0_PutString(BYTE *pData)
{
	while (*pData)
	{
		while (!(UART0->SR & UART_TxEmpty));	// wait until TX is empty
		UART0->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U0_Getch
* Function: Get data from UART0 SW RX FIFO
* In:       None
* Out:      pData	- received byte pointer
* Return:   TRUE	- One data is received
*			FALSE	- No data is received
* comments: 
*******************************************************************************/
bool U0_Getch(BYTE *pData)
{
	if (U0_rxwp == U0_rxrp)
		return FALSE;

	*pData = U0_rxfifo[U0_rxrp++];

	if (U0_rxrp == U0_FIFO_SIZE)
		U0_rxrp = 0;

	return TRUE;
}

/*******************************************************************************
 * UART1 Implementation
 *******************************************************************************/

/*******************************************************************************
 * Name:     U1_TxWait
 * Function: Wait until UART1 Tx line is empty
 * In:       None
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void U1_TxWait(void)
{
	while (!(UART1->SR & UART_TxEmpty))
		; // wait until TX is empty
}

/*******************************************************************************
 * Name:     U1_Putch
 * Function: Put a BYTE to UART1
 * In:       c	- data to put in
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void U1_Putch(BYTE c)
{
	while (!(UART1->SR & UART_TxEmpty))
		;				   // wait until TX is empty
	UART1->TxBUFR = (u8)c; // send a BYTE
}

/*******************************************************************************
 * Name:     U1_Puts
 * Function: Put data to UART1
 * In:       pData	- data buffer to send
 *			 nLen    - data length of buffer
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void U1_Puts(BYTE *pData, int nLen)
{
	while (nLen--)
	{
		while (!(UART1->SR & UART_TxEmpty))
			;						  // wait until TX is empty
		UART1->TxBUFR = (u8)*pData++; // send data
	}
}

/*******************************************************************************
 * Name:     U1_PutString
 * Function: Put String to UART1
 * In:       pData	- string to send
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void U1_PutString(BYTE *pData)
{
	while (*pData)
	{
		while (!(UART1->SR & UART_TxEmpty))
			;						  // wait until TX is empty
		UART1->TxBUFR = (u8)*pData++; // send data
	}
}

/*******************************************************************************
 * Name:     U1_Getch
 * Function: Get data from UART1 SW RX FIFO
 * In:       None
 * Out:      pData	- received byte pointer
 * Return:   TRUE	- One data is received
 *			FALSE	- No data is received
 * comments:
 *******************************************************************************/
bool U1_Getch(BYTE *pData)
{
	if (U1_rxwp == U1_rxrp)
		return FALSE;

	*pData = U1_rxfifo[U1_rxrp++];

	if (U1_rxrp == U1_FIFO_SIZE)
		U1_rxrp = 0;

	return TRUE;
}

/*******************************************************************************
* UART2 Implementation
*******************************************************************************/

/*******************************************************************************
* Name:     U2_TxWait
* Function: Wait until UART2 Tx line is empty
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U2_TxWait(void)
{
	while (!(UART2->SR & UART_TxEmpty));	// wait until TX is empty
}

/*******************************************************************************
* Name:     U2_Putch
* Function: Put a BYTE to UART2
* In:       c	- data to put in
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U2_Putch(BYTE c)
{
	while (!(UART2->SR & UART_TxEmpty));	// wait until TX is empty
	UART2->TxBUFR = (u8)c;					// send a BYTE
}

/*******************************************************************************
* Name:     U2_Puts
* Function: Put data to UART2
* In:       pData	- data buffer to send
*			nLen    - data length of buffer
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U2_Puts(BYTE *pData, int nLen)
{
	while (nLen--)
	{
		while (!(UART2->SR & UART_TxEmpty));	// wait until TX is empty
		UART2->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U2_PutString
* Function: Put String to UART2
* In:       pData	- string to send
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U2_PutString(BYTE *pData)
{
	while (*pData)
	{
		while (!(UART2->SR & UART_TxEmpty));	// wait until TX is empty
		UART2->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U2_Getch
* Function: Get data from UART2 SW RX FIFO
* In:       None
* Out:      pData	- received byte pointer
* Return:   TRUE	- One data is received
*			FALSE	- No data is received
* comments: 
*******************************************************************************/
bool U2_Getch(BYTE *pData)
{
	if (U2_rxwp == U2_rxrp)
		return FALSE;

	*pData = U2_rxfifo[U2_rxrp++];

	if (U2_rxrp == U2_FIFO_SIZE)
		U2_rxrp = 0;

	return TRUE;
}

/*******************************************************************************
* UART3 Implementation
*******************************************************************************/

/*******************************************************************************
* Name:     U3_TxWait
* Function: Wait until UART3 Tx line is empty
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U3_TxWait(void)
{
	while (!(UART3->SR & UART_TxEmpty));	// wait until TX is empty
}

/*******************************************************************************
* Name:     U3_Putch
* Function: Put a BYTE to UART3
* In:       c	- data to put in
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U3_Putch(BYTE c)
{
	while (!(UART3->SR & UART_TxEmpty));	// wait until TX is empty
	UART3->TxBUFR = (u8)c;					// send a BYTE
}

/*******************************************************************************
* Name:     U3_Puts
* Function: Put data to UART3
* In:       pData	- data buffer to send
*			nLen    - data length of buffer
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U3_Puts(BYTE *pData, int nLen)
{
	while (nLen--)
	{
		while (!(UART3->SR & UART_TxEmpty));	// wait until TX is empty
		UART3->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U3_PutString
* Function: Put String to UART2
* In:       pData	- string to send
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void U3_PutString(BYTE *pData)
{
	while (*pData)
	{
		while (!(UART3->SR & UART_TxEmpty));	// wait until TX is empty
		UART3->TxBUFR = (u8)*pData++;			// send data
	}
}

/*******************************************************************************
* Name:     U3_Getch
* Function: Get data from UART1 SW RX FIFO
* In:       None
* Out:      pData	- received byte pointer
* Return:   TRUE	- One data is received
*			FALSE	- No data is received
* comments: 
*******************************************************************************/
bool U3_Getch(BYTE *pData)
{
	if (U3_rxwp == U3_rxrp)
		return FALSE;

	*pData = U3_rxfifo[U3_rxrp++];

	if (U3_rxrp == U3_FIFO_SIZE)
		U3_rxrp = 0;

	return TRUE;
}

/*******************************************************************************
* Name:     U3_Flush
* Function: Flush all received data of UART3
* In:       None
* Out:      None
* Return:   None
*			FALSE	- No data is received
* comments: 
*******************************************************************************/
void U3_Flush(void)
{
	U3_rxrp = U3_rxwp;
}
#endif
/******* COPYRIGHT �� 2006, 2007 SAMSUNG SDS Co., Ltd.  ********END OF FILE****/
