#include "bsp_iuart.h"
#include "bsp_timer.h"
#include "bsp_util.h"
#include "bsp_rbuf.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;


static uint8_t 	s_RxBuf1[IUART1_RX_BUF_SIZE];
static RBUF_T	s_RxRingBuf1;
static uint8_t 	s_RxBuf2[IUART2_RX_BUF_SIZE];
static RBUF_T	s_RxRingBuf2;
static uint8_t 	s_RxBuf3[IUART3_RX_BUF_SIZE];
static RBUF_T	s_RxRingBuf3;
static uint8_t 	s_RxBuf4[IUART4_RX_BUF_SIZE];
static RBUF_T	s_RxRingBuf4;
static uint8_t 	s_RxBuf5[IUART5_RX_BUF_SIZE];
static RBUF_T	s_RxRingBuf5;


IUART_T 	iUartTable[IUART_MAXPORT];


void USART3_UART_Init(int nBaudRate)
{
	HAL_UART_DeInit(&huart3);
	iUartTable[2].fIsOpen = FALSE;
	iUartTable[2].nBaud = nBaudRate;
	iUartTable[2].hUart = NULL;
	IUart_Open(COM3, iUartTable[COM3].nBaud);
}

void bsp_InitIUart()
{
	UINT32 	i;

	HAL_UART_DeInit(&huart1);
	HAL_UART_DeInit(&huart2);
	HAL_UART_DeInit(&huart3);
	HAL_UART_DeInit(&huart4);
	HAL_UART_DeInit(&huart5);

	for ( i = 0; i < IUART_MAXPORT; i++)
	{
		if(i == COM3)			//
		{
			iUartTable[i].fIsOpen = FALSE;
			iUartTable[i].nBaud = 57600;
			iUartTable[i].hUart = NULL;

		}
		else if(i == COM4)
		{
			iUartTable[i].fIsOpen = FALSE;
			iUartTable[i].nBaud = 38400;
			iUartTable[i].hUart = NULL;
		}
		else if(i == COM5)
		{
			iUartTable[i].fIsOpen = FALSE;
			iUartTable[i].nBaud = 38400;
			iUartTable[i].hUart = NULL;
		}
		else
		{
			iUartTable[i].fIsOpen = FALSE;
			iUartTable[i].nBaud = 115200;
			iUartTable[i].hUart = NULL;
		}
	}

	IUart_Open(COM1, IUART1_BAUD);
	IUart_Open(COM2, IUART2_BAUD);
	IUart_Open(COM3, IUART3_BAUD);
	IUart_Open(COM4, IUART4_BAUD);
	IUart_Open(COM5, IUART5_BAUD);
}


