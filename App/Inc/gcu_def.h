/*******************************************************************************
*       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : gcu_def.h
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : Definitions for GCU
********************************************************************************/

#ifndef __GCU_DEF_H
#define __GCU_DEF_H

#include "global.h"
/* FW Version String ---------------------------------------------------------*/
#define GCU_FW_VERSION "0.0.0.1"
#define TG_TIMER 2		// 2 s
#define SWING_BARRIER

#define GCU_BAUD		57600
#define WDG_TOGGLE_PERIOD 	25

#define BART_IO		1


/* Interrupt Priority (range: 0(low) ~ 15(high) ------------------------------*/
#define PRIORITY_WDG   15	// WDG, Passenger Sensor Logic
#define PRIORITY_UART0 1	// Debugger Port
#define PRIORITY_UART1 12	// Master Swing barrier
#define PRIORITY_UART2 9	// Main Communication
#define PRIORITY_UART3 13	// Sub Swing barrier

/* GCU Type Definitions ------------------------------------------------------*/
typedef unsigned long	DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;

typedef long	LONG;
typedef int		INT;
typedef short	SHORT;
typedef char	CHAR;

typedef volatile unsigned long	VDWORD;
typedef volatile unsigned short	VWORD;
typedef volatile unsigned char	VBYTE;

typedef volatile long   VLONG;
typedef volatile int	VINT;
typedef volatile short  VSHORT;
typedef volatile char   VCHAR;

typedef unsigned long	ULONG;
typedef unsigned int	UINT;
typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;

typedef volatile unsigned long	VULONG;
typedef volatile unsigned int	VUINT;
typedef volatile unsigned short	VUSHORT;
typedef volatile unsigned char	VUCHAR;

/* GCU Constant Definitions --------------------------------------------------*/
//typedef enum { SUCCESS = 0, FAIL  = !SUCCESS} FunctionResult;

/* GPIO PIN Definitions ------------------------------------------------------*/
typedef enum { 
	GPIO_PIN0	= 0x0001,
	GPIO_PIN1	= 0x0002,
	GPIO_PIN2	= 0x0004,
	GPIO_PIN3	= 0x0008,
	GPIO_PIN4	= 0x0010,
	GPIO_PIN5	= 0x0020,
	GPIO_PIN6	= 0x0040,
	GPIO_PIN7	= 0x0080,
	GPIO_PIN8	= 0x0100,
	GPIO_PIN9	= 0x0200,
	GPIO_PIN10	= 0x0400,
	GPIO_PIN11	= 0x0800,
	GPIO_PIN12	= 0x1000,
	GPIO_PIN13	= 0x2000,
	GPIO_PIN14	= 0x4000,
	GPIO_PIN15	= 0x8000
} GPIO_PIN_DEF;

enum {
	BIT1	= 0x01,
	BIT2	= 0x02,
	BIT3	= 0x04,
	BIT4	= 0x08,
	BIT5	= 0x10,
	BIT6	= 0x20,
	BIT7	= 0x40,
	BIT8	= 0x80
};
#if 0
enum {
	OFF = 0,
	ON	= 1
};
#endif

enum {
	CHK_OFF		= 0,
	CHK_ALL		= 1,
	CHK_ONE		= 2,
};

enum
{
	STANDARD = 0,
	WIDE = 1
};

enum
{
	BARRIER_SWING = 0,
	BARRIER_RET = 1,
	BARRIER_OTHER = 2,
};

enum
{
	PASS_PATN_NONE = 0,

	// Dir Entry Admin (Approaching) (bit: xxxx x#3#2#1)
	PASS_PATN_ADMA_HIGH = 0x01,															  // sen 1
	PASS_PATN_ADMA_LOW1 = 0x02,															  // sen 2
	PASS_PATN_ADMA_LOW2 = 0x04,															  // sen 3
	PASS_PATN_ADMA_ALL = PASS_PATN_ADMA_HIGH | PASS_PATN_ADMA_LOW1 | PASS_PATN_ADMA_LOW2, // sen 1,2,3
	PASS_PATN_ADMA_HLOW1 = PASS_PATN_ADMA_LOW1 | PASS_PATN_ADMA_HIGH,					  // sen 1,2
	PASS_PATN_ADMA_HLOW2 = PASS_PATN_ADMA_HIGH | PASS_PATN_ADMA_LOW2,					  // sen 1,3
	PASS_PATN_ADMA_LOW = PASS_PATN_ADMA_LOW1 | PASS_PATN_ADMA_LOW2,						  // sen 2,3

	// Dir Exit Admin (Approaching) (bit: xxxx x#19#18#17)
	PASS_PATN_ADMB_HIGH = 0x04,															  // sen 19
	PASS_PATN_ADMB_LOW1 = 0x02,															  // sen 18
	PASS_PATN_ADMB_LOW2 = 0x01,															  // sen 17
	PASS_PATN_ADMB_ALL = PASS_PATN_ADMB_HIGH | PASS_PATN_ADMB_LOW1 | PASS_PATN_ADMB_LOW2, // sen 17,18,19
	PASS_PATN_ADMB_HLOW1 = PASS_PATN_ADMB_HIGH | PASS_PATN_ADMB_LOW1,					  // sen 18,19
	PASS_PATN_ADMB_HLOW2 = PASS_PATN_ADMB_HIGH | PASS_PATN_ADMB_LOW2,					  // sen 17,19
	PASS_PATN_ADMB_LOW = PASS_PATN_ADMB_LOW1 | PASS_PATN_ADMB_LOW2,						  // sen 17,18

	// Safety
	PASS_PATN_HIGH_EN = 0x01, // sen 8
	PASS_PATN_LOW_EN = 0x02,  // sen 9
	PASS_PATN_LOW_EX = 0x08,  // sen 11
	PASS_PATN_HIGH_EX = 0x10, // sen 12

