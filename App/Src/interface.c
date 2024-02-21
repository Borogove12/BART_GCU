/*******************************************************************************
*       COPYRIGHT �� 2020 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : interface.c
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : GCU application to interface with ECU
*******************************************************************************/

#include <string.h>
//#include "71x_lib.h"
#include "gcu_def.h"
#include "userlib.h"
#include "serial.h"
#include "interface.h"
#include "passsvc_swing.h"
#include "test.h"
#include "repository.h"
#include "fwdown.h"
#include "bsp.h"
#include "common_func.h"

T_PROTOCOL_FRAME GCUCommand;
int	gnFrameLen;

T_PROTOCOL_FRAME	GCUResponse;
T_PROTOCOL_FRAME	LastHostCMDResponse;

BYTE	gbLastMainSeq;
BYTE	gbLastHostCMD;

BYTE gbHostBaudRate = BAUD_57600;	// default baud rate

int  gnParameterLength = sizeof(T_GCU_PARAMETER);

//extern void UARTxConfig(UART_TypeDef *UARTx, u32 nBaudRate);			pms



/*******************************************************************************
* Name:     SetDoorControl
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void SetDoorControl(BYTE bCMDOption)
{
	if (bCMDOption == 0)
	{
		if (gfTestMode == TRUE)
		{
			gfFreeMode = TRUE;
		}
		else
		{
			gfFreeMode = FALSE;
		}
		Brr_CloseBarrier();
		// ControlBarrier(BARRIER_CLOSE);
	}
	else if (bCMDOption == 1)
	{
		gfFreeMode = TRUE;
		Brr_OpenBarrier(BARRIER_OPEN_FOR_EX);
		// ControlBarrier(BARRIER_OPEN_FOR_EX);	
	}
	else if (bCMDOption == 2)
	{
		gfFreeMode = TRUE;
		Brr_OpenBarrier(BARRIER_OPEN_FOR_EN);
		// ControlBarrier(BARRIER_OPEN_FOR_EN);
	}
	else if (bCMDOption == 3)
	{
		gfTestMode = TRUE;
		Brr_SetEmergency(FALSE);
	}
	else if (bCMDOption == 4)
	{
		gfTestMode = FALSE;
		gfFreeMode = FALSE;
		Brr_SetEmergency(TRUE);
	}
}

/*******************************************************************************
* Name:     SendACK
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void SendACK(void)
{
	U3_Putch(ACK);
}

/*******************************************************************************
* Name:     SendNAK
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void SendNAK(void)
{
	U3_Putch(NAK);
}

/*******************************************************************************
* Name:     IsCommandNeedResponse
* Function: 
* In:       bCMDCode
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
bool IsCommandNeedResponse(BYTE bCMDCode)
{
	int i;
	BYTE no_response_code[] = {
		CMD_SET_SCADA_OUT,
		CMD_INFORM_ALARM,
		CMD_CONTROL_ALL,
		CMD_CONTROL_DOOR,
		CMD_CONTROL_INDICATOR,
		CMD_CONTROL_LAMP,
		CMD_CONTROL_BUZZER,
		CMD_SAFETY_STOP,
		CMD_WRITE_REGISTER,
		CMD_SET_UPS_COMMAND};

	for (i = 0; i < sizeof(no_response_code); i++)
	{
		if (no_response_code[i] == bCMDCode)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*******************************************************************************
* Name:     MakeResponse
* Function: 
* In:       pbResOption
*			nOptionLen
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void MakeResponse(BYTE *pbResOption, int nOptionLen)
{
	GCUResponse.d.bMainSeq = GCUCommand.d.bMainSeq;

	if (GCUCommand.d.bMouleSeq == MAX_SEQUENCE)
		GCUResponse.d.bMouleSeq = 0;
	else
		GCUResponse.d.bMouleSeq = (GCUCommand.d.bMouleSeq+1);

	GCUResponse.d.bCmdCode = GCUCommand.d.bCmdCode;
	GCUResponse.d.bDataLen = nOptionLen+sizeof(GCUResponse.d.bCmdCode);	// add command length

	if (nOptionLen)
		memcpy(GCUResponse.d.mbOptionData, pbResOption, nOptionLen);
}

/*******************************************************************************
* Name:     SendResponse
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void SendResponse(void)
{
	BYTE	bBCC;
	BYTE	*pbResBuff;
	int		nResLen;

	pbResBuff	= GCUResponse.Buff;
	nResLen		= GCUResponse.d.bDataLen+FRAME_HEADER_LEN;

	// send STX
	U3_Putch(STX);
	bBCC = STX;				// BCC calculation

	// send data
	while (nResLen--)
	{
		// DLE processing
		if ((*pbResBuff == STX) || (*pbResBuff == ETX) || (*pbResBuff == DLE))
		{
			U3_Putch(DLE);
			bBCC ^= DLE;				// BCC calculation

			U3_Putch((*pbResBuff+DLE_OP));
			bBCC ^= (*pbResBuff+DLE_OP);	// BCC calculation
			pbResBuff++;
		}
		else
		{
			U3_Putch(*pbResBuff);
			bBCC ^= *pbResBuff;			// BCC calculation
			pbResBuff++;
		}
	}

	// send ETX
	U3_Putch(ETX);
	bBCC ^= ETX;

	// send BCC
	U3_Putch(bBCC);

#if _RES_DUMP_
	PRINT("\nRES %02X Data", GCUResponse.d.bCmdCode);
	Print2Hex("", GCUResponse.Buff, GCUResponse.d.bDataLen+FRAME_HEADER_LEN);
#endif
}

/*******************************************************************************
* Name:     CMDSetGCUOpMode
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDSetGCUOpMode(void)
{
	SetNewOpMode((T_GCU_OP_MODE *)GCUCommand.d.mbOptionData);
	MakeResponse(GCUCommand.d.mbOptionData, sizeof(T_GCU_OP_MODE));
}

/*******************************************************************************
* Name:     CMDGetGCUOpMode
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetGCUOpMode(void)
{
	T_GCU_OP_MODE CurMode;

	GetCurrentOpMode(&CurMode);
	MakeResponse((BYTE*)&CurMode, sizeof(T_GCU_OP_MODE));
}

/*******************************************************************************
* Name:     CMDSetGCUParameter
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDSetGCUParameter(void)
{
	gnParameterLength = GCUCommand.d.bDataLen-1;

	SetGCUParameter((T_GCU_PARAMETER*)GCUCommand.d.mbOptionData, gnParameterLength);
	MakeResponse(GCUCommand.d.mbOptionData, gnParameterLength);
}

/*******************************************************************************
* Name:     CMDGetGCUParameter
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetGCUParameter(void)
{
	T_GCU_PARAMETER	GurGCUParameter;

	GetGCUParameter(&GurGCUParameter);

	MakeResponse((BYTE*)&GurGCUParameter, gnParameterLength);
}

/*******************************************************************************
* Name:     CMDGetVersion
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetVersion(void)
{
	// Brr_SetSerialComm(TRUE);
	MakeResponse((BYTE*)GCU_FW_VERSION, sizeof(GCU_FW_VERSION));
}

/*******************************************************************************
* Name:     CMDSetScadaOut
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/

void CMDSetScadaOut(void)
{
	T_CMD_SCADA_OUT *pScadaOut = (T_CMD_SCADA_OUT *)GCUCommand.d.mbOptionData;

	outb(WRITE03_ADR, pScadaOut->bSCADA_Out & 0x0F);
}

/*******************************************************************************
* Name:     CMDResetGCU
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
BYTE ResetCMD = 0;

void CMDResetGCU(void)
{
	MakeResponse(NULL, 0);
	SendResponse();
	ResetCMD = 0x1;
	flash_write(0x4000, (byte *)&ResetCMD, 1);
	HAL_Delay(50);
	NVIC_SystemReset();
}

/*******************************************************************************
* Name:     CMDAuthorizePass
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDAuthorizePass(void)
{
	T_CMD_AUTH_PASS *pcmdAuthPass = (T_CMD_AUTH_PASS*)GCUCommand.d.mbOptionData;
	T_GCU_AUTH_COUNT resAuthCount;

	if (pcmdAuthPass->bAuthCount == 0)
	{		
		printf(" [CMDAuthorizePass] zero count! - A:%d, D:%d \n", pcmdAuthPass->bAuthCount, pcmdAuthPass->bDirection);
		printf(" [CMDAuthorizePass] org data. - A:%d, D:%d \n", GCUCommand.d.mbOptionData[1], GCUCommand.d.mbOptionData[0]);
		pcmdAuthPass->bAuthCount = 1;
	}

	IncreaseAuthCount(pcmdAuthPass->bDirection);

	if (pcmdAuthPass->bDirection == FROM_ENTRY)
	{
		ControlIndicator(pcmdAuthPass->bLamp, LAMP_OFF, 0, FALSE);
		ControlDirectionLED(pcmdAuthPass->bLampBar, DIR_RED);
	}
	else
	{
		ControlIndicator(LAMP_OFF, pcmdAuthPass->bLamp, 0, FALSE);
		ControlDirectionLED(DIR_RED, pcmdAuthPass->bLampBar);
	}

	GetAuthCount(&resAuthCount);
	MakeResponse((BYTE*)&resAuthCount, sizeof(T_GCU_AUTH_COUNT));
}

/*******************************************************************************
* Name:     CMDClearAuthorization
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDClearAuthorization(void)
{
	T_GCU_AUTH_COUNT resAuthCount;

	ClearAuthCount();
	GetAuthCount(&resAuthCount);

	MakeResponse((BYTE*)&resAuthCount, sizeof(T_GCU_AUTH_COUNT));
}

/*******************************************************************************
* Name:     CMDGetStatus
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetStatus(void)
{
	T_GCU_STATUS_RES GurStatus;
    T_CMD_SAFETY_STOP *pcmdSafetyStop = (T_CMD_SAFETY_STOP *)GCUCommand.d.mbOptionData;
	gfAIDetection = pcmdSafetyStop->bSafety;
    
	GetGCUStatus(&GurStatus);

	MakeResponse((BYTE*)&GurStatus, sizeof(T_GCU_STATUS_RES));
}

/*******************************************************************************
* Name:     CMDGetSensorSwitch
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetSensorSwitch(void)
{
	// not used
}

/*******************************************************************************
* Name:     CMDControlAllLED
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDControlAllLED(void)
{
	T_CMD_CONTROL_ALL_LED *pcmdControlAll = (T_CMD_CONTROL_ALL_LED *)GCUCommand.d.mbOptionData;

	// printf(" CMDControlAllLED = %d/%d/%d/%d/%d/%d \n", pcmdControlAll->bLamp_EN, pcmdControlAll->bLamp_EX, pcmdControlAll->bOverhead_EN, pcmdControlAll->bOverhead_EX, pcmdControlAll->bIndicator_EN, pcmdControlAll->bIndicator_EX);
	
	if (pcmdControlAll->test_flag == SET)
	{
		ControlStatusLED(pcmdControlAll->bLamp_EN, pcmdControlAll->bLamp_EX);
		ControlOverheadDisplay(pcmdControlAll->bOverhead_EN, pcmdControlAll->bOverhead_EX);
		ControlIndicator4Test(pcmdControlAll->bIndicator_EN, pcmdControlAll->bIndicator_EX);
	}
	else
	{
		ControlDirectionLED(pcmdControlAll->bLamp_EN, pcmdControlAll->bLamp_EN);
		ControlIndicator(pcmdControlAll->bIndicator_EN, pcmdControlAll->bIndicator_EX, 0, FALSE);
	}
}

/*******************************************************************************
* Name:     CMDControlBarrier
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDControlBarrier(void)
{
	SetDoorControl(GCUCommand.d.mbOptionData[0]);
}

/*******************************************************************************
* Name:     CMDControlIndicator
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDControlIndicator(void)
{
	T_CMD_CONTROL_INDICATOR *pcmdIndicator = (T_CMD_CONTROL_INDICATOR *)GCUCommand.d.mbOptionData;
	ControlIndicator4Test(pcmdIndicator->bIndicator_EN, pcmdIndicator->bIndicator_EX);	
}

/*******************************************************************************
* Name:     CMDControlLamp
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDControlLamp(void)
{
	T_CMD_CONTROL_LAMP *pcmdLamp = (T_CMD_CONTROL_LAMP *)GCUCommand.d.mbOptionData;
	ControlStatusLED(pcmdLamp->bLamp_EN, pcmdLamp->bLamp_EX);
}

/*******************************************************************************
* Name:     CMDControlBuzzer
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDControlBuzzer(void)
{
	T_CMD_CONTROL_BUZZER *pcmdBuzzer = (T_CMD_CONTROL_BUZZER *)GCUCommand.d.mbOptionData;

	if (GCUCommand.d.bDataLen == (sizeof(T_CMD_CONTROL_BUZZER)+1))
	{
		if (pcmdBuzzer->bBuzzerNo & MASK_TEST_BUZZER_MAIN)
			ControlBuzzer(pcmdBuzzer->bBuzzerControl, pcmdBuzzer->bBuzzerDuration);
	}
	else
	{
		ControlBuzzer(pcmdBuzzer->bBuzzerControl, pcmdBuzzer->bBuzzerDuration);
	}	
}

/*******************************************************************************
* Name:     CMDSafetyStop
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDSafetyStop(void)
{
	T_CMD_SAFETY_STOP *pcmdSafetyStop = (T_CMD_SAFETY_STOP *)GCUCommand.d.mbOptionData;
	gfAIDetection = pcmdSafetyStop->bSafety;
}

#define MAX_WRITE_REG	9
#define ECU_OUT_PT		9
#define MISC_OUT_PT		10
#define CIC_OUT_PT		11
#define EMG_OUT_PT		12

#define BIT_RETURNCUP	BIT1
#define BIT_MAIN_BUZZ	BIT2
#define BIT_EN_BUZZ		BIT3
#define BIT_EX_BUZZ		BIT4

/*******************************************************************************
* Name:     CMDWriteRegister
* Function: 
* In:       None
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/

void CMDWriteRegister(void)
{
	BYTE	*pbControl = (BYTE *)GCUCommand.d.mbOptionData;
	BYTE	dwPrimaryLEDValue 		= 0;
	BYTE	dwSecondaryLEDValue 	= 0;
	BYTE	dwBarrierValue 			= 0;
	BYTE	dwScadaValue 			= 0;
	BYTE	dwUPSCHKValue 			= 0;
	BYTE	dwUPSSHDNKValue 		= 0;

	printf("pbControl[0] = [%d] \n",pbControl[0]);      //Barrier, S_EMG = [0x80], S_DIR = [0x40], S_BRAKE = [0x20], S_OPEN = [0x10], P_EMG = [0x08], P_DIR = [0x04], P_BRAKE = [0x02], P_OPEN = [0x01]
	printf("pbControl[1] = [%d] \n",pbControl[1]);		//SCADA OUT, NXO = [0x8], TDO = [0x4], EBO = [0x2], OOS = [0x1]
	printf("pbControl[2] = [%d] \n",pbControl[2]);
	printf("pbControl[3] = [%d] \n",pbControl[3]);		//jig master dir green = [0x4], red = [0x2], yellow = [0x1]
	printf("pbControl[4] = [%d] \n",pbControl[4]);		//jig slave dir green = [0x4], red = [0x2], yellow = [0x1]
	printf("pbControl[5] = [%d] \n",pbControl[5]);
	printf("pbControl[6] = [%d] \n",pbControl[6]);		//jig master alm green = [0x4], blue = [0x2], red = [0x1]
	printf("pbControl[7] = [%d] \n",pbControl[7]);		//jig slave alm green = [0x4], blue = [0x2], red = [0x1]
	printf("pbControl[8] = [%d] \n",pbControl[8]);		//default 0x33 SHDN button 0x01 CHK 0x02 reverse

	// Barrier - Primary
	dwBarrierValue =  (pbControl[0] & 0x01)? 0x01: 0x00;			// output Primary Barrier - Open/Close
	dwBarrierValue |= (pbControl[0] & 0x02)? 0x02: 0x00;			// output Primary Barrier - Emg(Free)
	dwBarrierValue |= (pbControl[0] & 0x04)? 0x04: 0x00;			// output Primary Barrier - Direction
	dwBarrierValue |= (pbControl[0] & 0x08)? 0x08: 0x00;			// output Primary Barrier - Brake(Stop)

	// Barrier - Secondary
	dwBarrierValue |= (pbControl[0] & 0x10)? 0x10: 0x00;			// output Secondary Barrier - Open/Close
	dwBarrierValue |= (pbControl[0] & 0x20)? 0x20: 0x00;			// output Secondary Barrier - Emg(Free)
	dwBarrierValue |= (pbControl[0] & 0x40)? 0x40: 0x00;			// output Secondary Barrier - Direction
	dwBarrierValue |= (pbControl[0] & 0x80)? 0x80: 0x00;			// output Secondary Barrier - Brake(Stop)
	
	outb(WRITE02_ADR, dwBarrierValue);

	// SCADA
	dwScadaValue =  (pbControl[1] & 0x01)? 0x01: 0x00;				// output SCADA - OOS
	dwScadaValue |= (pbControl[1] & 0x02)? 0x02: 0x00;				// output SCADA - EBO
	dwScadaValue |= (pbControl[1] & 0x04)? 0x04: 0x00;				// output SCADA - TDO
	dwScadaValue |= (pbControl[1] & 0x08)? 0x08: 0x00;				// output SCADA - NXO

	outb(WRITE03_ADR, dwScadaValue);

	// Status Display - Primary
	dwPrimaryLEDValue =  (pbControl[3] & 0x01)? 0x01: 0x00;			//Status display - green
	dwPrimaryLEDValue |= (pbControl[3] & 0x02)? 0x02: 0x00;			//Status display - red

	// Overhead Display - Primary
	dwPrimaryLEDValue |= (pbControl[3] & 0x04)? 0x04: 0x00;			//Overhead display - green
	dwPrimaryLEDValue |= (pbControl[3] & 0x08)? 0x08: 0x00;			//Overhead display - red

	// Status Display - Secondary
	dwPrimaryLEDValue |= (pbControl[4] & 0x01)? 0x10: 0x00;		//Status display - green
	dwPrimaryLEDValue |= (pbControl[4] & 0x02)? 0x20: 0x00;		//Status display - red

	// Overhead Display - Secondary
	dwPrimaryLEDValue |= (pbControl[4] & 0x04)? 0x40: 0x00;		//Overhead display - green
	dwPrimaryLEDValue |= (pbControl[4] & 0x08)? 0x80: 0x00;		//Overhead display - red

	outb(WRITE00_ADR, dwPrimaryLEDValue);


	// Indicator Light - Primary
	dwSecondaryLEDValue  = (pbControl[6] & 0x01)? 0x01: 0x00;			//red
	dwSecondaryLEDValue |= (pbControl[6] & 0x04)? 0x02: 0x00;			//green
	dwSecondaryLEDValue |= (pbControl[6] & 0x02)? 0x04: 0x00;			//blue

	// Indicator Light - Secondary
	dwSecondaryLEDValue |= (pbControl[7] & 0x01)? 0x10: 0x00;		//red
	dwSecondaryLEDValue |= (pbControl[7] & 0x04)? 0x20: 0x00;		//green
	dwSecondaryLEDValue |= (pbControl[7] & 0x02)? 0x40: 0x00;		//blue


	// Write Register

	outb(WRITE01_ADR, dwSecondaryLEDValue);	

	if (pbControl[8] == 0x33)
	{
		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, GPIO_PIN_SET);
	}
	else
	{
		dwUPSCHKValue  = (pbControl[8] & 0x02)? 0x00: 0x01;
		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_CHK_Pin, dwUPSCHKValue);

		dwUPSSHDNKValue = (pbControl[8] & 0x01)? 0x00: 0x01;
		HAL_GPIO_WritePin(nUPS_GPIO_Port, UPS_SHDN_Pin, dwUPSSHDNKValue);

	}

	//pbControl[5] =  0;						// Control dummy direction display (WMATA not used)
	//pbControl[8] => UPS Shuddown On [0x00], UPS Shuddown Off [0x01] / UPS Check On [0x02], UPS Check Off [0x00] 
	//pbControl[8] =  0;						// UPS Command - BART도 UPS I/F 사용 (Out: 2EA) - 현재 SHDN 1EA만 구현되어 있으나 CHK신호 추가예정 -> Jig Program 에서는 UPS Write 시그널 줌. 20231002 by Joseph

	if (pbControl[10])	
		BuzzerOn();			//buzzer 한개만 사용
	else										
		BuzzerOff();
}

#define MAX_READ_REG	11

/*******************************************************************************
* Name:     CMDReadRegister
* Function: 
* In:       None
* Out:      Values of current register
* Return:   void
* comments: 
*******************************************************************************/
void CMDReadRegister(void)			//pms		이 함수 모두 확인 필요
{
	BYTE	mbReadData[MAX_READ_REG];			//0 ~ 10 = Total 11 0x6400000B 까지 read
	DWORD	dwSpareSenserValue  = 0;
	DWORD	dwUPSValue = 0;

	dwUPSValue = (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_CONN_Pin))? 0x01: 0x00;			// Connection Failure, High active //stm32L496 적용 보드에서는 conn이 없었음
	dwUPSValue |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_PWR_FAIL_Pin))? 0x02: 0x00;		// Power Failure, Low active
	dwUPSValue |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_LOW_BAT_Pin))? 0x04: 0x00;		// Low battery, Low active

	// BART 에서는 Passenger sensor 01 ~ 16(2 bytes) 만 사용
	dwSpareSenserValue = (HAL_GPIO_ReadPin(SP_SEN_GPIO_Port,SP_SEN1_Pin))? 0x01: 0x00;  	// Passenger sensor 3 - BART는 Passenger sensor 17, 18는 Spare로 GPIO 별도 할당(회로상엔 9, 19로 표기)
	dwSpareSenserValue |= (HAL_GPIO_ReadPin(SP_SEN_GPIO_Port,SP_SEN2_Pin))? 0x02: 0x00;

	mbReadData[0] =  RD_SWITCH & 0x0F;			// Door/Module switch 1 - BART는 Door Switch 1~4만 사용, High - detected, Low - not detected
	mbReadData[1] =  (RD_SWITCH >> 4) & 0x0F;	// Door/Module switch 2 - BART는 Door Switch 5~8만 사용, High - detected, Low - not detected
	mbReadData[2] =  0;							// Token capture unit sensor - Token box 없음
	mbReadData[3] =  (*(BYTE*)READ00_ADR);		// Passenger sensor 1 - BART는 Passenger sensor 1~8사용, High - detected, Low - not detected
	mbReadData[4] =  (*(BYTE*)READ01_ADR);		// Passenger sensor 2 - BART는 Passenger sensor 9~16사용(회로상엔 11~18로 표기) -> 회로상에도 9~16으로 표기 바랍니다 20230927 by Joseph, High - detected, Low - not detected
	mbReadData[5] =  dwSpareSenserValue;		// Passenger sensor 3 - BART는 Passenger sensor 17, 18는 Spare로 GPIO 별도 할당(회로상엔 9, 19로 표기) -> 회로상에도 스페어는 17,18 로 표기 바랍니다 20230927 by Joseph
												// add - jig test에서는 17 18 할당 -> JIG 에서는 17,18 sensor 체크는 뺄 예정입니다. 20230927 by Joseph
	mbReadData[6] =  0;							// Spare sensor (WMATA not used) -> JIG 에서는 Spare sensor 체크는 뺄 예정입니다. 20230927 by Joseph
	mbReadData[7] =  (*(BYTE*)READ04_ADR);		// Barrier status, High active (S_Brake:S_Abnormal:S_Fault:S_Position:P_Brake:P_Abnormal:P_Fault:P_Position)
	mbReadData[8] =  (*(BYTE*)READ03_ADR);		// SCADA, High active
	mbReadData[9] =  IsEMGSignalOn();			// External IRQ source  - 인터럽트 신호 BART에서 사용여부는 첨부 GCU_IO Specification 참조 -> EBO (Emergency) Signal 값으로 변경. 20231002 by Joseph
	mbReadData[10] =  dwUPSValue;				// UPS Status - BART도 UPS I/F 사용 (In: 3EA) 

	MakeResponse(mbReadData, sizeof(mbReadData));
}