BOOL IUart_Open(int nComPort, UINT32 nBaud)
{
	IUART_T* pIUart = NULL;
	IRQn_Type	irqNType = USART1_IRQn;

	if (nComPort < IUART_STARTPORT || nComPort >= (IUART_STARTPORT + IUART_MAXPORT))
		return FALSE;

	pIUart = &iUartTable[nComPort];

	if (nComPort == COM1)
	{
		pIUart->hUart = &huart1;
		pIUart->pRxRingBuf = &s_RxRingBuf1;
		RBuf_Init(pIUart->pRxRingBuf, s_RxBuf1, IUART1_RX_BUF_SIZE);
		irqNType = USART1_IRQn;
	}
	else if (nComPort == COM2)
	{
		pIUart->hUart = &huart2;
		pIUart->pRxRingBuf = &s_RxRingBuf2;
		RBuf_Init(pIUart->pRxRingBuf, s_RxBuf2, IUART2_RX_BUF_SIZE);
		irqNType = USART2_IRQn;
	}
	else if (nComPort == COM3)
	{
		pIUart->hUart = &huart3;
		pIUart->pRxRingBuf = &s_RxRingBuf3;
		RBuf_Init(pIUart->pRxRingBuf, s_RxBuf3, IUART3_RX_BUF_SIZE);
		irqNType = USART3_IRQn;
	}
	else if (nComPort == COM4)
	{
		pIUart->hUart = &huart4;
		pIUart->pRxRingBuf = &s_RxRingBuf4;
		RBuf_Init(pIUart->pRxRingBuf, s_RxBuf4, IUART4_RX_BUF_SIZE);
		irqNType = UART4_IRQn;
	}
	else if (nComPort == COM5)
	{
		pIUart->hUart = &huart5;
		pIUart->pRxRingBuf = &s_RxRingBuf5;
		RBuf_Init(pIUart->pRxRingBuf, s_RxBuf5, IUART5_RX_BUF_SIZE);
		irqNType = UART5_IRQn;

	}
	else
	{
		return FALSE;
	}

	pIUart->hUart->Init.BaudRate 		= nBaud;
	pIUart->hUart->Init.WordLength   	= UART_WORDLENGTH_8B;
	pIUart->hUart->Init.StopBits     	= UART_STOPBITS_1;
	pIUart->hUart->Init.Parity       	= UART_PARITY_NONE;
	pIUart->hUart->Init.Mode         	= UART_MODE_TX_RX;
	pIUart->hUart->Init.HwFlowCtl    	= UART_HWCONTROL_NONE;
	pIUart->hUart->Init.OverSampling 	= UART_OVERSAMPLING_16;
	pIUart->hUart->Init.OneBitSampling 	= UART_ONE_BIT_SAMPLE_DISABLE;
	pIUart->hUart->Init.ClockPrescaler 	= UART_PRESCALER_DIV1;
	pIUart->hUart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	pIUart->hUart->AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_DISABLE;
    HAL_UART_Init(pIUart->hUart);

    HAL_NVIC_SetPriority(irqNType, 0, 1);
    HAL_NVIC_EnableIRQ(irqNType);

    /* FIFO Enable
    // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
    ATOMIC_SET_BIT(pIUart->hUart->Instance->CR3, USART_CR3_EIE);
    // Enable the UART Parity Error interrupt and RX FIFO Threshold interrupt
    ATOMIC_SET_BIT(pIUart->hUart->Instance->CR3, USART_CR3_RXFTIE);
    // Enable the UART Parity Error interrupt and Data Register Not Empty interrupt
    ATOMIC_SET_BIT(pIUart->hUart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
	*/

    SET_BIT(pIUart->hUart->Instance->ICR, USART_ICR_TCCF);   // Set Clear transfer complete flag
    SET_BIT(pIUart->hUart->Instance->RQR, USART_RQR_RXFRQ);  // Set Receive Data flush Request
    // USART_CR1_PEIE | USART_CR1_RXNEIE
    ATOMIC_SET_BIT(pIUart->hUart->Instance->CR1, USART_CR1_RXNEIE); // Set RX 수신 인터럽트

    pIUart->fIsOpen = TRUE;


	return TRUE;
}



void IUart_Close(int nComPort)
{
	if (nComPort < IUART_STARTPORT || nComPort >= (IUART_STARTPORT + IUART_MAXPORT))
		return;

	if (iUartTable[nComPort].fIsOpen)
	{
		HAL_UART_DeInit(iUartTable[nComPort].hUart);
	}
}


int IUart_SendByte(int nComPort, BYTE bCh)
{
	if (nComPort < IUART_STARTPORT || nComPort >= (IUART_STARTPORT + IUART_MAXPORT))
		return -1;

	while ((iUartTable[nComPort].hUart->Instance->ISR & UART_FLAG_TXE) == RESET);
	iUartTable[nComPort].hUart->Instance->TDR = bCh;

	return bCh;
}



int IUart_SendBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	if (nComPort < IUART_STARTPORT || nComPort >= (IUART_STARTPORT + IUART_MAXPORT))
		return -1;

	for (int i = 0; i < usLen; i++)
		IUart_SendByte(nComPort, pBuf[i]);

	return usLen;
}



int IUart_RecvByte(int nComPort)
{
	BYTE	bRecvByte;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

	if (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) > 0)
	{
		RBuf_Read(iUartTable[nComPort].pRxRingBuf, &bRecvByte, 1);
	}
	else
	{
		return -1;
	}

	return bRecvByte;
}


int IUart_RecvByte_check(int nComPort, BYTE *pData)
{
    BYTE    bRecvByte;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

	if (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) > 0)
    {
    	RBuf_Read(iUartTable[nComPort].pRxRingBuf, &bRecvByte, 1);

        *pData = bRecvByte;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}