	PASS_PATN_ALL = PASS_PATN_HIGH_EN | PASS_PATN_LOW_EN | PASS_PATN_HIGH_EX | PASS_PATN_LOW_EX, // sen 8,9,11,12
	PASS_PATN_HIGH = PASS_PATN_HIGH_EX | PASS_PATN_HIGH_EN,										 // sen 8,12,
	PASS_PATN_LOW = PASS_PATN_LOW_EN | PASS_PATN_LOW_EX,										 // sen 9,11,
	PASS_PATN_HIGH_LOW_EN = PASS_PATN_HIGH_EN | PASS_PATN_HIGH_EX | PASS_PATN_LOW_EN,			 // sen 8,12,11
	PASS_PATN_HIGH_LOW_EX = PASS_PATN_HIGH_EN | PASS_PATN_HIGH_EX | PASS_PATN_LOW_EX,			 // sen 8,12,9
	PASS_PATN_ALL_EN = PASS_PATN_HIGH_EN | PASS_PATN_LOW_EN,									 // sen 11,12
	PASS_PATN_ALL_EX = PASS_PATN_HIGH_EX | PASS_PATN_LOW_EX,									 // sen 8, 9

	// For the swing barrier
	PASS_PATN_ENTER = 0x03,		 // EN: #1#2 / EX: #1#2
	PASS_PATN_FORCE_OPEN = 0x04, // EN: #3 / EX: #3
	PASS_PATN_PASS_READY = 0x04, // EN: #6 / EX: #16
	PASS_PATN_PASS = 0x12,		 // sen #11,12,13,14
	PASS_PATN_PASS_OVER = 0x13,	 // sen #17
};

/* GPIO0 PIN Definitions -----------------------------------------------------*/
#define UART3_TX_PIN	GPIO_PIN0	// P0.0
#define UART3_RX_PIN 	GPIO_PIN1	// P0.1
#define RESERVE_PIN2	GPIO_PIN2	// P0.2
#define RESERVE_PIN3	GPIO_PIN3	// P0.3
#define WATCH_DOG_PIN	GPIO_PIN4	// P0.4
#define EMG_FB_PIN		GPIO_PIN5	// P0.5
#define RESERVE_PIN6 	GPIO_PIN6	// P0.6
#define RESERVE_PIN7 	GPIO_PIN7	// P0.7
#define UART0_RX_PIN	GPIO_PIN8	// P0.8
#define UART0_TX_PIN	GPIO_PIN9	// P0.9
#define UART1_RX_PIN	GPIO_PIN10	// P0.10
#define UART1_TX_PIN	GPIO_PIN11	// P0.11
#define RESERVE_PIN12 	GPIO_PIN12	// P0.12
#define UART2_RX_PIN	GPIO_PIN13	// P0.13
#define UART2_TX_PIN	GPIO_PIN14	// P0.14
#define RESERVE_PIN15	GPIO_PIN15	// P0.15

/* GPIO1 PIN Definitions -----------------------------------------------------*/
#define DIP_SW_PIN		0xFF
#define DIP_SW1_PIN		GPIO_PIN0	// P1.0
#define DIP_SW2_PIN		GPIO_PIN1	// P1.1
#define DIP_SW3_PIN		GPIO_PIN2	// P1.2
#define DIP_SW4_PIN		GPIO_PIN3	// P1.3
#define DIP_SW5_PIN		GPIO_PIN4	// P1.4
#define DIP_SW6_PIN		GPIO_PIN5	// P1.5
#define DIP_SW7_PIN		GPIO_PIN6	// P1.6
#define DIP_SW8_PIN		GPIO_PIN7	// P1.7
#define GCU_OUT5_PIN	GPIO_PIN8	// P1.8
#define GCU_OUT6_PIN	GPIO_PIN9	// P1.9
									// P1.8~11
#define DIRA_ACK_PIN	GPIO_PIN12	// P1.12
#define DIRB_ACK_PIN	GPIO_PIN13	// P1.13
#define EMERGENCY_PIN	GPIO_PIN14	// P1.14
#define TCU_INLET_PIN	GPIO_PIN15	// P1.15

/* GPIO2 PIN Definitions -----------------------------------------------------*/
#define GPIO_CS2_PIN	GPIO_PIN2	// P2.2
#define GPIO_CS3_PIN	GPIO_PIN3	// P2.3
#define RETURN_LED_PIN	GPIO_PIN4	// P2.4
#define BUZZER_PIN		GPIO_PIN5	// P2.5
#define EN_BUZZER_PIN	GPIO_PIN6	// P2.6
#define EX_BUZZER_PIN	GPIO_PIN7	// P2.7
#define EXT_INT2_PIN	GPIO_PIN8	// P2.8
#define EXT_INT3_PIN	GPIO_PIN9	// P2.9
#define RFID1_SEL_PIN	GPIO_PIN12	// P2.12
#define RFID2_SEL_PIN	GPIO_PIN13	// P2.13
#define RFID1_PWR_PIN	GPIO_PIN14	// P2.14
#define RFID2_PWR_PIN	GPIO_PIN15	// P2.15
#if 0
/* GCU IO (EXTMEM 2) Address Definitions -------------------------------------*/
#define ADDR_RD_SW1		0x64000001	// Door/Module switch 1
#define ADDR_RD_SW2		0x64000002	// Door/Module switch 2
#define ADDR_RD_TSEN	0x64000003	// Token capture unit sensor
#define ADDR_RD_PSEN1	0x64000004	// Passenger sensor 1
#define ADDR_RD_PSEN2	0x64000005	// Passenger sensor 2
#define ADDR_RD_PSEN3	0x64000006	// Passenger sensor 3
#define ADDR_RD_SPSEN	0x64000007	// Spare sensor (WMATA not used)
#define ADDR_RD_FDOOR	0x64000008	// Sector door switch
#define ADDR_RD_ECUOUT	0x64000009	// ECU Output
#define ADDR_RD_EXTIRQ	0x6400000A	// External IRQ source
#define ADDR_RD_UPSSTAT	0x6400000B  // UPS Status
#define ADDR_RD_CPLDVER 0x6400000F  // CPLD Version

