
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "integer.h"
#include "struct.h"
#include "bsp_types.h"
//#include "gcu_def.h"


#define COM_PORT_DEBUG			COM1


#ifndef max
#define max(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b)	((a) > (b) ? (b) : (a))
#endif

#define memclr(s,n)			memset(s,0,n)
#define	DEC2BCD(dec)		((((dec)/10)<<4)|(((dec)%10)&0xf))
#define	BCD2DEC(bcd)		((((bcd)>>4&0xf)*10)+((bcd)&0xf))
#define	ROUNDUP(a, u)		(((a)+(u)-1)/(u)*(u))
#define	ROUNDOFF(a, u)		((a)/(u)*(u))
#define htonbo(d, s, n) 	ChangeByteOrder(d, s, n)
#define ntohbo(d, s, n) 	ChangeByteOrder(d, s, n)
#define	lowbyte(x)			((x) & 0xff)
#define	highbyte(x)			lowbyte((x) >> 8)
#define	dim(x)				(sizeof(x) / sizeof(x[0]))

#define BV(bit)     		(1<<(bit))
#define cbi(sfr,bit)  		(sfr &= ~BV(bit))
#define sbi(sfr,bit)  		(sfr |= BV(bit))
#define setbit(port,bit) 	((port) |= BV(bit))
#define setbits(port,mask)  ((port) |= mask)
#define clrbit(port,bit) 	((port) &= ~BV(bit))
#define clrbits(port,mask)  ((port) &= ~(mask))
#define testbit(port,bit) 	((port) & BV(bit))
#define togglebit(port,bit) ((port) ^= BV(bit))

// 32 Bits Read/Write
#define indw(addr)			(*(volatile unsigned int *)(addr))
#define outdw(addr, data)	((*(volatile unsigned int *)(addr)) = (data))

// 16 Bits Read/Write
#define inw(addr)			(*(volatile unsigned short *)(addr))
#define outw(addr, data)	((*(volatile unsigned short *)(addr)) = (word)(data))

// 8 Bits Read/Write
//#define inb(addr)			(*(volatile unsigned char *)(addr))
//#define outb(addr, data)	((*(volatile unsigned char *)(addr)) = (byte)(data))


#if _MAIN
#define	GLOBAL// declare
#else
#define	GLOBAL	extern
#endif

/* main.c */
extern 	const char Version[];
extern 	const char MyName[];
extern 	const char PgmDate[];
extern 	CONFIG 	config;
word dip_sw(void);
void CMD_Test(byte bVal); 			//temp
byte extinb(byte Add);				//test Read Address temp

/* time.c */

void	TimeOfDay(void);
//void 	delay(int dly);
void	sleep(int t);
void 	msleep(int t);
clock_t	ticktime(void);

clock_t mstime(clock_t *tp);
int 	timeout(time_t *tp, int sec);
int     mstimeout(clock_t *cp, int msec);

int		setdate(struct tm *t, char *str);
time_t	asc2time(byte *asc);

void    TimerService(void);
int     timer(void (*func)(), int arg, int sec);
int     untimer(int tmid);
int     timer_id(void);
int     timer_in(int tmid);
void    timer_out(int tmid);
dword   mclocktime(void);
dword   clocktime(void);


time_t 	SystemInitTime(void);

#define DISABLE_INT()
#define ENABLE_INT()


#define READ_BASE     		0x60000000
#define WRITE_BASE     		0x64000000


#define READ00_ADR      	(READ_BASE+0x00U) 	// nPSEN1_CS
#define READ01_ADR      	(READ_BASE+0x100U)	// nPSEN2_CS
#define READ02_ADR      	(READ_BASE+0x300U) 	// nDOOR_SW_CS
#define READ03_ADR      	(READ_BASE+0x500U)	// nSCADA_IN_CS
#define READ04_ADR      	(READ_BASE+0x600U) 	// nBRR_IN_CS

#define WRITE00_ADR      	(WRITE_BASE+0x00U) 	// n5V_LED_CS
#define WRITE01_ADR      	(WRITE_BASE+0x100U)	// n24V_LED_CS
#define WRITE02_ADR      	(WRITE_BASE+0x200U) // nBRR_OUT_CS
#define WRITE03_ADR      	(WRITE_BASE+0x400U)	// nSCADA_OUT_CS
/*
#define wnP_LED_CS_ADDR       (*(volatile unsigned char *)(WRITE_BASE + (0x00U))) // out
#define wnS_LED_CS_ADDR       (*(volatile unsigned char *)(WRITE_BASE + (0x100U))) // out
#define wnBRR_OUT_CS_ADDR     (*(volatile unsigned char *)(WRITE_BASE + (0x200U))) // out
#define wnSCADA_OUT_CS_ADDR   (*(volatile unsigned char *)(WRITE_BASE + (0x400U))) // out
*/

#define wnP_LED_CS_ADDR       (*(__IO uint8_t *)(WRITE_BASE + (0x00U))) // out
#define wnS_LED_CS_ADDR       (*(__IO uint8_t *)(WRITE_BASE + (0x100U))) // out
#define wnBRR_OUT_CS_ADDR     (*(__IO uint8_t *)(WRITE_BASE + (0x200U))) // out
#define wnSCADA_OUT_CS_ADDR   (*(__IO uint8_t *)(WRITE_BASE + (0x400U))) // out
#define inb(addr)			(*(volatile unsigned char *)(addr))
#define outb(addr, data)	((*(volatile unsigned char *)(addr)) = (byte)(data))

bool     uartInit(void);


void Buzzer(UINT8 nBuzzerControl);
#endif
