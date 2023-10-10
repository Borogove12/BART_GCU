
#ifndef _BSP_UART_H_
#define _BSP_UART_H_


#define COM1    0       // DEBUG Port
#define COM2    1       // Not Used
#define COM3    2       // ECU Port
#define COM4    3       // primary barrier
#define COM5    4       // secondary barrier

#define COM_MAX			16

#define IUART_STARTPORT 	COM1
#define IUART_MAXPORT   	5			// 최대 포트 수
#define IUART_ENDPORT		COM5

#if defined(STRAFFIC_GCU)
    #define DEBUG_PORT  	COM1
    #define ECU_PORT    	COM3
    #define P_BARRIER_PORT 	COM4
    #define s_BARRIER_PORT 	COM5

#else
    #define DEBUG_PORT      COM3
#endif



// Baudrate
#if 0
#define BAUD_1200       1200
#define BAUD_2400       2400
#define BAUD_4800       4800
#define BAUD_9600       9600
#define BAUD_19200      19200
#define BAUD_38400      38400
#define BAUD_57600      57600
#define BAUD_115200     115200
#define BAUD_230400     230400
#define BAUD_460800     460800
#define BAUD_921600     921600
#endif

// Parity
#define PARITY_NONE     0
#define PARITY_ODD      1
#define PARITY_EVEN     2
#define PARITY_MARK     3
#define PARITY_SPACE    4
#define PARITY_MASK     5



void bsp_InitUart();

BOOL Uart_Open(int nComPort, UINT32 uBaudrate);

int Uart_SendByte(int nComPort, BYTE bCh);
int Uart_SendBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen);

int Uart_RecvByte(int nComPort);
int Uart_RecvBuffer(int nComPort, PUCHAR pBuf, UINT16 usLen);
int Uart_RecvByteBlocked(int nComPort);
int Uart_RecvBufferBlocked(int nComPort, PUCHAR pBuf, UINT16 usLen);
int Uart_RecvByteTimed(int nComPort, int nTimeout);
int Uart_RecvBufferTimed(int nComPort, PUCHAR pBuf, UINT16 usLen, int nTimeout);

BOOL Uart_IsRxEmpty(int nComPort);
void Uart_ClearRxBuffer(int nComPort);

void Uart_Printf(int nComPort, char *fmt, ...);
void Uart_HexaDump(int nComport, PUCHAR pBuf, UINT16 usLen, char *fmt, ...);

int GetInputStr(PCHAR pBuf, int nBufSize);

void Uart_Test(void);

/*
** ============================================================================
** 	기존 소스 및 System 호환 함수 정의
** ============================================================================
*/
int __io_putchar(int ch);
int __io_getchar(void);

void _low_level_put(char c);
int	kbhit(void);
int	pause(void);
int _putchar(int c);
void putch(int c);
int getch(void);
void putstr(char *s);
void cputs(char *s);
void HexPrint(WORD h, uint8_t n);
void HexCprint(WORD h, uint8_t n);
int sc_qhit(int com);	// int qhit(QUEUE *q); 대체 함수
void sc_putc(int com, char c);
int sc_getc(int com, int tmout);
void sc_flush(int com);


#endif /* INC_BSP_UART_H_ */
