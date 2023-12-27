/*******************************************************************************
 *       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : barrier.c
 * Author       : Joseph Yosep Choi
 * Date Created : 09/14/2022
 * Description  : Swing barrier(SHHM) interface
 *******************************************************************************/

#include <string.h>
#include "userlib.h"
#include "serial.h"
#include "barrier.h"
#include "repository.h"
#include "test.h"
#include "bsp.h"
#include "common_func.h"

B_PROTOCOL_FRAME rsp_U4; // Primary barrier rsp_U1 -> rsp_U4
B_PROTOCOL_FRAME rsp_U5; // Secondary barrier rsp_U3 -> rsp_U5

B_PROTOCOL_FRAME req;

BYTE gbLastMainSeq_Swing;
BYTE gbLastHostCMD_Swing;
BOOL gIsOpenCmdIssued;
BOOL gIsCloseCmdIssued;

extern IUART_T iUartTable[IUART_MAXPORT];
extern uint8_t Add_200_Value;

/*******************************************************************************
 * Name:     Brr_SetParam
 * Function: 70H/71H command
 * Length:   6 bytes
 * In:       bool isMain: indicator for main/sub
 *           B_BARRIER_PARAMETER param: parameter for barrier setting
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_SetParam(bool isMain, B_BARRIER_PARAMETER param) // not used pms
{
#if BART_IO

#else

    BYTE cmd = isMain ? CMD_SET_PARAM_MAIN : CMD_SET_PARAM_SUB;
    BYTE *data = NULL;
    memcpy(data, &param.Buff, 6);
    MakeRequest(cmd, data, 6);
#endif
}

/*******************************************************************************
 * Name:     Brr_ResetBarrier
 * Function: 72H/73H command
 * Length:   0 bytes
 * In:       bool isMain: indicator for main/sub
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_ResetBarrier(bool isMain) // not used pms
{
#if BART_IO

#else
    BYTE cmd = isMain ? CMD_RESET_MAIN : CMD_RESET_SUB;
    MakeRequest(cmd, NULL, 0);
#endif
}

/*******************************************************************************
 * Name:     Brr_SetEmergency
 * Function: 78H/79H command
 * Length:   0 bytes
 * In:       bool isActive: indicator for the active/deactive
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_SetEmergency(bool isActive)
{
    static clock_t ms;
    BYTE cmd = isActive ? CMD_SET_EMEREGENCY : CMD_RESET_EMEREGENCY;    
    gfisBarrierEmg = isActive;
    Brr_SetEmergency_P(isActive);
    Brr_SetEmergency_S(isActive);
    printf(" [%02X] command\n",cmd);

    // 100ms 단위로 printf 발생   //너무 많은 접근으로 버퍼가 터지는 문제가 있음 해당 구문이 문제 없다면 다른 printf에도 적용 예정
    if (!mstimeout(&ms, 100))
        return;
}

/*******************************************************************************
 * Name:     Brr_SetSerialComm
 * Function: 7AH/7BH command
 * Length:   0 bytes
 * In:       bool isEnabled: indicator for the enabled/disabled
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_SetSerialComm(bool isEnabled) //	not used pms
{
    BYTE cmd = isEnabled ? CMD_SET_SERIAL_COMM : CMD_RESET_SERIAL_COMM;
    MakeRequest(cmd, NULL, 0);
    printf(" [%02X] command\n", cmd);
}

/*******************************************************************************
 * Name:     Brr_OpenBarrier
 * Function: 7CH command
 * Length:   1 bytes
 * In:       BYTE direction: 1-Entry / 2-Exit
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_OpenBarrier(BYTE direction)
{
    switch (direction)
    {
    case BARRIER_OPEN_FOR_EX:
        Brr_ControlBarrier_P(ENTRY_OPEN);
        Brr_ControlBarrier_S(ENTRY_OPEN);
        break;

    case BARRIER_OPEN_FOR_EN:
        Brr_ControlBarrier_P(EXIT_OPEN);
        Brr_ControlBarrier_S(EXIT_OPEN);
        break;
    }

    if (gIsOpenCmdIssued == FALSE)
    {
        printf(" [%02X] command\n", CMD_OPEN_BARRIER);
        gIsOpenCmdIssued = TRUE;
        gIsCloseCmdIssued = FALSE;
    }
}

/*******************************************************************************
 * Name:     Brr_CloseBarrier
 * Function: 7DH command
 * Length:   1 bytes
 * In:       None
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_CloseBarrier(void)
{
    Brr_ControlBarrier_P(CLOSE);
    Brr_ControlBarrier_S(CLOSE);
    if (gIsCloseCmdIssued == FALSE)
    {
        printf(" [%02X] command\n", CMD_CLOSE_BARRIER);
        gIsOpenCmdIssued = FALSE;
        gIsCloseCmdIssued = TRUE;
    }
}

/*******************************************************************************
 * Name:     Brr_StopBarrier
 * Function: 7EH/7FH command
 * Length:   0 bytes
 * In:       bool isStop: indicator for the stop/release
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_StopBarrier(bool isStop)
{
    BYTE cmd = isStop ? CMD_SET_STOP_BARRIER : CMD_RESET_STOP_BARRIER;
    Brr_SetBreak_P(isStop);
    Brr_SetBreak_S(isStop);
    printf(" [%02X] command\n", cmd);
}

/*******************************************************************************
 * Name:     Brr_Inq_Status
 * Function: 92H
 * Length:   0 bytes
 * In:       None
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_Inq_Status(void)
{
    MakeRequest(CMD_GET_STATUS_MAIN, NULL, 0);
}

/*******************************************************************************
 * Name:     Brr_Res_Status
 * Function: 92H
 * Length:   6 bytes
 * In:       bool isMain
 * Out:      None
 * Return:   void
 *******************************************************************************/