#define ADDR_WR_FDOOR	0x64000011	// Control Flap door
#define ADDR_WR_SOLSIG	0x64000012	// Control solenoids of token capture unit (WMATA not used)
#define ADDR_WR_SOLPWM  0x64000013	// Control solenoids of token capture unit by PWM mode (WMATA not used)
#define ADDR_WR_MDIR	0x64000014	// Control master direction display
#define ADDR_WR_SDIR	0x64000015	// Control slave direction display
#define ADDR_WR_DDIR	0x64000016	// Control dummy direction display (WMATA not used)
#define ADDR_WR_MLAMP	0x64000017	// Control master lamp
#define ADDR_WR_SLAMP	0x64000018	// Control slave lamp
#define ADDR_WR_UPSCMD 	0x64000019	// UPS Command
////////////////////////////////////////////////////////////////////////////////////
/* GPIO1 PIN Definitions -----------------------------------------------------*/

#define DIP_SW_PIN		0xFF

#define DIP_SW1_PIN		GPIO_PIN0// P1.0  -BART DIP SW1

#define DIP_SW2_PIN		GPIO_PIN1// P1.1  -BART DIP SW2

#define DIP_SW3_PIN		GPIO_PIN2// P1.2  -BART DIP SW3

#define DIP_SW4_PIN		GPIO_PIN3// P1.3  -BART DIP SW4

#define DIP_SW5_PIN		GPIO_PIN4// P1.4  -BART DIP SW5

#define DIP_SW6_PIN		GPIO_PIN5// P1.5  -BART DIP SW6

#define DIP_SW7_PIN		GPIO_PIN6// P1.6  -BART DIP SW7

#define DIP_SW8_PIN		GPIO_PIN7// P1.7  -BART DIP SW8

#define GCU_OUT5_PIN	GPIO_PIN8// P1.8   (회로상 미연결)

#define GCU_OUT6_PIN	GPIO_PIN9// P1.9   (회로상 미연결)

// P1.8~11

#define DIRA_ACK_PIN	GPIO_PIN12// P1.12   (회로상 미연결)

#define DIRB_ACK_PING	PIO_PIN13// P1.13   (회로상 미연결)

#define EMERGENCY_PIN	GPIO_PIN14// P1.14   (회로상 미연결)

#define TCU_INLET_PIN	GPIO_PIN15// P1.15   (회로상 미연결)

​

/* GPIO2 PIN Definitions -----------------------------------------------------*/

#define GPIO_CS2_PIN GPIO_PIN2// P2.2

#define GPIO_CS3_PIN GPIO_PIN3// P2.3

#define RETURN_LED_PIN GPIO_PIN4// P2.4   (회로상 미연결)

#define BUZZER_PIN GPIO_PIN5// P2.5   - BART는 부저 1개만 사용 (기존 Main 부저 아니면 Entry부저로 추정됨)

#define EN_BUZZER_PIN GPIO_PIN6// P2.6  - BART는 부저 1개만 사용 (기존 Main 부저 아니면 Entry부저로 추정됨)

#define EX_BUZZER_PIN GPIO_PIN7// P2.7  - BART는 부저 1개만 사용

#define EXT_INT2_PIN GPIO_PIN8// P2.8 - 인터럽트 신호 BART에서 사용여부는 첨부 GCU_IO Specification 참조

#define EXT_INT3_PIN GPIO_PIN9// P2.9 - 인터럽트 신호 BART에서 사용여부는 첨부 GCU_IO Specification 참조

#define RFID1_SEL_PIN GPIO_PIN12// P2.12 - Token box 없음

#define RFID2_SEL_PIN GPIO_PIN13// P2.13 - Token box 없음

#define RFID1_PWR_PIN GPIO_PIN14// P2.14 - Token box 없음

#define RFID2_PWR_PIN GPIO_PIN15// P2.15 - Token box 없음

​
//CMDReadRegister 매치
/* GCU IO (EXTMEM 2) Address Definitions -------------------------------------*/

#define ADDR_RD_SW1		0x64000001// Door/Module switch 1 - BART는 Door Switch 1~4만 사용

#define ADDR_RD_SW2		0x64000002// Door/Module switch 2 - BART는 Door Switch 5~8만 사용

#define ADDR_RD_TSEN	0x64000003// Token capture unit sensor - Token box 없음

#define ADDR_RD_PSEN1	0x64000004// Passenger sensor 1 - BART는 Passenger sensor 1~8사용

#define ADDR_RD_PSEN2	0x64000005// Passenger sensor 2 - BART는 Passenger sensor 9~16사용(회로상엔 11~18로 표기)

#define ADDR_RD_PSEN3	0x64000006// Passenger sensor 3 - BART는 Passenger sensor 17, 18는 Spare로 GPIO 별도 할당(회로상엔 9, 19로 표기)

#define ADDR_RD_SPSEN	0x64000007// Spare sensor (WMATA not used)

#define ADDR_RD_FDOOR	0x64000008// Sector door switch - BART에서는 Swing barrier 사용, Switch 없음

#define ADDR_RD_ECUOUT	0x64000009// ECU Output - BART에서는 SCADA모듈과 IO인터페이스 함 (IN:5EA, OUT:5EA), 해당 어드레스를 사용해야하는지 신규로 할당해야할지는 확인 필요

#define ADDR_RD_EXTIRQ	0x6400000A// External IRQ source  - 인터럽트 신호 BART에서 사용여부는 첨부 GCU_IO Specification 참조

#define ADDR_RD_UPSSTAT	0x6400000B  // UPS Status - BART도 UPS I/F 사용 (In: 3EA)

#define ADDR_RD_CPLDVER 0x6400000F  // CPLD Version - BART는 CPLD 없음

​

#define ADDR_WR_FDOOR	0x64000011// Control Flap door - BART는 Swing barrier와 IO 인터페이스 (IN: FAULT, ANOMALY, BRAKED, GSTATUS / OUT: EMG, DIR, BRAKE, OPEN/CLOSE)