/*******************************************************************************
* Name:     CMDGetCPLDVersion
* Function: 
* In:       None
* Out:      Current CPLD version
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetCPLDVersion(void)
{
	BYTE	bCPLDVersion = 0;

	MakeResponse(&bCPLDVersion, sizeof(bCPLDVersion));
}

/*******************************************************************************
* Name:     CMDGetDipSwitch
* Function: 
* In:       None
* Out:      Value of current dip switch 
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetDipSwitch(void)
{
	BYTE dipSwitch = ReadDipSwitch();

	MakeResponse(&dipSwitch, sizeof(dipSwitch));
}

/*******************************************************************************
* Name:     CMDSetUPSCommand
* Function: 
* In:       Command
* Out:      None
* Return:   void
* comments: 
*******************************************************************************/
void CMDSetUPSCommand(void)
{
	// Set UPS command
	BYTE bUPSCommand = GCUCommand.d.mbOptionData[0];

	printf(" [CMDSetUPSCommand] %d \n", bUPSCommand);	

	switch (bUPSCommand)
	{
	case UPS_CMD_CHECK_ON:
		ControlUPS_CheckOn();
		ControlUPS_ShutDownOff();
		break;
	case UPS_CMD_SHUTDOWN:
		ControlUPS_ShutDownOn();
		break;	
	default:
		ControlUPS_CheckOn();
		ControlUPS_ShutDownOff();
		break;
	}
}