void Brr_Res_Status(bool isMain) // isMain : TRUE - UART1 / FALSE - UART3
{
    if (isMain) // UART4
    {
        memcpy(&gMainBarrierStatus, (B_BARRIER_STATUS *)rsp_U4.d.data, sizeof(B_BARRIER_STATUS));
        // printf(" Receive Main Status %d/%d/%d  \n", gMainBarrierStatus.B.status0, gMainBarrierStatus.B.status3, gMainBarrierStatus.B.status4);
    }
    else // UART5
    {
        memcpy(&gSubBarrierStatus, (B_BARRIER_STATUS *)rsp_U5.d.data, sizeof(B_BARRIER_STATUS));
        // printf(" Receive Sub Status %d/%d/%d  \n", gSubBarrierStatus.B.status0, gSubBarrierStatus.B.status3, gSubBarrierStatus.B.status4);
    }
}

/*******************************************************************************
 * Name:     Brr_Status_IO
 * Function:
 * Length:
 * In:       void
 * Out:      None
 * Return:   void
 *******************************************************************************/
#if 0
void Brr_Status_IO(void)				//pms
{

	gMainBarrierStatus.param.direction = 	(Add_200_Value & 0x04)? 1:0;			//No information from the Barrier controller
	gMainBarrierStatus.param.emergency = 	(Add_200_Value & 0x02)? 1:0;			//No information from the Barrier controller
	gMainBarrierStatus.param.cmdMode   = 	(Add_200_Value & 0x01)? 1:0;			//No information from the Barrier controller
	gMainBarrierStatus.param.brakeMode = 	(Add_200_Value & 0x08)? 1:0;			//No information from the Barrier controller

	gMainBarrierStatus.param.actStatus = 	(extinb(ReadAddr600) & 0x01)? 1:0;		//Low Active
	gMainBarrierStatus.param.faultStatus = 	(extinb(ReadAddr600) & 0x02)? 1:0;		//Action not confirmed
	gMainBarrierStatus.param.errStatus = 	(extinb(ReadAddr600) & 0x04)? 1:0;		//anomaly signal //Low Active
	gMainBarrierStatus.param.brakeStatus = 	(extinb(ReadAddr600) & 0x08)? 1:0;		//High Active


	gSubBarrierStatus.param.direction = 	(Add_200_Value & 0x40)? 1:0;			//No information from the Barrier controller
	gSubBarrierStatus.param.emergency = 	(Add_200_Value & 0x20)? 1:0;			//No information from the Barrier controller
	gSubBarrierStatus.param.cmdMode   = 	(Add_200_Value & 0x10)? 1:0;			//No information from the Barrier controller
	gSubBarrierStatus.param.brakeMode = 	(Add_200_Value & 0x80)? 1:0;			//No information from the Barrier controller

	gSubBarrierStatus.param.actStatus = 	(extinb(ReadAddr600) & 0x10)? 0:1;		//Low Active
	gSubBarrierStatus.param.faultStatus = 	(extinb(ReadAddr600) & 0x20)? 1:0;		//Action not confirmed
	gSubBarrierStatus.param.errStatus = 	(extinb(ReadAddr600) & 0x40)? 1:0;		//anomaly signal - High Active
	gSubBarrierStatus.param.brakeStatus = 	(extinb(ReadAddr600) & 0x80)? 1:0;		//High Active

}
#endif