#define ADDR_WR_SOLSIG	0x64000012// Control solenoids of token capture unit (WMATA not used)

#define ADDR_WR_SOLPWM  0x64000013// Control solenoids of token capture unit by PWM mode (WMATA not used)

#define ADDR_WR_MDIR	0x64000014// Control master direction display - BART는 Primary 측 Status Display를 의미함 (RED, GREEN)- Yellow제어하지 않음

#define ADDR_WR_SDIR	0x64000015// Control slave direction display - BART는 Secondary 측 Status Display를 의미함 (RED, GREEN)- Yellow제어하지 않음

#define ADDR_WR_DDIR	0x64000016// Control dummy direction display (WMATA not used)

#define ADDR_WR_MLAMP	0x64000017// Control master lamp - BART는 Primary 측 Indicator Ligit를 의미함 (RED, BLUE, GREEN)

#define ADDR_WR_SLAMP	0x64000018// Control slave lamp - BART는 Secondary 측 Indicator Ligit를 의미함 (RED, BLUE, GREEN)

#define ADDR_WR_UPSCMD 	0x64000019// UPS Command - BART도 UPS I/F 사용 (Out: 2EA) - 현재 SHDN 1EA만 구현되어 있으나 CHK신호 추가예정

#endif
////////////////////////////////////////////////////////////////////////////////////
/* GCU IO Address Access definition ------------------------------------------*/

#if 0
#define RD_SWITCH1		(*(VBYTE*)(ADDR_RD_SW1))
#define RD_SWITCH2		(*(VBYTE*)(ADDR_RD_SW2))
#define RD_TCUSEN		(*(VBYTE*)(ADDR_RD_TSEN)) // (WMATA not used)
#define RD_PASSSEN1		(*(VBYTE*)(ADDR_RD_PSEN1))
#define RD_PASSSEN2		(*(VBYTE*)(ADDR_RD_PSEN2))
#define RD_PASSSEN3		(*(VBYTE*)(ADDR_RD_PSEN3))
#define RD_SPARESEN		(*(VBYTE*)(ADDR_RD_SPSEN)) // (WMATA not used)
#define RD_FDOORSW		(*(VBYTE*)(ADDR_RD_FDOOR))
#define RD_ECUOUT		(*(VBYTE*)(ADDR_RD_ECUOUT))
#define RD_EXTIRQ		(*(VBYTE*)(ADDR_RD_EXTIRQ))
#define RD_UPSSTAT 		(*(VBYTE*)(ADDR_RD_UPSSTAT))
#define RD_CPLDVER		(*(VBYTE*)(ADDR_RD_CPLDVER))

#define WR_FDOOR		(*(VBYTE*)(ADDR_WR_FDOOR))
#define WR_SOLSIG		(*(VBYTE*)(ADDR_WR_SOLSIG)) // (WMATA not used)
#define WR_SOLPWM		(*(VBYTE*)(ADDR_WR_SOLPWM)) // (WMATA not used)
#define WR_MDIR			(*(VBYTE*)(ADDR_WR_MDIR))
#define WR_SDIR			(*(VBYTE*)(ADDR_WR_SDIR))
#define WR_DDIR			(*(VBYTE*)(ADDR_WR_DDIR))
#define WR_MLAMP		(*(VBYTE*)(ADDR_WR_MLAMP))
#define WR_SLAMP		(*(VBYTE*)(ADDR_WR_SLAMP))
#define WR_UPSCMD 		(*(VBYTE*)(ADDR_WR_UPSCMD))
#endif


#define RD_SWITCH		(*(VBYTE*)(READ02_ADR))						// global.h
//#define RD_SWITCH2	(*(VBYTE*)(ADDR_RD_SW2))
//#define RD_TCUSEN		(*(VBYTE*)(ADDR_RD_TSEN)) 					// (WMATA not used)
#define RD_PASSSEN1		(*(VBYTE*)(READ00_ADR))						// global.h
#define RD_PASSSEN2		(*(VBYTE*)(READ01_ADR))						// global.h
//#define RD_PASSSEN3		(*(VBYTE*)(ADDR_RD_PSEN3))
//#define RD_SPARESEN		(*(VBYTE*)(ADDR_RD_SPSEN)) 				// (WMATA not used)
#define RD_FDOORSW		0	//(*(VBYTE*)(ADDR_RD_FDOOR))			// SectorDoor
#define RD_ECUOUT		0	//(*(VBYTE*)(ADDR_RD_ECUOUT))
#define RD_EXTIRQ		0	//(*(VBYTE*)(ADDR_RD_EXTIRQ))
#define RD_UPSSTAT 		0	//(*(VBYTE*)(ADDR_RD_UPSSTAT))
#define RD_CPLDVER		0	//(*(VBYTE*)(ADDR_RD_CPLDVER))

#define WR_FDOOR		(*(VBYTE*)(ADDR_WR_FDOOR))
//#define WR_SOLSIG		(*(VBYTE*)(ADDR_WR_SOLSIG)) 				// (WMATA not used)
//#define WR_SOLPWM		(*(VBYTE*)(ADDR_WR_SOLPWM)) 				// (WMATA not used)
#define WR_MDIR			(*(VBYTE*)(ADDR_WR_MDIR))
#define WR_SDIR			(*(VBYTE*)(ADDR_WR_SDIR))
#define WR_DDIR			(*(VBYTE*)(ADDR_WR_DDIR))
#define WR_MLAMP		(*(VBYTE*)(ADDR_WR_MLAMP))
#define WR_SLAMP		(*(VBYTE*)(ADDR_WR_SLAMP))
#define WR_UPSCMD 		(*(VBYTE*)(ADDR_WR_UPSCMD))


/* GCU IO control function ---------------------------------------------------*/
#define inp(x)			(*(VBYTE*)(x))			// x - address
#define outp(x, y)		(*(VBYTE*)(x) = (y))	// x - address, y - value

