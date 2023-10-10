/*******************************************************************************
*       COPYRIGHT �� 2020 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : interface.c
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : Library for GCU application
*******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "userlib.h"
#include "printf.h"

volatile DWORD	gdwTick;

char mcDbgBuffer[256];
char mcDbgData[4096];
int  nDbgDataPt = 0;

#if 0
void inline DebugPutChar(char c)
{
	UART_ByteSend(DEBUG_UART, (u8*)&c);
}
#endif
void DebugPutString(char *s)
{
	while (*s)
	{
		DebugPutChar(*s);
		s ++;
	}
}

void DebugPrintf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsprintf(mcDbgBuffer, fmt, args);
	va_end(args);

	DebugPutString(mcDbgBuffer);
}

void DebugPrintfL(char *fmt, ...)
{
	DebugPrintf(fmt);
	DebugPutString("\r\n");
}
#if 0
void inline CICPutChar(char c)                //HYD
{
	UART_ByteSend(CIC_UART, (u8*)&c);
}
#endif
/*******************************************************************************
* Name:     Print2Hex
* Function: print hex buffer
* In:       szTitle - Debugging Title
*           pBuff - data to be printed
*           nLen - data length
* Return:   None
* comments: print title and hex data with 16 characters a line
*******************************************************************************/
void Print2Hex(char* szTitle, void* pBuff, int nLen)
{
	int i;
	char *pcBuff = (char*)pBuff;

	//PRINT("Recv: %s", szTitle);
	printf("Recv: %s", szTitle);
	for (i=0; i<nLen; i++)
	{
		//if (!(i%16)) PRINT("\r\n");
		if (!(i%16)) printf("\r\n");

		//PRINT("%x ", *pcBuff++);
		printf("%x ", *pcBuff++);
	}
	//PRINT("\r\n");
	printf("\r\n");
}

/*******************************************************************************
* Name:     Print2HexL
* Function: print hex buffer in a line
* In:       szTitle - Debugging Title
*           pBuff - data to be printed
*           nLen - data length
* Return:   None
* comments: print title and hex data in a line
*******************************************************************************/
void Print2HexL(char* szTitle, void* pBuff, int nLen)
{
	char *pcBuff = (char*)pBuff;

	//PRINT("%s: ", szTitle);
	printf("%s: ", szTitle);
	while(nLen--)
		printf("%x ", *pcBuff++);//PRINT("%x ", *pcBuff++);	//PRINT("%02X ", *pcBuff++);

	//PRINT("\r\n");
	printf("\r\n");
}

/*******************************************************************************
* Name:     delay_ms
* Function: time delay function
* In:       nDelay - delay time (unit: ms)
* Out:      None
* Return:   void
* comments: this function can not delay over 500ms. (actually approx. 568ms)
*******************************************************************************/
#if 0
void delay_ms(int nDelay)
{
	DWORD dwDelayCount;
	u16	  nCount;

	TIM_CounterConfig(DELAY_TIM, TIM_CLEAR);	// Clear Timer Counter - initial Counter is 0xFFFC
	TIM_CounterConfig(DELAY_TIM, TIM_START);	// Start Timer

	dwDelayCount = DELAY_CLOCK*nDelay/1000;		// calculate delay clock count

	for (;;)
	{
		nCount = TIM_CounterValue(DELAY_TIM);
		if ((nCount >= (u16)dwDelayCount) && (nCount<0xFFFC))
			break;
	}

	TIM_CounterConfig(DELAY_TIM, TIM_STOP);	// Stop Timer
}
#endif
void SetTimer(TTIMER *pTimer)
{
	pTimer->fStart = TRUE;
	pTimer->dwStartTick = gdwTick;
}

void ResetTimer(TTIMER *pTimer)
{
	pTimer->fStart = FALSE;
}

/*******************************************************************************
* Name:     IsTimeout
* Function: sets(starts) the timer
* In:       pTimer - Timer to check timeout
*			dwTimeout - timeout value (unit: 10ms)
* Out:      None
* Return:   void
* comments: Timeout range is between 0 and (0xFFFFFFFF * 10ms)
*******************************************************************************/
bool IsTimeout(TTIMER *pTimer, DWORD dwTimeout)
{
	bool	fTimeout = FALSE;
	DWORD	dwCurrentTick;

	if (pTimer->fStart)
	{
		dwCurrentTick = gdwTick;

		if (dwCurrentTick >= pTimer->dwStartTick )
		{
			if ( (dwCurrentTick - pTimer->dwStartTick) >= dwTimeout )
				fTimeout = TRUE;
		}
		else // in case of (dwCurrentTick < pTimer->dwStartTick)
		{
			if ( (0xFFFFFFFF - pTimer->dwStartTick + dwCurrentTick) >= dwTimeout )
				fTimeout = TRUE;
		}
	}

	return fTimeout;
}

