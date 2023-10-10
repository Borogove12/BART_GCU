/*******************************************************************************
*          COPYRIGHT ¨Ï 2006 SAMSUNG SDS Co., Ltd. All rights reserved
********************************************************************************
* File Name    : fwdown.h
* Author       : Youn Weon Young
* Date Created : 2006/10/11
* Description  : firmware download definition
********************************************************************************
* [ Revision History ]
* VER.	DATE.		NOTES.											SIGN.
*		2006/10/11	1st Created										YWY
*******************************************************************************/

#ifndef __FWDOWN_H_
#define __FWDOWN_H_

// Control Characters
#define SOH		0x01	
#define STX		0x02
#define EOT		0x04
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18

// Packet data size
#define SOH_DATA_SIZE		128
#define SOH_PACKET_SIZE		133
#define STX_DATA_SIZE		1024
#define STX_PACKET_SIZE		1029
#define MAX_DATA_SIZE		STX_PACKET_SIZE

// Flash definitions
#define FLASH_APP_OFFSET	0x8000
#define FLASH_APP_LIMIT		0x40000
#define FLASH_APP_BASE		0x40008000

//
#define PACKET_TIMEOUT      50000
#define PACKET_RETRY		5
#define BYTE_TIMEOUT		0x00

/* Functions -----------------------------------------------------------------*/
void YmodemDownload(void);
//void FlashProgramTest(void);

#endif  // __FWDOWN_H_

/********** COPYRIGHT ¨Ï 2006 SAMSUNG SDS Co., Ltd.  ***********END OF FILE****/
