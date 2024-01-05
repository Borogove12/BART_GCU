/*******************************************************************************
 *       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : common_func.h
 * Author       : Joseph Yosep Choi
 * Date Created : 2022/09/14
 * Description  : Definition of common functions
 *******************************************************************************/

#ifndef __COMMON_FUNC_H_
#define __COMMON_FUNC_H_

#include "gcu_def.h"
#include "userlib.h"
#include "repository.h"
#include "printf.h"

/* Global variables ----------------------------------------------------------*/
extern BYTE gbIndicatorBlink;
extern BYTE gbIndicatorCMD_EN;
extern BYTE gbIndicatorCMD_EX;
extern BYTE gbLampCMD_EN;
extern BYTE gbLampCMD_EX;
extern BYTE gbPushFlag;
extern BYTE gbPowerFailFlag;
extern BYTE gbIndicatorFlag;
extern BYTE gbEmergencyFlag;
extern BYTE gbAuthDirection;
extern BYTE gbPrevEmgSignal;

extern bool gfModeChanged;
extern bool gfisAuthTimeout;
extern bool gfisBarrierEmg;

extern DWORD gdwBuzzerTimeout;
extern DWORD gdwTimeoutIndicator;
extern DWORD gdwTimeoutSafety;
extern DWORD gdwTimeoutLuggage;
extern DWORD gdwTimeoutBlink;
extern DWORD gdwTimeoutAuthorize;
extern DWORD gdwTimeoutJumping;
extern DWORD gdwTimeoutUnauthorize;

extern TTIMER timerAuthorize;
extern TTIMER timerEmergency;
extern TTIMER timerPassageClear;
extern TTIMER timerTailgatingDetection;
extern TTIMER timerIllegalEnterEN;
extern TTIMER timerIllegalEnterEX;
extern TTIMER timerBlink;
extern TTIMER timerBarrierCheck;
extern TTIMER timerPush;
extern TTIMER timerIndicator;
extern TTIMER timerLuggageWait;
extern TTIMER timerSafety;
extern TTIMER timerJumping;
extern TTIMER timerTailgating;
extern TTIMER timerBarrierStop;
extern TTIMER timerInverseEnter;
extern TTIMER timerReset;
extern TTIMER timerCloseWait;
extern TTIMER timerStopHolding;
extern TTIMER timerTgFilter;
extern TTIMER timerBuzzer;
extern TTIMER timerBarrierCmdWait;

extern void SetDefaultOpMode(void);
extern void SetDefaultParameter(void);
extern void SetDefaultStatus(void);
extern void ApplyStatusMode(void);
extern void SetNewOpMode(T_GCU_OP_MODE *pNewMode);
extern void CheckModeChange(void);
extern void GetCurrentOpMode(T_GCU_OP_MODE *pCurMode);
extern void SetGCUParameter(T_GCU_PARAMETER *pNewParameter, int nLen);
extern void GetGCUParameter(T_GCU_PARAMETER *pCurParameter);
extern void GetGCUStatus(T_GCU_STATUS_RES *pCurStatus);

extern void GetAuthCount(T_GCU_AUTH_COUNT *pAuthCount);
extern void IncreaseAuthCount(BYTE bDir);
extern void ClearAuthCount(void);

extern void ControlBarrier(BYTE bFDoorControl);
extern void ControlBuzzer(BYTE bBuzzerControl, BYTE bDuration);
extern void ControlDirectionLED(BYTE bLEDControl_EN, BYTE bLEDControl_EX);
extern void ControlStatusLED(BYTE bLEDControl_EN, BYTE bLEDControl_EX);
extern void ControlOverheadDisplay(BYTE bLEDControl_EN, BYTE bLEDControl_EX);
extern void ControlIndicator(BYTE bLEDControl_EN, BYTE bLEDControl_EX, BYTE bDuration, bool isOneTime);
extern void ControlIndicatorBlink(BYTE isSet);

extern void CheckUPSStatus(void);
extern void CheckEmergencySignal(void);
extern void CheckBuzzerTimer(void);
extern void CheckIndicatorTimer(void);
extern void CheckPushTimer(BYTE command);

extern void CheckPassSenError(bool isSwing);

extern void InhibitPass(int nDir);
extern void ClearPassageAlarm(void);

extern void CheckAuthTimeout(void);
extern void CheckIllegalAlarmTimer(void);
extern void CheckBlinkTimer(void);
extern void SetAuthTimer(void);
extern void SetTailgatingTimer(void);

extern void InitPassageMode(void);
extern void ClearIllegalPass(void);
extern void CheckIllegalEnterTimer(BYTE dir);

void ControlIndicator4Test(BYTE bLEDControl_EN, BYTE bLEDControl_EX);

#endif // __COMMON_FUNC_H_
    /******* COPYRIGHT �� 2022 STraffic Co., Ltd.  ********END OF FILE****/