/*******************************************************************************
* Name:     strntoi
* Function: 
* In:       sz - decimal character buffer
*			nLen - length of character
* Out:      None
* Return:   integer value
* comments: trims the left space char. 
*			if space char is after number char, stop calculating
*******************************************************************************/
int strntoi(char *sz, int nLen)
{
	int		nValue = 0;

	while (nLen--)
	{
		if ((*sz >= '0') && (*sz <= '9'))
			nValue = nValue*10 + (*sz - '0');
		else if ((*sz != ' ') || nValue)
			break;

		sz++;
	}

	return nValue;
}
#if 0
void DEBUG_WaitTxFifo()
{
	while(!((UART_FlagStatus(DEBUG_UART)) & UART_TxEmpty)); // wait until the data transmission is finished
}

void DEBUG_SendChar(BYTE bData)
{
	UART_ByteSend( DEBUG_UART,&bData );
	DEBUG_WaitTxFifo();
}

void DEBUG_Printf( const char *pbformat, ... )
{
	//Modified printf function. For minimal impact on code performance use 16 or less characters.
	static const char hex[] = "0123456789ABCDEF";
	va_list ap;
	ULONG iVal, iDiv, iBase, iWidth, iVar;
	char *pbData, bFormatFlag, bArg;
	char abOutput[ 11 ];
	BYTE i;

	va_start( ap, pbformat );

	for (;;) {													//Until full format string read
		while ((bFormatFlag = *pbformat++) != '%') 
		{ //Until '%' or '\0'
			if (!bFormatFlag) 
			{
				va_end(ap);
				//Wait until FIFO empty
				DEBUG_WaitTxFifo();
				return;
			}
			DEBUG_SendChar(bFormatFlag);
		}

		//Leading zeros
		iWidth = 0;
		if (*pbformat >= '0' && *pbformat <= '9') 
		{
			iWidth = *pbformat - '0';
			pbformat++;
		}

		if (*pbformat == 'l') pbformat++;

		switch (bFormatFlag = *pbformat++) 
		{

		case 'c':
			bFormatFlag = va_arg(ap, int);

		default:
			DEBUG_SendChar(bFormatFlag);
			break;

		case 's':
		case 'S':
			pbData = va_arg(ap, char *);
			while (*pbData)
			{
				bArg = *pbData++;
				DEBUG_SendChar(bArg);
			}			
				
			break;
		case 'd':
		case 'u':
			iBase = 10;
			if ( sizeof( int ) == 2 )
				iDiv = 10000;
			else
				iDiv = 1000000000;

			goto CONVERSION_LOOP;

		case 'x':
			iBase = 16;
			if ( sizeof( int ) == 2 )
				iDiv = 0x1000;
			else
				iDiv = 0x10000000;

CONVERSION_LOOP:

			iVal = va_arg( ap, int );
			if ( bFormatFlag == 'd') {
				if ( ( (int)iVal ) < 0 ) {
					iVal = - iVal;
					DEBUG_SendChar( '-' );
				}
			}

			//Suppress lead zeroes
			if ( (iBase == 10) || iWidth ) {
				while ( iDiv > 1 && iDiv > iVal ) {
					iDiv /= iBase;
				}
			}

			iVar = 0;
			for(i=0;i<sizeof( abOutput );i++){
				abOutput[i]=0x00;
			}

			do {
				abOutput[ iVar++ ] = hex[ iVal / iDiv ];
				iVal %= iDiv;
				iDiv /= iBase;
			} while ( iDiv );

			//Generate leading zeros if required
			while ( (LONG)(iWidth - iVar) > 0 ) {
				DEBUG_SendChar( '0' );
				iWidth--;
			}

			//abOutput characters
			pbData = abOutput;
			while ( *pbData ) DEBUG_SendChar( *pbData++ );
		}
	}
}

void DEBUG_PrintfL(char *fmt, ...)
{
	DEBUG_Printf(fmt);
	DebugPutString("\r\n");
}

void SetPrintData(char* szData)
{
	memcpy(mcDbgData+nDbgDataPt, szData, strlen(szData));
	nDbgDataPt += strlen(szData);
	memcpy(mcDbgData+nDbgDataPt, "\r\n", 2);
	nDbgDataPt += 2;
}

void PrintData(void)
{
	if (nDbgDataPt)
	{
		DebugPutString(mcDbgData);
		memset(mcDbgData, 0, sizeof(mcDbgData));
		nDbgDataPt = 0;
	}
}
#endif

DWORD GetTick(void) 
{
	return gdwTick;
}
/****END OF FILE****/