void WaitTX(void)
{
    U5_TxWait();
    U4_TxWait();
}

void SendSTX(void)
{
    WaitTX();
    U5_Putch(0xAA);
    U4_Putch(0xAA);
}

void SendBCC(BYTE bBCC)
{
    WaitTX();
    U5_Putch(bBCC);
    U4_Putch(bBCC);
}

BYTE SendData(void)
{
    BYTE bBCC;
    BYTE *pbReqBuff;
    int nReqLen;

    bBCC = 0;
    pbReqBuff = req.Buff;
    nReqLen = req.d.dataLen + FRAME_HEADER_LEN;

    while (nReqLen--)
    {
        WaitTX();
        U5_Putch(*pbReqBuff);
        U4_Putch(*pbReqBuff);

        bBCC ^= *pbReqBuff; // BCC calculation
        pbReqBuff++;
    }

    return bBCC;
}

/*******************************************************************************
 * Name:     MakeRequest
 * Function:
 * In:       pbResOption
 *			 nOptionLen
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void MakeRequest(BYTE command, BYTE *data, int nOptionLen)
{
    BYTE bBCC;

    if (req.d.moduleSeq == MAX_SEQUENCE)
        req.d.moduleSeq = 0;
    else
        req.d.moduleSeq = gbLastMainSeq_Swing++;

    req.d.command = command;
    req.d.dataLen = nOptionLen;

    if (nOptionLen)
        memcpy(req.d.data, data, nOptionLen);

    SendSTX();
    bBCC = SendData();
    SendBCC(bBCC);
}

/*******************************************************************************
 * Name:     SendRequest
 * Function:
 * In:       None
 * Out:      None
 * Return:   void
 * comments:
 *******************************************************************************/
void SendRequest(void)
{
    BYTE bBCC;
    BYTE *pbReqBuff;
    int nReqLen;

    // Secondary barrier first
    bBCC = 0;
    pbReqBuff = req.Buff;
    nReqLen = req.d.dataLen + FRAME_HEADER_LEN;
    // send STX
    U5_Putch(0xAA);

    // send data
    while (nReqLen--)
    {
        U5_Putch(*pbReqBuff);

        bBCC ^= *pbReqBuff; // BCC calculation
        pbReqBuff++;
    }

    // send BCC
    U5_Putch(bBCC);

    // Primary barrier last
    bBCC = 0;
    pbReqBuff = req.Buff;
    nReqLen = req.d.dataLen + FRAME_HEADER_LEN;
    // send STX
    U4_Putch(0xAA);

    // send data
    while (nReqLen--)
    {
        U4_Putch(*pbReqBuff);

        bBCC ^= *pbReqBuff; // BCC calculation
        pbReqBuff++;
    }

    // send BCC
    U4_Putch(bBCC);
}

/*******************************************************************************
 * Name:     CheckProtocol_UART1
 * Function: It process the interface with barrier via UART1
 * In:       None
 * Out:      None
 * Return:   int - SUCCESS (0) : one command is received
 *			    - else        : no command is received
 * comments: It checks serial data from Host and protocol format of data received.
 *           If the protocol format is correct, it returns SUCCESS result.
 *******************************************************************************/

