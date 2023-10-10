#include "bsp.h"
#include <stdio.h>
#include <stdarg.h>

void bsp_InitUart()
{
	bsp_InitIUart();
}



BOOL Uart_Open(int nComPort, UINT32 uBaudrate)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
		return (IUart_Open(nComPort, uBaudrate));
	return FALSE;
}



int Uart_SendByte(int nComPort, BYTE c)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
		return (IUart_SendByte(nComPort, c));

	return -1;
}



int Uart_SendBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
		return (IUart_SendBuffer(nComPort, pBuf, usLen));

	return -1;
}



int Uart_RecvByte(int nComPort)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
		return (IUart_RecvByte(nComPort));

    return -1;
}



int Uart_RecvBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_RecvBuffer(nComPort, pBuf, usLen));

    return -1;
}


int Uart_RecvByteBlocked(int nComPort)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_RecvByteBlocked(nComPort));

    return -1;
}


int Uart_RecvBufferBlocked(int nComPort, PUCHAR pBuf, UINT16 usLen)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_RecvBufferBlocked(nComPort, pBuf, usLen));

    return -1;
}


int Uart_RecvByteTimed(int nComPort, int nTimeout)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_RecvByteTimed(nComPort, nTimeout));

    return -1;
}


int Uart_RecvBufferTimed(int nComPort, PUCHAR pBuf, UINT16 usLen, int nTimeout)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_RecvBufferTimed(nComPort, pBuf, usLen, nTimeout));

    return -1;
}



BOOL Uart_IsRxEmpty(int nComPort)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_IsRxEmpty(nComPort));

    return TRUE;
}




void Uart_ClearRxBuffer(int nComPort)
{
	if (nComPort >= IUART_STARTPORT && nComPort <= IUART_ENDPORT)
        return (IUart_ClearRxBuffer(nComPort));

}



void Uart_Printf(int nComPort, char *fmt, ...)
{
	char	prtBuf[256];
	int nRet;
	va_list	arg_ptr;

	va_start(arg_ptr, fmt);
	nRet = vsprintf(prtBuf, fmt, arg_ptr);
	va_end(arg_ptr);

	if (nRet > 0)
		Uart_SendBuffer(nComPort, (PUCHAR)prtBuf, nRet);
}



void Uart_Test(void)
{
	int ch1, ch9;

	Uart_Printf(COM1, "COM1 UART send Test!!!\r\n");
	Uart_Printf(COM5, "COM5 UART send Test!!!\r\n");

	Uart_Printf(COM1, "for Exit Press 'q'\r\n");
	Uart_Printf(COM5, "for Exit Press 'q'\r\n");

	while (1)
	{
		ch1 = Uart_RecvByte(COM1);
		ch9 = Uart_RecvByte(COM5);
		if (ch1 == -1 && ch9 == -1)
			continue;
		if (ch1 == 'q' || ch9 == 'q')
			break;
		if (ch1 != -1)
			Uart_SendByte(COM5, (BYTE)ch1);
		if (ch9 != -1)
			Uart_SendByte(COM1, (BYTE)ch9);
	}
	printf("UART Test End!!!\n");
}


/*
*******************************************************************************
*           호환성 함수 정의
*******************************************************************************
*/

/*
**	CubeIde 호환 함수
 */
int __io_putchar(int ch)
{
	if (ch == '\n')
		_low_level_put('\r');
	_low_level_put((char)ch);

	return ch;
}


// scanf 사용시 필요
int __io_getchar(void)
{
    char data[4];
    uint8_t ch, len = 1;

    ch = (uint8_t)Uart_RecvByteBlocked(DEBUG_PORT);

    memset(data, 0x00, 4);
    switch(ch)
    {
        case '\r':
        case '\n':
            len = 2;
            sprintf(data, "\r\n");
            break;

        case '\b':
        case 0x7F:
            len = 3;
            sprintf(data, "\b \b");
            break;

        default:
            data[0] = ch;
            break;
    }
    Uart_SendBuffer(DEBUG_PORT, (uint8_t *)data, len);

    return ch;
}



