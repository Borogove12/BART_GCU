/*******************************************************************************
 *       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : barrier.h
 * Author       : Joseph Yosep Choi
 * Date Created : 09/14/2022
 * Description  : Swing barrier(SHHM) interface
 *******************************************************************************/

#ifndef __BARRIER_H_
#define __BARRIER_H_

#include "gcu_def.h"
#include "bsp_types.h"
/* Compile options -----------------------------------------------------------*/
#define _CMD_DUMP_ 0
#define _RES_DUMP_ 0

/* Constant definitions ------------------------------------------------------*/
// Barrier command code
enum					// inform 없음
{
    CMD_ACK = 0x01,
    CMD_NAK = 0x02, // Unable to execute
    CMD_MSG_ERR = 0x03,
    CMD_MSG_DUP = 0x04,
    CMD_SET_PARAM_MAIN = 0x70,
    CMD_SET_PARAM_SUB = 0x71,
    CMD_RESET_MAIN = 0x72,
    CMD_RESET_SUB = 0x73,
    CMD_SET_EMEREGENCY = 0x78,
    CMD_RESET_EMEREGENCY = 0x79,
    CMD_SET_SERIAL_COMM = 0x7A,
    CMD_RESET_SERIAL_COMM = 0x7B,
    CMD_OPEN_BARRIER = 0x7C,
    CMD_CLOSE_BARRIER = 0x7D,
    CMD_SET_STOP_BARRIER = 0x7E,
    CMD_RESET_STOP_BARRIER = 0x7F,

    CMD_GET_PARAM_MAIN = 0x90,
    CMD_GET_PARAM_SUB = 0x91,
    CMD_GET_STATUS_MAIN = 0x92,				//어떤 정보가 돌아오는가
    CMD_GET_STATUS_SUB = 0x93,
    CMD_GET_SPEED_MAIN = 0x94,
    CMD_GET_SPEED_SUB = 0x95,
    CMD_GET_VERSION_MAIN = 0x9C,
    CMD_GET_VERSION_SUB = 0x9D,
    CMD_GET_IAP_VERSION_MAIN = 0x9E,
    CMD_GET_IAP_VERSION_SUB = 0x9F,
};

/* Protocol definitions ------------------------------------------------------*/
typedef struct
{    
    BYTE command;
    BYTE moduleSeq;
    BYTE dataLen;
    BYTE data[15];
    // BYTE dataLen;
    // BYTE command;
    // BYTE data[16];
} B_PROTOCOL_FRAME_D;

typedef union
{
    BYTE Buff[19];
    B_PROTOCOL_FRAME_D d;		//18 Byte
} B_PROTOCOL_FRAME;				// 19Byte

/* Parameter definitions ------------------------------------------------------*/
typedef union
{
    BYTE Buff[6];

    struct
    {
        BYTE param1;
        BYTE param2;
        BYTE param3;
        BYTE param4;
        BYTE param5;
        BYTE param6;
    } B;

    struct
    {
        int dStrength       : 4; // 0-10
        int sStrength       : 4;   
        int speed           : 4; // 0-15
        int config          : 1; // 0: not set, 1: set (If not set, barrier will set automatically after initializing)
        int barrierType     : 1; // 0: Right, 1: Left  (From free side)
        int emgSignal       : 1; // 0: Low, 1: High
        int aisleType       : 1; // 0: Standard, 1: ADA
        int closeDelay1     : 8; // 0-100 (Closing shutdown delay)
        int closeDelay2     : 8; // 0-100 (Closing braking delay)
        int openDelay1      : 8; // 0-100 (Opening shutdown delay)
        int openDelay2      : 8; // 0-100 (Opening braking delay)
    } param;

} B_BARRIER_PARAMETER;

/* Barrier status definitions ------------------------------------------------------*/
typedef union
{
    BYTE Buff[6];

    struct
    {
        BYTE status0;
        BYTE status1; // Reserved
        BYTE status2; // Reserved
        BYTE status3;
        BYTE status4;
        BYTE status5;
    } B;

    struct
    {
        int senserStatus : 8; // Not used
        int reserved1 : 8;    // Not used
        int reserved2 : 8;    // Not used
        int cmdMode : 1;      // 0: Close cmd, 1: Open cmd
        int brakeMode : 1;    // 0: Normal, 1: Brake
        int direction : 1;    // 0: Outbound, 1: Inbound
        int commSource : 1;   // 0: IO, 1: Serial
        int reserved3 : 2;    // Not used
        int emergency : 2;    // 0: Normal, 1: SW emg, 2: HW emg, 3: Both
        int posStatus : 3;    // 2: Close pos, 5: Open pos
        int actStatus : 1;    // 0: Stop, 1: Move
        int faultStatus : 1;  // 0: Normal, 1: Fault
        int errStatus : 1;    // 0: Normal, 1: Error
        int brakeStatus : 1;  // 0: Normal, 1: Brake
        int emgStatus : 1;    // 0: Normal, 1: EMG
        int errorCode : 8;
    } param;

} B_BARRIER_STATUS;

/* Functions prototypes ------------------------------------------------------*/
void MakeRequest(BYTE command, BYTE *data, int nOptionLen);
void Brr_SetEmergency(bool isActive);
void Brr_OpenBarrier(BYTE direction);
void Brr_CloseBarrier(void);
void Brr_StopBarrier(bool isStop);
void Brr_Inq_Status(void);
void Brr_SetSerialComm(bool isEnabled); 
void SendRequest(void);
void ProcessComm_Barrier(void);
int CheckProtocol_UART4(void);
int CheckProtocol_UART5(void);


#define ReadAddr600		4

#endif // __INTERFACE_H_
    /****END OF FILE****/