int IUart_RecvBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	UINT32 nReaded;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

	if (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) > 0)
	{
		nReaded = RBuf_Read(iUartTable[nComPort].pRxRingBuf, pBuf, usLen);
	}
	else
	{
		return -1;
	}

	return nReaded;
}


int IUart_RecvByteBlocked(int nComPort)
{
	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

   // while (RBuf_Available(iUartTable[nComPort].pRxRingBuf) == 0)
    while (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) == 0)
        ;

    return IUart_RecvByte(nComPort);
}



int IUart_RecvBufferBlocked(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	UINT32 nReaded;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

    while (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) >= usLen)
        ;

    nReaded = RBuf_Read(iUartTable[nComPort].pRxRingBuf, pBuf, usLen);

    return nReaded;
}



int IUart_RecvByteTimed(int nComPort, int nTimeout)
{
	uint32_t startTick;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

	startTick = bsp_GetTickCount();
	while (bsp_GetTickCount() - startTick <= nTimeout)
	{
		if (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) > 0)
			return IUart_RecvByte(nComPort);
	}

	return -1;  // Timeout
}


/*
**  주언진 Timeout안에 usLen만 큼의 데이터를 수신 한다.
**
**
**  리턴 : 시간안에 수신한 바이트 수, 에러시 -1
 */
int IUart_RecvBufferTimed(int nComPort, PUCHAR pBuf, UINT16 usLen, int nTimeout)
{
    UINT16 nRecv = 0;
	uint32_t startTick;

	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return -1;

	startTick = bsp_GetTickCount();
	while (bsp_GetTickCount() - startTick <= nTimeout)
    {
        if (nRecv >= usLen)
            break;

		if (RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf) > 0)
        {
            pBuf[nRecv++] = IUart_RecvByte(nComPort);
            continue;
        }
    }

    return (nRecv);  // Timeout
}



BOOL IUart_IsRxEmpty(int nComPort)
{
	UINT32 	nRxLength;
	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return TRUE;	// 에러시 Empty 취급

	nRxLength = RBuf_Get_Full(iUartTable[nComPort].pRxRingBuf);
	return (nRxLength == 0);
}


void IUart_ClearRxBuffer(int nComPort)
{
	if (nComPort < IUART_STARTPORT || nComPort > IUART_ENDPORT)
		return;

	RBuf_Reset(iUartTable[nComPort].pRxRingBuf);
}



/*
** ==========================================================================
**	USART 인터럽트 서비스 루틴
** ==========================================================================
*/

// 내부 USART/UART 인터럽트 서비스 처리 루틴
void IUartISR(IUART_T* pIUart)
{
    uint32_t isrflags   = READ_REG(pIUart->hUart->Instance->ISR);

    // 수신 인터럽트 처리
    if ((isrflags & USART_ISR_RXNE_RXFNE) != RESET)
    {
        uint8_t ch;

        ch = (uint8_t)(READ_REG(pIUart->hUart->Instance->RDR) & 0x00FF);
        RBuf_Write(pIUart->pRxRingBuf, &ch, 1);
    }

    // Clear Interrupt Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_PEF);     // Parity Error Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_FEF);     // Framing Error Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_NEF);     // Noise detected Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_OREF);    // OverRun Error Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_IDLEF);   // IDLE line detected Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_TCF);     // Transmission Complete Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_LBDF);    // LIN Break Detection Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_CTSF);    // CTS Interrupt Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_CMF);     // Character Match Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_WUF);     // Wake Up from stop mode Clear Flag
    SET_BIT(pIUart->hUart->Instance->ICR, UART_CLEAR_TXFECF);  // TXFIFO empty Clear Flag
}


void USART1_IRQHandler(void)
{
    IUartISR(&iUartTable[COM1]);
}

void USART2_IRQHandler(void)
{
    IUartISR(&iUartTable[COM2]);
}

void USART3_IRQHandler(void)
{
    IUartISR(&iUartTable[COM3]);
}

void UART4_IRQHandler(void)
{
    IUartISR(&iUartTable[COM4]);
}

void UART5_IRQHandler(void)
{
    IUartISR(&iUartTable[COM5]);
}