/*******************************************************************************
* Name:     CMDGetUPSStatus
* Function: 
* In:       None
* Out:      Current UPS Status
* Return:   void
* comments: 
*******************************************************************************/
void CMDGetUPSStatus(void)
{
	BYTE bUPSStatus = 0;

	bUPSStatus = (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_CONN_Pin))? 0x01: 0x00;			// Connection Failure, High active
	bUPSStatus |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_PWR_FAIL_Pin))? 0x02: 0x00;	// Power Failure, Low active
	bUPSStatus |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port,nUPS_LOW_BAT_Pin))? 0x04: 0x00;		// Low battery, Low active

	MakeResponse(&bUPSStatus, sizeof(bUPSStatus));
}

/*******************************************************************************
* Name:     CheckProtocol_UART3
* Function: Main GCU Process
* In:       None
* Out:      None
* Return:   int - SUCCESS (0) : one command is received
*			    - else        : no command is received
* comments: It checks serial data from Host and protocol format of data received.
*           If the protocol format is correct, it returns SUCCESS result.
*******************************************************************************/

int CheckProtocol_UART3(void)
{
	static BYTE bRcvState = STATE_STX_WAIT;
	static BYTE bBCC;
	static BYTE bRcvData;

	while (IUart_RecvByte_check(COM3, &bRcvData))
	{
		if (bRcvState == STATE_DATA_BCC)
		{
			bRcvState = STATE_STX_WAIT;

			if (bBCC == bRcvData)
			{
				if (GCUCommand.d.bDataLen != (BYTE)(gnFrameLen - FRAME_HEADER_LEN))
				{
					printf("LENGTH ERR: %d - %d \n", GCUCommand.d.bDataLen, (gnFrameLen - FRAME_HEADER_LEN));
					gnFrameLen = 0;
					return ERR_WRONG_CMD_LEN;
				}
				else
					return SUCCESS;
			}
			else
			{
				printf("len: %d, BCC: %d, rcv: %d \n", gnFrameLen, bBCC, bRcvData);
				Print2Hex("", GCUCommand.Buff, gnFrameLen);
				gnFrameLen = 0;
				return ERR_BCC;
			}
		}
		else
		{
			switch (bRcvData)
			{
			case STX:
				bRcvState = STATE_DATA_RCV;
				bBCC = bRcvData;
				gnFrameLen = 0;
				break;
			case ETX:
				bRcvState = STATE_DATA_BCC;
				bBCC ^= bRcvData;
				break;
			case DLE:
				if (bRcvState == STATE_DATA_RCV)
				{
					bRcvState = STATE_DATA_DLE;
					bBCC ^= bRcvData;
				}
				break;
			default:
				if (bRcvState == STATE_DATA_RCV)
				{
					GCUCommand.Buff[gnFrameLen++] = bRcvData;
					bBCC ^= bRcvData;
				}
				else if (bRcvState == STATE_DATA_DLE)
				{
					if ((bRcvData == DLE_STX) || (bRcvData == DLE_ETX) || (bRcvData == DLE_DLE))
					{
						bRcvState = STATE_DATA_RCV;
						GCUCommand.Buff[gnFrameLen++] = (bRcvData ^ DLE_OP);
						bBCC ^= bRcvData;
					}
					else
					{
						bRcvState = STATE_STX_WAIT;
						gnFrameLen = 0;
						printf("ERR_DLE_PROCESS \n");
						return ERR_DLE_PROCESS;
					}
				}
				else	// STATE_STX_WAIT
				{
					if (bRcvData == ACK)
						return ACK_RECEIVED;
					else if (bRcvData == NAK)
						return NAK_RECEIVED;
				}
				break;
			}
		}

		if (gnFrameLen >= FRAME_LEN_RET)
		{
			bRcvState = STATE_STX_WAIT;
			gnFrameLen = 0;
			printf("ERR_FRAME_LENGTH \n");
			return ERR_FRAME_LENGTH;
		}
	}

	return ERR_NO_DATA;
}