enum
{
	UPS_CMD_CHK 	= 0x01,
	UPS_CMD_PWR_DN 	= 0x02,
};
//#define ControlUPS(x) 		(WR_UPSCMD = (x))	changed from ms
#define ControlUPS(x) 			HAL_GPIO_WritePin(nUPS_GPIO_Port,((x) * 0x040),GPIO_PIN_SET)

#define ControlUPSOn(x) 		HAL_GPIO_WritePin(nUPS_GPIO_Port,((x) * 0x080),GPIO_PIN_SET)
#define ControlUPSOff(x) 		HAL_GPIO_WritePin(nUPS_GPIO_Port,((x) * 0x080),GPIO_PIN_RESET)
enum {	
	BARRIER_FREE	= 0x00,
	BARRIER_OPEN_FOR_EN	= 0x01,
	BARRIER_OPEN_FOR_EX	= 0x02,
	BARRIER_OPEN	= 0x66,
	BARRIER_CLOSE_S = 0x03,
	BARRIER_CLOSE	= 0x77,
	

	FDOOR1_SW_OPEN	= 0x01,
	FDOOR1_SW_CLOSE	= 0x02,
	FDOOR2_SW_OPEN	= 0x10,
	FDOOR2_SW_CLOSE	= 0x20,
	FDOOR_SW_FAIL	= 0x00,
	FDOOR_SW_OPEN	= 0x11,
	FDOOR_SW_CLOSE	= 0x22,
	FDOOR_SW_EMG	= 0x88,

	MASK_FDOOR1		= 0x0F,
	MASK_FDOOR2		= 0xF0,
	MASK_FDOOR		= 0x77,
	MASK_FDOOR_PWR	= 0x88,

	MASK_SDOOR1		= 0x0F,
	MASK_SDOOR2		= 0x0F,
	MASK_PUSH		= 0x10,
	MASK_SDOOR		= 0xFF,
	MASK_POWER_CHK	= 0x07,
};
#define ControlFlapDoor(x)	(WR_FDOOR = (x))

enum {
	TSOL_NONE		= 0x00,
	TSOL_SHUTTER	= 0x01,
	TSOL_STOPPER	= 0x02,
	TSOL_ERRPATH	= 0x04,
	TSOL_SELPATH	= 0x08,

	TSOL_RET_PATH	= 0x06,
	TSOL_BOX1_PATH  = 0x02,
	TSOL_BOX2_PATH  = 0x0A,  
	TSOL_JAM_PATH	= 0x0E
};

enum {
	TSEN_INLET1		= 0x01,
	TSEN_INLET2		= 0x02,
	TSEN_INLET		= 0x03,
	TSEN_ANTENNA	= 0x04,
	TSEN_RET_PATH	= 0x08,
	TSEN_BOX2_PATH	= 0x20,
	TSEN_BOX1_PATH	= 0x10,
	TSEN_RETURNCUP1 = 0x40,
	TSEN_RETURNCUP2 = 0x80,
	TSEN_RETURNCUP	= 0xC0,
	
	MASK_JAMSENSOR	= 0x3C,
	MASK_ENDSENSOR	= 0x03
};

enum
{
	SDOOR_OPEN = 0x05,
	SDOOR_CLOSE = 0x02,	
};

enum
{
	DIR_OFF = 0x00,
	DIR_GREEN = 0x01,
	DIR_RED = 0x02,

	OHD_OFF = 0x00,
	OHD_GREEN = 0x04,
	OHD_RED = 0x08,

	MASK_LAMP_CMD = 0x07
};


enum
{
	STATE_OFF = 0x00,
	STATE_ALLOW = 0x01,
	STATE_STOP = 0x02,
	STATE_ALL = 0x03
};

enum
{
	LAMP_OFF = 0x00,
	LAMP_RED_ON = 0x01,
	LAMP_GREEN_ON = 0x02,		//origin LAMP_BLUE_ON
	LAMP_BLUE_ON = 0x04,		//origin LAMP_GREEN_ON
};


enum
{
	ENTRY_OPEN  = 0x00,
	CLOSE 		= 0x01,
	EXIT_OPEN 	= 0x02,
	OPEN		= 0x04,
	ENTRY		= 0x08,
	EXIT		= 0x10
};

#define ReadDipSwitch()		(BYTE)((GPIOB->IDR)^0xff) & 0xff

enum {
	MASK_NOMAL_MODE		= 0X00,					//add 		pms
	MASK_TEST_CAPTURE	= 0xC0,
	MASK_EMG_SIGNAL     = 0x08,				//origin
	//MASK_EMG_SIGNAL     = 0x88,		//1000 1000 //pms_test
	MASK_SWING_MODE     = 0xC0, // 1100 0000
	MASK_SELF_TEST      = 0x16, // 0001 0110	//origin
	//MASK_SELF_TEST      = 0x7E, // 0111 1110	//pms_mail0818
	MASK_JIG_TEST	    = 0x01
};

#define LED_BUZZ_ON		1
#define LED_BUZZ_OFF	0

#define BUZZ_ON		0
#define BUZZ_OFF	1

enum {
	BUZZER_NO_MAIN	= 0,
	BUZZER_NO_DIRB	= 1,
	BUZZER_NO_DIRA	= 2,

	BUZZER_COUNT	= 3,

	BUZZER_OFF		= 0,
	BUZZER_ON		= 1,
	BUZZER_PERIODIC = 2,

	MASK_BUZZER_CMD	= 3
};

#define IsEMGSignalOn()		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) // 1- On, 0 - Off

/* Bit structure - Switch 1 --------------------------------------------------*/
typedef struct {
	int Door1:		1;
	int	Door2:		1;
	int Door3:		1;
	int Door4:		1;
	int Door5:		1;
	int Door6:		1;
	int Door7:		1;
	int Reserve:	1;
} T_SWITCH1;

/* Bit structure - Switch 2  -------------------------------------------------*/
typedef struct {
	int Door8:		1;
	int	TokenBox:	1;
	int EnModule:	1;
	int ExModule:	1;
	int Spare1:		1;
	int Spare2:		1;
	int Reserve:	2;
} T_SWITCH2;

