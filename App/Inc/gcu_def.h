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
#define GCU_FW_VERSION "1.0.12.4"
#define TG_TIMER 2		// 2 s
#define GCU_BAUD		57600

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

enum
{
	STANDARD = 0,
	WIDE = 1
};

enum
{
	PASS_PATN_NONE = 0,
	PASS_PATN_ENTER = 0x02,		 // EN: #2 / EX: #10
	PASS_PATN_PASS_READY = 0x12, // EN: #4,7 / EX: #12,15
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

////////////////////////////////////////////////////////////////////////////////////
/* GCU IO Address Access definition ------------------------------------------*/
#define RD_SWITCH		(*(VBYTE*)(READ02_ADR))						// global.h
#define RD_PASSSEN1		(*(VBYTE*)(READ00_ADR))						// global.h
#define RD_PASSSEN2		(*(VBYTE*)(READ01_ADR))						// global.h

#define WR_FDOOR		(*(VBYTE*)(ADDR_WR_FDOOR))
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
	UPS_CMD_CHECK_ON 	= 0x01,
	UPS_CMD_SHUTDOWN 	= 0x02,
};

// TODO: Must be changed for Prod board
// Oak Board: Low active
// #define ControlUPS_CheckOn() 		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, GPIO_PIN_RESET)
// #define ControlUPS_CheckOff() 		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, GPIO_PIN_SET)
// Prod Board: High active
#define ControlUPS_CheckOn() 		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, GPIO_PIN_SET)
#define ControlUPS_CheckOff() 		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, GPIO_PIN_RESET)

// TODO: Must be changed for Prod board
// Oak Board: High active
// #define ControlUPS_ShutDownOn() 	HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, GPIO_PIN_SET)
// #define ControlUPS_ShutDownOff() 	HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, GPIO_PIN_RESET)
// Prod Board: Low active
#define ControlUPS_ShutDownOn() 	HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, GPIO_PIN_RESET)
#define ControlUPS_ShutDownOff() 	HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, GPIO_PIN_SET)

enum {	
	BARRIER_FREE	= 0x00,
	BARRIER_OPEN_FOR_EX	= 0x02,
	BARRIER_OPEN_FOR_EN	= 0x01,
	BARRIER_OPEN	= 0x66,
	BARRIER_CLOSE 	= 0x03,

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
	LAMP_GREEN_ON = 0x02,
	LAMP_BLUE_ON = 0x04,
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
	MASK_SWING_MODE     = 0xC0, // 1100 0000
	MASK_SELF_TEST      = 0x76, // 0111 0110	//modify pms
	MASK_JIG_TEST	    = 0x01
};

#define LED_BUZZ_ON		1
#define LED_BUZZ_OFF	0

#define BUZZ_ON		0
#define BUZZ_OFF	1

enum {
	BUZZER_NO_MAIN	= 0,

	BUZZER_OFF		= 0,
	BUZZER_ON		= 1,
};