void _low_level_put(char c)
{
    Uart_SendByte(DEBUG_PORT, c);

//	USART1->TDR = c;
//
//	while((USART1->ISR & USART_ISR_TC) == 0)
//	{}
}



BOOL kbhit()
{
    return (Uart_IsRxEmpty(DEBUG_PORT) == FALSE);
}



int getch(void)
{
    while (!kbhit())
        ;

    return Uart_RecvByte(DEBUG_PORT);
}



int _putchar(int c)
{
    if ( c == '\n' )
        _low_level_put((char)'\r');
    _low_level_put((char)c);

    return(c);
}



void putch(int c)
{
    _low_level_put(c);
}



int pause(void)
{
    cputs("\nPress any key to continue.");
    return  getch();
}



void putstr(char *s)
{
    while (*s != '\0')
        _putchar(*s++);
}



void cputs(char *s)
{
    while (*s != '\0')
        _putchar(*s++);
}


/* printf("%0?x", h) */
void HexPrint(WORD h, uint8_t n)
{
    char    s[8];

    htoa(h, s);
    n = 4 - n;
    while ( n < 4 )
        putchar(s[n++]);
}



void HexCprint(WORD h, uint8_t n)
{
    char    s[8];

    htoa(h, s);
    n = 4 - n;
    while ( n < 4 )
        putch(s[n++]);
}



int sc_qhit(int com)
{
    return (Uart_IsRxEmpty(com) == FALSE);
}



void sc_putc(int com, char c)
{
    Uart_SendByte(com, (UCHAR)c);
}



int sc_getc(int com, int tmout)
{
    if (tmout < 0)
        tmout = ~tmout;
    else
        tmout *= 1000;

    return (Uart_RecvByteTimed(com, tmout));
}



void sc_flush(int com)
{
    Uart_ClearRxBuffer(com);
}



int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here     */
    /* e.g. write a character to a UART, or to the */
    /* debugger console with SWI WriteC            */
    _putchar(ch);
    return ch;
}



int ferror(FILE *f)
{
    /* Your implementation of ferror */
    return EOF;
}



int fgetc(FILE *f)
{
    BYTE dat;

    dat = getch();
    _putchar(dat);

    if (dat == '\r')
        dat = '\n';

    return(dat);
}



int GetInputStr(PCHAR pBuf, int nBufSize)
{
    int nPos = 0;

    while (nPos < nBufSize)
    {
        int c = getch();
        switch (c)
        {
            case BREAK:
                putstr("^C");
                return(-1);

            case ESC:
                return(-1);

            case '\r' :
            case '\n' :
                pBuf[nPos] = '\0';
                if(nPos == 0)
                    return(0);
                return  nPos;

            case '\b':
                if (nPos) {
                    _putchar('\b');
                    _putchar(' ');
                    _putchar('\b');
                    nPos--;
                }
                break;

            default:
                _putchar(c);
                pBuf[nPos] = c;
                nPos++;
                break;
        }

//        if (!kbhit())
//            return 0;
    }
    pBuf[nPos] = '\0';

    return (-1); // length error
}



void Uart_HexaDump(int nComPort, PUCHAR pBuf, UINT16 usLen, char *fmt, ...)
{
    char prtBuf[256];
    int nRet;
    va_list arg_ptr;
    UINT16 i, baseIndex;

    va_start(arg_ptr, fmt);
    nRet = vsprintf(prtBuf, fmt, arg_ptr);
    va_end(arg_ptr);

    Uart_SendBuffer(nComPort, (PUCHAR)prtBuf, nRet);

    i = 0;
    while (i < usLen)
    {
        baseIndex = i;
        Uart_Printf(nComPort, "\r\n%04X: ", baseIndex);

        for (int j = 0; j < 16; j++)
        {
            if (j != 0)
                Uart_SendBuffer(nComPort, (PUCHAR)" ", 1);
            if (i < usLen)
            {
                Uart_Printf(nComPort, "%02X", pBuf[i]);
                i++;
            }
        }
    }
    Uart_Printf(nComPort, "\r\n");
}


void U3_Putch(BYTE c)
{
	IUart_SendByte(COM3, (uint8_t)c);
}