/*******************************************************************************
* Name:     ProcessCommand
* Function: process command from Host
* In:       None
* Out:      None
* Return:   - SUCCESS (0) : new command is processed successfully
*			- ERR_OLD_SEQUENCE : same command is received again
*			- ERR_ILLEGAL_CMD  : command code is not defined
* comments: It checks serial data from Host and protocol format of data received.
*           If the protocol format is correct, it returns SUCCESS result.
*******************************************************************************/
int ProcessCommand(void)
{
	if (GCUCommand.d.bMainSeq == gbLastMainSeq)	// Same main sequence
	{
		if (GCUCommand.d.bCmdCode == GCUResponse.d.bCmdCode)	// command code is same with that of the last response
			return ERR_OLD_SEQUENCE;
		else if (GCUCommand.d.bCmdCode == gbLastHostCMD)		// command code is same with last host command code
		{
			if (!IsCommandNeedResponse(GCUCommand.d.bCmdCode))
			{
				return ERR_OLD_SEQ_NO_RESPONSE;
			}
			else if (GCUCommand.d.bCmdCode == LastHostCMDResponse.d.bCmdCode)
			{// command code is same with that of the stored response for the last host command.
				memcpy(&GCUResponse, &LastHostCMDResponse, (LastHostCMDResponse.d.bDataLen+FRAME_HEADER_LEN));
				return ERR_OLD_SEQUENCE;
			}
		}
		// except above case, process the command from Host 
	}
	
	SendACK();

	gbLastHostCMD = GCUCommand.d.bCmdCode;

	switch (GCUCommand.d.bCmdCode)	// command
	{
	case CMD_SET_MODE:			CMDSetGCUOpMode();		break;
	case CMD_GET_MODE:			CMDGetGCUOpMode();		break;
	case CMD_SET_PARAMETER:		CMDSetGCUParameter();	break;
	case CMD_GET_PARAMETER:		CMDGetGCUParameter();	break;
	case CMD_GET_VERSION:		CMDGetVersion();		break;
	case CMD_SET_SCADA_OUT:		CMDSetScadaOut();		break;
	case CMD_SW_RESET:			CMDResetGCU();			break;
	case CMD_AUTHORIZE:			CMDAuthorizePass();		break;
	case CMD_CLEAR_AUTH:		CMDClearAuthorization();break;
	case CMD_GET_STATUS:		CMDGetStatus();			break;
	case CMD_GET_SENSOR_SWITCH:	CMDGetSensorSwitch();	break;
	case CMD_CONTROL_ALL:		CMDControlAllLED();		break;
	case CMD_CONTROL_DOOR:		CMDControlBarrier();	break;
	case CMD_CONTROL_LAMP:		CMDControlLamp();		break;
	case CMD_CONTROL_INDICATOR:	CMDControlIndicator();	break;
	case CMD_CONTROL_BUZZER:	CMDControlBuzzer();		break;
	case CMD_SAFETY_STOP:		CMDSafetyStop();		break;
	case CMD_WRITE_REGISTER:	CMDWriteRegister();		break;
	case CMD_READ_REGISTER:		CMDReadRegister();		break;
	case CMD_GET_CPLD_VERSION:	CMDGetCPLDVersion();	break;
	case CMD_GET_DIP_SWITCH:	CMDGetDipSwitch();		break;
	case CMD_SET_UPS_COMMAND:	CMDSetUPSCommand();		break;
	case CMD_GET_UPS_STATUS:	CMDGetUPSStatus();		break;
	default:
		return ERR_ILLEGAL_CMD;
	}

	return SUCCESS;
}

/*******************************************************************************
 * Name:     ProcessComm_ECU
 * Function: Communication process for ECU
 * In:       None
 * Out:      None
 * Return:   void
 * comments: it checks serial data from Host and protocol format of data received.
 *           If the protocol format is correct, it makes action refer to the command
 *           from Host.
 *******************************************************************************/
void ProcessComm_ECU(void)
{
	int nError = CheckProtocol_UART3();
	if (nError == SUCCESS)
	{
		nError = ProcessCommand();
		if (nError == SUCCESS)
		{
			if (IsCommandNeedResponse(GCUCommand.d.bCmdCode))
			{
				SendResponse();
				memcpy(&LastHostCMDResponse, &GCUResponse, (GCUResponse.d.bDataLen + FRAME_HEADER_LEN));
			}
		}
		else if (nError == ERR_OLD_SEQUENCE)
			SendResponse();
		else if (nError == ERR_ILLEGAL_CMD)
			SendNAK();
	}
	else
	{
		if ((nError == ERR_BCC) || (nError == ERR_WRONG_CMD_LEN) || (nError == ERR_DLE_PROCESS))
			SendNAK();
		else if (nError == NAK_RECEIVED)
			SendResponse();
	}
}
/*****END OF FILE****/