int CheckProtocol_UART4(void)
{
    static BYTE rcvState_U4 = STATE_STX_WAIT;
    static BYTE BCC_U4 = 0;
    static int len_U4 = 0;
    static int packetLen_U4 = 0;
    BYTE rcvData;

    //  while (U1_Getch(&rcvData))			//스타트는 [AA] 다음 [len]이 들어오고 다음[Data]
    while (IUart_RecvByte_check(COM4, &rcvData))
    {
        if (rcvData == 0xAA)
        {
            packetLen_U4 = 0;
            len_U4 = 0;
            BCC_U4 = 0;
            rcvState_U4 = STATE_DATA_HEAD;
        }
        else
        {
            switch (rcvState_U4)
            {
            case STATE_DATA_HEAD:
                rsp_U4.Buff[packetLen_U4++] = rcvData;
                BCC_U4 ^= rcvData;
                if (packetLen_U4 == 3)
                {
                    len_U4 = rcvData;
                    if (len_U4 == 0)
                    {
                        rcvState_U4 = STATE_DATA_BCC;
                    }
                    else
                    {
                        rcvState_U4 = STATE_DATA_BODY;
                    }
                }
                break;
            case STATE_DATA_BODY:
                rsp_U4.Buff[packetLen_U4++] = rcvData;
                BCC_U4 ^= rcvData;
                len_U4--;

                if (len_U4 == 0)
                {
                    rcvState_U4 = STATE_DATA_BCC;
                }
                break;
            case STATE_DATA_BCC:
                rcvState_U4 = STATE_STX_WAIT;
                if (BCC_U4 == rcvData)
                {
                    if (rsp_U4.d.dataLen != (BYTE)(packetLen_U4 - FRAME_HEADER_LEN))
                    {
                        printf("U4 LENGTH ERR: %d - %d  \n", rsp_U4.d.dataLen, (packetLen_U4 - FRAME_HEADER_LEN));
                        return ERR_WRONG_CMD_LEN;
                    }
                    else
                    {
                        return SUCCESS;
                    }
                }
                else
                {   if (rsp_U4.d.command != 6)
                    {
                        printf("U4 BCC ERR. len: %02X, BCC: %02X, cmd: %02X  \n", rsp_U4.d.dataLen, BCC_U4, rsp_U4.d.command);
                        Print2Hex("", rsp_U4.Buff, packetLen_U4);
                    }

                    return ERR_BCC;                    
                }
            }
        }
    }

    return FAIL;
}

/*******************************************************************************
 * Name:     CheckProtocol_UART3
 * Function: It process the interface with barrier via UART3
 * In:       None
 * Out:      None
 * Return:   int - SUCCESS (0) : one command is received
 *			    - else        : no command is received
 * comments: It checks serial data from Host and protocol format of data received.
 *           If the protocol format is correct, it returns SUCCESS result.
 *******************************************************************************/
int CheckProtocol_UART5(void)
{
    static BYTE rcvState_U5 = STATE_STX_WAIT;
    static BYTE BCC_U5 = 0;
    static int len_U5 = 0;
    static int packetLen_U5 = 0;
    int temp_Len = 0;
    BYTE rcvData;

    while (IUart_RecvByte_check(COM5, &rcvData))
    {
        if (rcvData == 0xAA)
        {
            packetLen_U5 = 0;
            len_U5 = 0;
            BCC_U5 = 0;
            rcvState_U5 = STATE_DATA_HEAD;
        }
        else
        {
            switch (rcvState_U5)
            {
            case STATE_DATA_HEAD:
                rsp_U5.Buff[packetLen_U5++] = rcvData;
                BCC_U5 ^= rcvData;
                if (packetLen_U5 == 3)
                {
                    len_U5 = rcvData;
                    if (len_U5 == 0)
                    {
                        rcvState_U5 = STATE_DATA_BCC;
                    }
                    else
                    {
                        rcvState_U5 = STATE_DATA_BODY;
                    }
                }
                break;
            case STATE_DATA_BODY:
                rsp_U5.Buff[packetLen_U5++] = rcvData;
                BCC_U5 ^= rcvData;
                len_U5--;

                if (len_U5 == 0)
                {
                    rcvState_U5 = STATE_DATA_BCC;
                }
                break;
            case STATE_DATA_BCC:
                rcvState_U5 = STATE_STX_WAIT;
                if (BCC_U5 == rcvData)
                {
                    if (rsp_U5.d.dataLen != (BYTE)(packetLen_U5 - FRAME_HEADER_LEN))
                    {
                        printf("U5 LENGTH ERR: %d - %d  \n", rsp_U5.d.dataLen, (packetLen_U5 - FRAME_HEADER_LEN));
                        return ERR_WRONG_CMD_LEN;
                    }
                    else
                    {
                        return SUCCESS;
                    }
                }
                else
                {
                    if (rsp_U5.d.command != 6)
                    {
                        printf("U5 BCC ERR. len: %02X, BCC: %02X, cmd: %02X  \n", rsp_U5.d.dataLen, BCC_U5, rsp_U5.d.command);
                        Print2Hex("", rsp_U5.Buff, packetLen_U5);
                    }

                    return ERR_BCC;
                }
            }
        }
    }

    return FAIL;
}