/* Bit structure - Passenger sensors for the swing faregate ------------------------------------*/
typedef union
{
	struct
	{
		BYTE bSen1;
		BYTE bSen2;
	} B;

	struct
	{
		int s01 : 1; // Port 1
		int s02 : 1; // Port 2
		int s03 : 1; // Port 3
		int s04 : 1; // Port 4
		int s05 : 1; // Port 5
		int s06 : 1; // Port 6
		int s07 : 1; // Port 7
		int s08 : 1; // Port 8
		int s09 : 1; // Port 9
		int s10 : 1; // Port 10
		int s11 : 1; // Port 11
		int s12 : 1; // Port 12
		int s13 : 1; // Port 13
		int s14 : 1; // Port 14
		int s15 : 1; // Port 15
		int s16 : 1; // Port 16
	} b;

	struct
	{
		int enter : 2;	 // 1,2
		int passage : 6; // 3,4,5,6,7,8
		int end : 2;	 // 9,10
		int upper : 2;	 // 11,12
		int lower : 4;	 // 13,14,15,16
	} dirEntry;

	struct
	{
		int end : 2;	 // 1,2
		int upper : 2;	 // 3,4
		int lower : 4;	 // 5,6,7,8
		int enter : 2;	 // 9,10
		int passage : 6; // 11,12,13,14,15,16
	} dirExit;

	struct
	{
		int enter_EN : 2; // 1,2
		int upper_EN : 2; // 3,4
		int lower_EN : 4; // 5,6,7,8
		int enter_EX : 2; // 9,10
		int upper_EX : 2; // 11,12
		int lower_EX : 4; // 13,14,15,16
	} section;

	struct
	{
		int entry : 8; // 1,2
		int exit : 8; // 3,4
	} side;
} T_PASS_SEN_SWING;

typedef union
{
	struct
	{
		BYTE bSen1;
		BYTE bSen2;
	} B;

	struct
	{
		int s01 : 1;
		int s02 : 1;
		int s03 : 1;
		int s04 : 1;
		int s05 : 1;
		int s06 : 1;
		int s07 : 1;
		int s08 : 1;
		int s09 : 1;
		int s10 : 1;
		int s11 : 1;
		int s12 : 1;
		int s13 : 1;
		int s14 : 1;
		int s15 : 1;
		int s16 : 1;
	} b;

	struct
	{
		int dummy_EN : 4;  // 1,2,3,4,
		int safety_EN : 4; // 5,6,7,8
		int dummy_EX : 4;  // 9,10,11,12
		int safety_EX : 4; // 13,14,15,16
	} swing;

} T_PASS_SEN_ERROR;

/* Bit structure - Spare sensors ---------------------------------------------*/
typedef struct {
	int End1:		1;
	int End2:		1;
	int Spare1:		1;
	int Spare2:		1;
	int dummy:		4;
} T_SPARE_SEN;

/* Bit structure - Sensors of flap door --------------------------------------*/
typedef struct {
	int Door1:		3;
	int dummy1:		1;
	int Door2:		3;
	int dummy2:		1;
} T_FDOOR_SEN;

/* Bit structure - ECU out signal --------------------------------------------*/
typedef struct {
	int AuthDirA:	1;
	int AuthDirB:	1;
	int rfu1:		1;
	int rfu2:		1;
	int dummy:		4;
} T_ECUOUT;

/* Operation mode ------------------------------------------------------------*/
enum {
	// Direction Service Mode
	NO_SERVICE		= 0,
	IN_SERVICE		= 1,
	FREE_SERVICE	= 2,

	// Emergency, Maintenance
	MODE_NONE			= 0x00,
	EMERGENCY_MODE		= 0x01,
	EMERGENCY_MODE_TTL	= 0x02,
	MASK_MODE_EMERGENCY = 0x03,
	MAINTENANCE_MODE	= 0x10,
	FREE_MODE			= 0x20,

	// Flap Door Mode
	FD_MODE_CLOSE	= 0,
	FD_MODE_OPEN	= 1
};

typedef struct {
	BYTE	bServiceMode_EN;	// NO_SERVICE(0), IN_SERVICE(1), FREE_SERVICE(2)
	BYTE	bServiceMode_EX;	// NO_SERVICE(0), IN_SERVICE(1), FREE_SERVICE(2)
	BYTE 	bEmergencyMaint;		// MODE_NONE(0x00), EMERGENCY_MODE(0x01), MAINTENANCE_MODE(0x10), FREE_MODE(0x20)
	BYTE	bFlapDoorMode;		// FD_MODE_CLOSE(0), FD_MODE_OPEN(1)
} T_GCU_OP_MODE;

/* Parameters ----------------------------------------------------------------*/
enum
{
	// Passage Type
	PASSAGE_TYPE_NONE = 0,
	PASSAGE_TYPE_W = 'W', // 18 sets sensor type for WMATA (Washington D.C)
	PASSAGE_TYPE_S = 'S', // 16 sets sensor type for swing gate (WMATA, BART)

	// Passage Control Mode
	PASS_MODE_MANUAL = 0, // default
	PASS_MODE_EASY = 1,

	// Alarm Mode
	ALARM_MODE_POLL = 0, // default
	ALARM_MODE_ACTIVE = 1,

	// Authorization mode
	AUTH_TYPE_SERIAL = 0, // default
	AUTH_TYPE_TTL = 1,

	// Timeouts
	DEFAULT_POWER_FAIL_TIMEOUT = 5, // sec
	DEFAULT_FDOOR_TIMEOUT_TICK = 3, // sec
	DEFAULT_AUTH_TIMEOUT = 20,		// sec
	DEFAULT_EMG_TIMEOUT = 45,		// sec
	MAX_AUTH_TIMEOUT = 60,			// sec
	MAX_EMG_TIMEOUT = 60,			// sec
	MAX_SAFETY_STAY_TIMEOUT = 45,	// sec
	NO_DELAY = 0,
	DEFAULT_ALARM_DELAY = 10, // sec
	NO_DURATION = 0,
	DEFAULT_ALARM_DURATION = 30, // sec
	TIMEOUT_ANTENNA = 250,		 // sec

