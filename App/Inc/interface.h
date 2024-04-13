/*******************************************************************************
*       COPYRIGHT �� 2020 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : interface.h
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : GCU application for interface with host
*******************************************************************************/

#ifndef __INTERFACE_H_
#define __INTERFACE_H_

/* Compile options -----------------------------------------------------------*/
#define _CMD_DUMP_		0
#define _RES_DUMP_		0

/* Constant definitions ------------------------------------------------------*/
enum {	// command code			//WMATA GCU Interface Specification_20200403.doc 참고
	CMD_SET_MODE			= 0x30,
	CMD_GET_MODE			= 0x31,
	CMD_SET_PARAMETER		= 0x32,
	CMD_GET_PARAMETER		= 0x33,
	CMD_GET_VERSION			= 0x34,
	CMD_SET_SCADA_OUT		= 0x35,
	CMD_SW_RESET			= 0x38,
	CMD_AUTHORIZE			= 0x40,
	CMD_CLEAR_AUTH			= 0x41,
	CMD_GET_STATUS			= 0x42,
	CMD_GET_MEMORY_TEST	= 0x48,
	CMD_GET_SENSOR_ALARM	= 0x49,
	CMD_INFORM_ALARM		= 0x50,	
	CMD_CONTROL_ALL			= 0x60,
	CMD_CONTROL_DOOR		= 0x61,
	CMD_CONTROL_LAMP		= 0x62,
	CMD_CONTROL_INDICATOR	= 0x63,
	CMD_CONTROL_BUZZER		= 0x64,
	CMD_SAFETY_STOP			= 0x65,
	CMD_WRITE_REGISTER		= 0x90,
	CMD_READ_REGISTER		= 0x91,
	CMD_GET_CPLD_VERSION	= 0x92,
	CMD_GET_DIP_SWITCH		= 0x93,
	CMD_SET_UPS_COMMAND		= 0x94,
	CMD_GET_UPS_STATUS		= 0x95,	

	MAX_CMD_CODE_NEED_RESPONSE	= 0x5F
};

/* Protocol definitions ------------------------------------------------------*/
typedef struct {
	BYTE	bMainSeq;
	BYTE	bMouleSeq;
	BYTE	bDataLen;
	BYTE	bCmdCode;
	BYTE mbOptionData[FRAME_LEN_RET - FRAME_HEADER_LEN - 1];
} T_PROTOCOL_FRAME_D;

typedef union {
	BYTE Buff[FRAME_LEN_RET];
	T_PROTOCOL_FRAME_D	d;
} T_PROTOCOL_FRAME;

extern void ProcessComm_ECU(void);

#endif  // __INTERFACE_H_
/****END OF FILE****/