#define IsEMGSignalOn()		HAL_GPIO_ReadPin(EMG_GPIO_Port,EMG_Pin) // 1- On, 0 - Off

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
		UINT32 s01 : 1; // Port 1
		UINT32 s02 : 1; // Port 2
		UINT32 s03 : 1; // Port 3
		UINT32 s04 : 1; // Port 4
		UINT32 s05 : 1; // Port 5
		UINT32 s06 : 1; // Port 6
		UINT32 s07 : 1; // Port 7
		UINT32 s08 : 1; // Port 8
		UINT32 s09 : 1; // Port 9
		UINT32 s10 : 1; // Port 10
		UINT32 s11 : 1; // Port 11
		UINT32 s12 : 1; // Port 12
		UINT32 s13 : 1; // Port 13
		UINT32 s14 : 1; // Port 14
		UINT32 s15 : 1; // Port 15
		UINT32 s16 : 1; // Port 16
	} b;

	struct
	{
		UINT32 enter : 2;	 // 1,2
		UINT32 passage : 6; // 3,4,5,6,7,8
		UINT32 end : 2;	 // 9,10
		UINT32 upper : 2;	 // 11,12
		UINT32 lower : 4;	 // 13,14,15,16
	} dirEntry;

	struct
	{
		UINT32 end : 2;	 // 1,2
		UINT32 upper : 2;	 // 3,4
		UINT32 lower : 4;	 // 5,6,7,8
		UINT32 enter : 2;	 // 9,10
		UINT32 passage : 6; // 11,12,13,14,15,16
	} dirExit;

	struct
	{
		UINT32 enter_EN : 2; // 1,2
		UINT32 upper_EN : 2; // 3,4
		UINT32 lower_EN : 4; // 5,6,7,8
		UINT32 enter_EX : 2; // 9,10
		UINT32 upper_EX : 2; // 11,12
		UINT32 lower_EX : 4; // 13,14,15,16
	} section;

	struct
	{
		UINT32 entry : 8; // 1-8
		UINT32 exit : 8; // 9-16
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
		UINT32 s01 : 1;
		UINT32 s02 : 1;
		UINT32 s03 : 1;
		UINT32 s04 : 1;
		UINT32 s05 : 1;
		UINT32 s06 : 1;
		UINT32 s07 : 1;
		UINT32 s08 : 1;
		UINT32 s09 : 1;
		UINT32 s10 : 1;
		UINT32 s11 : 1;
		UINT32 s12 : 1;
		UINT32 s13 : 1;
		UINT32 s14 : 1;
		UINT32 s15 : 1;
		UINT32 s16 : 1;
	} b;

	struct
	{
		UINT32 dummy_EN : 4;  // 1,2,3,4,
		UINT32 safety_EN : 4; // 5,6,7,8
		UINT32 dummy_EX : 4;  // 9,10,11,12
		UINT32 safety_EX : 4; // 13,14,15,16
	} swing;

} T_PASS_SEN_ERROR;

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
	BYTE 	bEmergencyMaint;	// MODE_NONE(0x00), EMERGENCY_MODE(0x01), MAINTENANCE_MODE(0x10), FREE_MODE(0x20)
	BYTE	bFlapDoorMode;		// FD_MODE_CLOSE(0), FD_MODE_OPEN(1)
} T_GCU_OP_MODE;

/* Parameters ----------------------------------------------------------------*/
enum
{
	// Passage Type
	PASSAGE_TYPE_NONE = 0,
	PASSAGE_TYPE_W = 'W', // 18 sets sensor type for WMATA (Washington D.C)
	PASSAGE_TYPE_S = 'S', // 16 sets sensor type for BART (Bay Area, SF/OAK )
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
	DEFAULT_ILLEGAL_ENTRY_TIMEOUT = 10,	// sec

	// time unit conversion factor to tick count
	TICK_COUNT_10SEC = 10000, // 1 sec <- 100 * TICK(10ms)
	TICK_COUNT_1SEC = 1000,	 // 1 sec <- 100 * TICK(10ms)
	TICK_COUNT_100MS = 100,	 // 0.1 sec <- 10 * TICK(10ms)

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
	BYTE bAuthTimeOut;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bEMGTimeout;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bSensorBlockTimeout;	// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bAlarmTimeout;			// 0(No Timeout) ~ 255 (unit: sec)
	BYTE bIllegalEntryTimeout;	// 0(No Timeout) ~ 255 (unit: sec)
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
		UINT32 nServiceMode_EN		: 2;
		UINT32 nServiceMode_EX		: 2;
		UINT32 nEmergencyMode		: 2;
		UINT32 nMaintenanceMode		: 1;
		UINT32 nFlapDoorMode		: 1;
	} b;
} T_MODE_STATUS;

typedef union {
	BYTE bAlarm;

	struct {
		UINT32 nFromEN			: 3;
		UINT32 nPassOverFromEN	: 1;
		UINT32 nFromEX			: 3;
		UINT32 nPassOverFromEX	: 1;
	} b;
} T_PASSAGE_ALARM;

typedef union {
	BYTE bAlarm;

	struct {
		UINT32 nFDoor1		: 2;
		UINT32 nFDoor2		: 2;
		UINT32 nEMGSignal	: 1;
		UINT32 nSideDoor	: 1;
		UINT32 nPSensor		: 1;
		UINT32 nSafetyErr	: 1;
	} b;
} T_MODULE_ALARM;

typedef union {
	BYTE bIllegalPass;

	struct
	{
		UINT32 nTailgating_EN 	: 1;
		UINT32 nJumping_EN 	: 1;
		UINT32 nIllegal_EN 	: 1;
		UINT32 nCounter_EN 	: 1;
		UINT32 nTailgating_EX 	: 1;
		UINT32 nJumping_EX 	: 1;
		UINT32 nIllegal_EX		: 1;
		UINT32 nCounter_EX 	: 1;
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

typedef struct {
	BYTE bSCADA_Out;
} T_CMD_SCADA_OUT;

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