	DEFAULT_SENSOR_BLOCK_TIMEOUT = 180, // sec
	DEFAULT_PASSAGE_CLEAR_TIMEOUT = 2,	// sec
	DEFAULT_BARRIER_OPEN_TIMEOUT = 3,	// sec
	DEFAULT_ALARM_TIMEOUT = 5,			// sec
	DEFAULT_JUMPING_TIMEOUT = 3,		// sec
	DEFAULT_TAILGATING_TIMEOUT = 2,		// sec

	// time unit conversion factor to tick count
	TICK_COUNT_10SEC = 1000, // 1 sec <- 100 * TICK(10ms)
	TICK_COUNT_1SEC = 100,	 // 1 sec <- 100 * TICK(10ms)
	TICK_COUNT_100MS = 10,	 // 0.1 sec <- 10 * TICK(10ms)

	// Alarm Zone
	ALARM_ZONE_NONE = 0, // default
	ALARM_ZONE1 = 1,
	ALARM_ZONE2 = 2,
	ALARM_ZONE3 = 3,

	// Alarm display type
	ALARM_OFF = 0x00, // default
	ALARM_RED_ON = 0x01,
	ALARM_BLINK = 0x10,
	ALARM_BUZZER_ON = 0x80,

	LUGGAGE_LIMIT_STD = 150,
	LUGGAGE_LIMIT_WDE = 150, // unit 10 ms

	SAFETY_TIMEOUT_STD = 10,
	SAFETY_TIMEOUT_WDE = 5,
};

typedef struct {
	BYTE bPassageType;			// PASSAGE_TYPE_G, PASSAGE_TYPE_B
	BYTE bPassageMode;			// PASS_MODE_MANUAL, PASS_MODE_EASY
	BYTE bAlarmMode;			// ALARM_MODE_POLL, ALARM_MODE_ACTIVE
	BYTE bAuthType;				// AUTH_TYPE_SERIAL, AUTH_TYPE_TTL
	BYTE bAuthTimeOut;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bCriticalZone;			// ALARM_ZONE_NONE, ALARM_ZONE1, ALARM_ZONE2, ALARM_ZONE3
	BYTE bCounterZone;			// ALARM_ZONE_NONE, ALARM_ZONE1, ALARM_ZONE2, ALARM_ZONE3
	BYTE bEMGTimeout;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bSensorBlockTimeout;	// 0(No Timeout(check)) ~ 255 (unit: sec)
	BYTE bBarrierOpenTimeout;	// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bAlarmTimeout;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bIllegalEntryTimeout;	// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bAutoEmergency;		// 0(Disable), 1 or else(Enable)
	BYTE bCheckChild;			// ON: Check child, OFF: Not Check Child
	BYTE bGateType;				// 0: Standard, 1: Wide	
} T_GCU_PARAMETER;

typedef struct {
	BYTE	bAuthCount_EN;
	BYTE	bAuthCount_EX;
} T_GCU_AUTH_COUNT;


/* Communication Baud Rate ---------------------------------------------------*/
enum {
	BAUD_9600		= 0,
	BAUD_19200		= 1,
	BAUD_38400		= 2,
	BAUD_57600		= 3,	// default baud
	BAUD_115200		= 4,

	BAUD_MAX_PARAM  = 4
};


/* Flag setting value */
enum {
	FLG_OFF = 0,
	FLG_SET = 1
};

/* Authorization -------------------------------------------------------------*/
enum {
	// Direction
	FROM_NONE	= 0,
	FROM_ENTRY	= 1,
	FROM_EXIT	= 2,

	AUTHORIZE_MAX_COUNT	= 255,
};

typedef struct {
	BYTE	bDirection;		// DIR_ENTRY, DIR_EXIT
	BYTE	bAuthCount;		// 1 ~ 255
	BYTE    bLamp; 			// LAMP_OFF, LAMP_RED_ON, LAMP_BLUE_ON, LAMP_GREEN_ON
	BYTE	bLampBar;		// DIR_OFF, DIR_BLUE, DIR_YELLOW
	BYTE	bDuration;		// 0 (Permanent) ~ 255 (unit: 0.1sec)
	BYTE	bTokenPath;		// TOKEN_PATH_NONE, TOKEN_PATH_BOX1, TOKEN_PATH_BOX2, TOKEN_PATH_RETURN
} T_CMD_AUTH_PASS;

enum
{
	PASS_ALARM_NONE = 0,
	PASS_ALARM_ILLEGAL_ENTER = 1,
	PASS_ALARM_COUNTER_ENTER = 2,
	PASS_ALARM_TAILGATING = 3,
	PASS_ALARM_CHILD_CRITICAL = 4,
	PASS_ALARM_SAFETY_FRAUD = 5,
	PASS_ALARM_JUMPING = 6,

	MASK_PASSAGE_ALARM = 0x77,
	MASK_PASSAGE_ALARM_TCU = 0x7F,

	FDOOR_ALARM_NONE = 0,
	FDOOR_ALARM_OPEN_FAULT = 1,
	FDOOR_ALARM_CLOSE_FAULT = 2,
	FDOOR_ALARM_SIGNAL_FAULT = 3,
};

typedef union {
	BYTE bMode;

	struct {
		int nServiceMode_EN		: 2;
		int nServiceMode_EX		: 2;
		int nEmergencyMode		: 2;
		int nMaintenanceMode	: 1;
		int nFlapDoorMode		: 1;
	} b;
} T_MODE_STATUS;

typedef union {
	BYTE bAlarm;

	struct {
		int nFromEN			: 3;
		int nPassOverFromEN	: 1;
		int nFromEX			: 3;
		int nPassOverFromEX	: 1;
	} b;
} T_PASSAGE_ALARM;