/*******************************************************************************
 * Name:     ProcessCommand_Barrier
 * Function: process command from ACMCHost
 * In:       None
 * Out:      None
 * Return:   - SUCCESS (0) : new command is processed successfully
 *			- ERR_OLD_SEQUENCE : same command is received again
 *			- ERR_ILLEGAL_CMD  : command code is not defined
 * comments: It checks serial data from Host and protocol format of data received.
 *           If the protocol format is correct, it returns SUCCESS result.
 *******************************************************************************/
int ProcessCommand_Barrier(bool isMain)
{
    BYTE cmd;
    if (isMain == TRUE)
    {
        cmd = rsp_U4.d.command;
    }
    else
    {
        cmd = rsp_U5.d.command;
    }
    switch (cmd) // command
    {
    case CMD_GET_PARAM_MAIN:
        // CMDSetGCUOpMode();
        break;
    case CMD_GET_PARAM_SUB:
        // CMDGetGCUOpMode();
        break;
    case CMD_GET_STATUS_MAIN:
        Brr_Res_Status(isMain);
        break;
    case CMD_GET_STATUS_SUB:
        // CMDGetGCUOpMode();
        break;
    case CMD_GET_SPEED_MAIN:
        // CMDSetGCUOpMode();
        break;
    case CMD_GET_SPEED_SUB:
        // CMDGetGCUOpMode();
        break;
    case CMD_GET_VERSION_MAIN:
        // CMDSetGCUOpMode();
        break;
    case CMD_GET_VERSION_SUB:
        // CMDGetGCUOpMode();
        break;
    case CMD_GET_IAP_VERSION_MAIN:
        // CMDSetGCUOpMode();
        break;
    case CMD_GET_IAP_VERSION_SUB:
        // CMDGetGCUOpMode();
        break;
    case CMD_ACK:
        return SUCCESS;
    case CMD_NAK:
        return SUCCESS;
    case CMD_MSG_ERR:
        return SUCCESS;
    case CMD_MSG_DUP:
        return SUCCESS;
    default:
        return SUCCESS;
    }

    return SUCCESS;
}

/*******************************************************************************
 * Name:     ProcessComm_Barrier
 * Function: Communication process for Barrier
 * In:       None
 * Out:      None
 * Return:   void
 * comments: it checks serial data from Host and protocol format of data received.
 *           If the protocol format is correct, it makes action refer to the command
 *           from Host.
 *******************************************************************************/
void ProcessComm_Barrier(void)
{
    // Changed an order. Slave barrier first.
    int nError = CheckProtocol_UART5();
    if (nError == SUCCESS)
    {
        if (ProcessCommand_Barrier(FALSE) != SUCCESS)
        {
            printf("UART5 ERR  \n");
        }
    }

    nError = CheckProtocol_UART4();
    if (nError == SUCCESS)
    {
        if (ProcessCommand_Barrier(TRUE) != SUCCESS)
        {
            // TODO: Do we need the retry logic?
            printf("UART4 ERR  \n");
        }
    }
}

/*
void U4_TxWait(void)
{
    while(!LL_USART_IsActiveFlag_TXE(UART4))
        ;
}


void U5_TxWait(void)
{
    while(!LL_USART_IsActiveFlag_TXE(UART5))
        ;
}

void U4_Putch(BYTE c)
{
     while(!LL_USART_IsActiveFlag_TXE(UART4))
         ;
      LL_USART_TransmitData8(UART4, c);
}

void U5_Putch(BYTE c)
{
     while(!LL_USART_IsActiveFlag_TXE(UART5))
         ;
      LL_USART_TransmitData8(UART5, c);
}
*/

void U4_TxWait(void)
{
    while ((iUartTable[COM4].hUart->Instance->ISR & UART_FLAG_TXE) == RESET)
        ;
}

void U5_TxWait(void)
{
    while ((iUartTable[COM5].hUart->Instance->ISR & UART_FLAG_TXE) == RESET)
        ;
}

void U4_Putch(BYTE c)
{
    IUart_SendByte(COM4, (uint8_t)c);
}

void U5_Putch(BYTE c)
{
    IUart_SendByte(COM5, (uint8_t)c);
}

/*****END OF FILE****/