typedef union {
	BYTE bAlarm;

	struct {
		int nFDoor1		: 2;
		int nFDoor2		: 2;
		int nEMGSignal	: 1;
		int nSideDoor	: 1;
		int nPSensor	: 1;
		int nSafetyErr	: 1;
	} b;
} T_MODULE_ALARM;

typedef union {
	BYTE bIllegalPass;

	struct
	{
		int nTailgating_EN 	: 1;
		int nJumping_EN 	: 1;
		int nIllegal_EN 	: 1;
		int nCounter_EN 	: 1;
		int nTailgating_EX 	: 1;
		int nJumping_EX 	: 1;
		int nIllegal_EX		: 1;
		int nCounter_EX 	: 1;
	} b;
} T_ILLEGAL_PASS;

//#define ReadEndSensor()		(BYTE)(~RD_SPARESEN)
//#define ReadTCUSensor()		(BYTE)(~RD_TCUSEN)

typedef struct {
	BYTE	bModeStatus;
	BYTE	bAuthCount_EN;
	BYTE	bAuthCount_EX;
	BYTE	bPassageAlarm;
	BYTE	bModuleAlarm;
	BYTE	bBarrierSw;
	BYTE	bMaintenanceDoorSw;
	BYTE	bUPSStatus;
	BYTE	bSafetySen;
	BYTE	bSCADA;
	BYTE	bDoorForcedOpen;
	BYTE	bPassengerSensor1;
	BYTE	bPassengerSensor2;
	BYTE	bPassengerSensor3;
	BYTE	bIllegalPass;
	BYTE	bDipSwitchValue;
	BYTE	bDoorPowerState;
	BYTE 	bPeopleCount;
} T_GCU_STATUS_RES;

typedef struct {
	T_MODE_STATUS	ModeStatus;
	volatile BYTE	bAuthCount_EN;
	volatile BYTE	bAuthCount_EX;
	T_PASSAGE_ALARM PassageAlarm;
	T_MODULE_ALARM	ModuleAlarm;
	volatile BYTE	bBarrierSw;
	volatile BYTE	bMaintenanceDoorSw;
	volatile BYTE	bUPSStatus;
	volatile BYTE	bSafetyDetection;
	volatile BYTE	bSCADA;
	volatile BYTE	bDoorForcedOpen;
	volatile BYTE	bPassengerSensor1;
	volatile BYTE	bPassengerSensor2;
	volatile BYTE	bPassengerSensor3;
	T_ILLEGAL_PASS 	IllegalPass;
	volatile BYTE 	bPeopleCount;
} T_GCU_STATUS;

typedef struct {
	BYTE bPSensor1;
	BYTE bPSensor2;
	BYTE bPSensor3;
	BYTE bBarrierSw;
	BYTE bMaintenanceDoorSw;
	BYTE bModuleSw;
	BYTE bCaptureSensor;
	BYTE bEndSensor;
} T_RES_SENSOR_SW;

typedef struct {
	BYTE test_flag;
	BYTE bLamp_EN;
	BYTE bLamp_EX;
	BYTE bIndicator_EN;
	BYTE bIndicator_EX;	
	BYTE bOverhead_EN;
	BYTE bOverhead_EX;
} T_CMD_CONTROL_ALL_LED;

typedef struct {
	BYTE bIndicator_EN;
	BYTE bIndicator_EX;
} T_CMD_CONTROL_INDICATOR;

typedef struct {
	BYTE bLamp_EN;
	BYTE bLamp_EX;
} T_CMD_CONTROL_LAMP;

typedef struct {
	BYTE bSafety;
} T_CMD_SAFETY_STOP;

enum  {
	MASK_TEST_BUZZER_MAIN = 1,
	MASK_TEST_BUZZER_DIRA = 2,
	MASK_TEST_BUZZER_DIRB = 4
};

// Message packet length
enum
{
	FRAME_LEN_SWING = 19, 	// Protocol Frame Max Length for the swing barrier
	FRAME_LEN_RET = 128,   	// Protocol Frame Max Length for the retractible barrier
	FRAME_HEADER_LEN = 3,  	// Protocol Frame Header Length [Command, Module Seq, Data length]
	MAX_SEQUENCE = 255
};

enum
{ // protocol receiving state
	STATE_STX_WAIT,
	STATE_DATA_RCV,
	STATE_DATA_BCC,
	STATE_DATA_DLE,
	STATE_DATA_LEN,
	STATE_DATA_HEAD,
	STATE_DATA_BODY,
};

// protocol error response
enum
{
	ERR_BCC = 0x05,
	ACK_RECEIVED = 0x06,
	NAK_RECEIVED = 0x07,

	ERR_OLD_SEQUENCE = 0x08,
	ERR_OLD_SEQ_NO_RESPONSE = 0x09,
	ERR_WRONG_CMD_LEN = 0x0A,
	ERR_ILLEGAL_CMD = 0x0B,
	ERR_FRAME_LENGTH = 0x0C,
	ERR_DLE_PROCESS = 0x0D,
	ERR_NO_DATA = 0xFF
};

typedef struct {
	BYTE bBuzzerControl;
	BYTE bBuzzerDuration;
	BYTE bBuzzerNo;
} T_CMD_CONTROL_BUZZER;

typedef struct {
	BYTE	bDirection;		// DIR_ENTRY, DIR_EXIT
	BYTE	bLamp;			// LAMP_OFF, LAMP_RED_ON, LAMP_BLUE_ON, LAMP_GREEN_ON
	BYTE	bLampBar;		// DIR_OFF, DIR_BLUE, DIR_YELLOW
	BYTE	bDuration;		// 0 (Permanent) ~ 255 (unit: 0.1sec)
	BYTE	bTokenPath;		// TOKEN_PATH_NONE, TOKEN_PATH_BOX1, TOKEN_PATH_BOX2, TOKEN_PATH_RETURN
} T_CMD_DSP_ALARM;

#endif // __GCU_DEF_H
/*****END OF FILE****/
