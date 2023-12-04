/*******************************************************************************
 *       COPYRIGHT �� 2022 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : common_func.c
 * Author       : Joseph Yosep Choi
 * Date Created : 2022/09/14
 * Description  : Implementation of common functions
 *******************************************************************************/

#include <string.h>
#include "common_func.h"
#include "passsvc_swing.h"
#include "global.h"
#include "bsp.h"

// Indicator Light, Concession Lamp
BYTE gbIndicatorBlink = FLG_OFF;
BYTE gbIndicatorCMD_EN = LAMP_OFF;
BYTE gbIndicatorCMD_EN_Cur = LAMP_OFF;
BYTE gbIndicatorCMD_EX = LAMP_OFF;
BYTE gbIndicatorCMD_EX_Cur = LAMP_OFF;

// Status Display, Direction Lamp
BYTE gbLampCMD_EN = DIR_OFF;
BYTE gbLampCMD_EN_Cur = DIR_OFF;
BYTE gbLampCMD_EX = DIR_OFF;
BYTE gbLampCMD_EX_Cur = DIR_OFF;

// Overhead Display, Direction Lamp
BYTE gbOverehadCMD_EN = DIR_OFF;
BYTE gbOverehadCMD_EN_Cur = DIR_OFF;
BYTE gbOverehadCMD_EX = DIR_OFF;
BYTE gbOverehadCMD_EX_Cur = DIR_OFF;

BYTE gbPushFlag = FLG_OFF;
BYTE gbPowerFailFlag = FLG_OFF;
BYTE gbIndicatorFlag = FLG_OFF;
BYTE gbEmergencyFlag = FLG_OFF;
BYTE gbAuthDirection = FROM_NONE;
BYTE gbPrevEmgSignal = OFF;
BYTE gbPrevUPSStatus = 0;

bool gfModeChanged = FALSE; // mode change state
bool gfisAuthTimeout = FALSE;

DWORD gdwBuzzerTimeout;
DWORD gdwTimeoutIndicator;
DWORD gdwTimeoutSafety;
DWORD gdwTimeoutLuggage;
DWORD gdwTimeoutBlink;
DWORD gdwTimeoutAuthorize;
DWORD gdwTimeoutJumping;
DWORD gdwTimeoutUnauthorize;

TTIMER timerAuthorize;
TTIMER timerEmergency;
TTIMER timerPassageClear;
TTIMER timerTailgatingDetection;
TTIMER timerPowerFailureCheck;
TTIMER timerPowerRecoveryCheck;
TTIMER timerIllegalEnterEN;
TTIMER timerIllegalEnterEX;
TTIMER timerBlink;
TTIMER timerBarrierCheck;
TTIMER timerPush;
TTIMER timerIndicator;
TTIMER timerLuggageWait;
TTIMER timerSafety;
TTIMER timerJumping;
TTIMER timerTailgating;
TTIMER timerBarrierStop;
TTIMER timerInverseEnter;
TTIMER timerReset;
TTIMER timerCloseWait;
TTIMER timerStopHolding;
TTIMER timerTgFilter;
TTIMER timerBuzzer;
TTIMER timerBarrierCmdWait;

extern T_PASS_SEN_SWING psenNewSwing;
/* Setting Functions ------------------------------------------------------*/
void SetDefaultOpMode(void)
{
    gCurGCUOpMode.bServiceMode_EN = NO_SERVICE;
    gCurGCUOpMode.bServiceMode_EX = NO_SERVICE;
    gCurGCUOpMode.bEmergencyMaint = MODE_NONE;
    gCurGCUOpMode.bFlapDoorMode = FD_MODE_OPEN;

    memcpy(&gNewGCUOpMode, &gCurGCUOpMode, sizeof(T_GCU_OP_MODE));
}

void SetDefaultParameter(void)
{
    gGCUParameter.bAuthTimeOut = DEFAULT_AUTH_TIMEOUT;
    gGCUParameter.bEMGTimeout = DEFAULT_EMG_TIMEOUT;
    gGCUParameter.bSensorBlockTimeout = DEFAULT_SENSOR_BLOCK_TIMEOUT;
    gGCUParameter.bAlarmTimeout = DEFAULT_ALARM_TIMEOUT;
    gGCUParameter.bIllegalEntryTimeout = DEFAULT_ILLEGAL_ENTRY_TIMEOUT;
    gGCUParameter.bGateType = STANDARD;

    gdwTimeoutSafety = SAFETY_TIMEOUT_STD;
    gdwTimeoutLuggage = LUGGAGE_LIMIT_STD;
}

void SetDefaultStatus(void)
{
    gGCUStatus.bAuthCount_EN = 0;
    gGCUStatus.bAuthCount_EX = 0;
    gGCUStatus.PassageAlarm.bAlarm = 0;
    gGCUStatus.ModuleAlarm.bAlarm = 0;
    gGCUStatus.bMaintenanceDoorSw = (RD_SWITCH & MASK_SDOOR); // door mask 0xff
    gGCUStatus.bUPSStatus = 0x06;
    gGCUStatus.bDoorForcedOpen = OFF;
}

void ApplyStatusMode(void)
{
    gGCUStatus.ModeStatus.b.nServiceMode_EN = gCurGCUOpMode.bServiceMode_EN;
    gGCUStatus.ModeStatus.b.nServiceMode_EX = gCurGCUOpMode.bServiceMode_EX;
    gGCUStatus.ModeStatus.b.nEmergencyMode = (gCurGCUOpMode.bEmergencyMaint & MASK_MODE_EMERGENCY);
    gGCUStatus.ModeStatus.b.nMaintenanceMode = (gCurGCUOpMode.bEmergencyMaint << 7) & 1;
    gGCUStatus.ModeStatus.b.nFlapDoorMode = gCurGCUOpMode.bFlapDoorMode;
}

void SetNewOpMode(T_GCU_OP_MODE *pNewMode)
{
    printf(" [SetNewOpMode] Req mode EX:%02X/EN:%02X/EMG:%02X\n", pNewMode->bServiceMode_EX, pNewMode->bServiceMode_EN, pNewMode->bEmergencyMaint);

    if (pNewMode->bServiceMode_EN > FREE_SERVICE)
        pNewMode->bServiceMode_EN = gCurGCUOpMode.bServiceMode_EN;

    if (pNewMode->bServiceMode_EX > FREE_SERVICE)
        pNewMode->bServiceMode_EX = gCurGCUOpMode.bServiceMode_EX;

    if (pNewMode->bFlapDoorMode > FD_MODE_OPEN)
        pNewMode->bFlapDoorMode = gCurGCUOpMode.bFlapDoorMode;

    memcpy(&gNewGCUOpMode, pNewMode, sizeof(T_GCU_OP_MODE));

    if (gNewGCUOpMode.bEmergencyMaint)
    {
        // If emergency or maint mode, apply new mode immediately.
        memcpy(&gCurGCUOpMode, &gNewGCUOpMode, sizeof(T_GCU_OP_MODE));
        ApplyStatusMode();
        InitPassageMode();
        gfModeChanged = FALSE;
    }
    else
    {
        // If in service mode, decide to apply new mode by passenger check logic (refer to CheckModeChange()).
        gfModeChanged = TRUE;
    }
}

void CheckModeChange(void)
{
    if (gfModeChanged == TRUE && (gGCUStatus.bAuthCount_EN == 0) && (gGCUStatus.bAuthCount_EX == 0))
    {
        printf(" [CheckModeChange] Old mode EX:%02X/EN:%02X \n", gCurGCUOpMode.bServiceMode_EX, gCurGCUOpMode.bServiceMode_EN);
        printf(" [CheckModeChange] New mode EX:%02X/EN:%02X \n", gNewGCUOpMode.bServiceMode_EX, gNewGCUOpMode.bServiceMode_EN);
        gfModeChanged = FALSE;
        memcpy(&gCurGCUOpMode, &gNewGCUOpMode, sizeof(T_GCU_OP_MODE));
        ApplyStatusMode();
        InitPassageMode();
    }
}

void GetCurrentOpMode(T_GCU_OP_MODE *pCurMode)
{
    memcpy(pCurMode, &gCurGCUOpMode, sizeof(T_GCU_OP_MODE));
}

void SetGCUParameter(T_GCU_PARAMETER *pNewParameter, int nLen)
{
    gdwTimeoutSafety = SAFETY_TIMEOUT_STD;
    gdwTimeoutLuggage = LUGGAGE_LIMIT_STD;

    printf(" SetGCUParameter = %d/%d/%d/%d/%d \n", pNewParameter->bAlarmTimeout, pNewParameter->bAuthTimeOut, pNewParameter->bEMGTimeout, pNewParameter->bIllegalEntryTimeout);

    memcpy(&gGCUParameter, pNewParameter, sizeof(T_GCU_PARAMETER));
}

void GetGCUParameter(T_GCU_PARAMETER *pCurParameter)
{
    memcpy(pCurParameter, &gGCUParameter, sizeof(T_GCU_PARAMETER));
}

void GetGCUStatus(T_GCU_STATUS_RES *pCurStatus)
{
    T_MODE_STATUS ModeStatus;
    DWORD dwSpareSenserValue = 0;

    // mode status
    ModeStatus.b.nServiceMode_EN = (int)gCurGCUOpMode.bServiceMode_EN;
    ModeStatus.b.nServiceMode_EX = (int)gCurGCUOpMode.bServiceMode_EX;
    ModeStatus.b.nEmergencyMode = (int)(gCurGCUOpMode.bEmergencyMaint & MASK_MODE_EMERGENCY);

    if (gCurGCUOpMode.bEmergencyMaint & MAINTENANCE_MODE)
        ModeStatus.b.nMaintenanceMode = ON;
    else
        ModeStatus.b.nMaintenanceMode = OFF;

    ModeStatus.b.nFlapDoorMode = (int)gCurGCUOpMode.bFlapDoorMode;

    // passage alarm
    pCurStatus->bModeStatus = ModeStatus.bMode;
    pCurStatus->bAuthCount_EN = gGCUStatus.bAuthCount_EN;
    pCurStatus->bAuthCount_EX = gGCUStatus.bAuthCount_EX;
    pCurStatus->bPassageAlarm = gGCUStatus.PassageAlarm.bAlarm;
    pCurStatus->bModuleAlarm = gGCUStatus.ModuleAlarm.bAlarm;
    pCurStatus->bBarrierSw = gGCUStatus.bBarrierSw;

    pCurStatus->bMaintenanceDoorSw = (RD_SWITCH & MASK_SDOOR);
    pCurStatus->bUPSStatus = gGCUStatus.bUPSStatus;
    pCurStatus->bSafetySen = gGCUStatus.bSafetyDetection;
    pCurStatus->bSCADA = gGCUStatus.bSCADA;
    pCurStatus->bDoorForcedOpen = gGCUStatus.bDoorForcedOpen;

    // Passenger sensor 3 - BART는 Passenger sensor 17, 18는 Spare로 GPIO 별도 할당(회로상엔 9, 19로 표기)
    dwSpareSenserValue = (HAL_GPIO_ReadPin(SP_SEN_GPIO_Port, SP_SEN1_Pin)) ? 0x01 : 0x00;
    dwSpareSenserValue |= (HAL_GPIO_ReadPin(SP_SEN_GPIO_Port, SP_SEN2_Pin)) ? 0x02 : 0x00;

    pCurStatus->bPassengerSensor1 = RD_PASSSEN1;
    pCurStatus->bPassengerSensor2 = RD_PASSSEN2;
    pCurStatus->bPassengerSensor3 = dwSpareSenserValue;

    pCurStatus->bIllegalPass = gGCUStatus.IllegalPass.bIllegalPass;
    pCurStatus->bDipSwitchValue = ReadDipSwitch();
    pCurStatus->bPeopleCount = gGCUStatus.bPeopleCount;
}

/* Auth Functions ------------------------------------------------------*/
void GetAuthCount(T_GCU_AUTH_COUNT *pAuthCount)
{
    pAuthCount->bAuthCount_EN = gGCUStatus.bAuthCount_EN;
    pAuthCount->bAuthCount_EX = gGCUStatus.bAuthCount_EX;
}

void IncreaseAuthCount(BYTE bDir)
{
    printf(" [IncreaseAuthCount] Dir:%d \n", bDir);

    if (bDir == FROM_ENTRY)
    {
        gGCUStatus.bAuthCount_EN = (gGCUStatus.bAuthCount_EN < AUTHORIZE_MAX_COUNT) ? gGCUStatus.bAuthCount_EN + 1 : AUTHORIZE_MAX_COUNT;
        gGCUStatus.IllegalPass.b.nIllegal_EN = FLG_OFF;
    }
    else if (bDir == FROM_EXIT)
    {
        gGCUStatus.bAuthCount_EX = (gGCUStatus.bAuthCount_EX < AUTHORIZE_MAX_COUNT) ? gGCUStatus.bAuthCount_EX + 1 : AUTHORIZE_MAX_COUNT;
        gGCUStatus.IllegalPass.b.nIllegal_EX = FLG_OFF;
    }

    if (gbPrevDir != bDir)
    {
        ClearPassageAlarm();
        gbPrevDir = bDir;
    }

    ControlBarrier(bDir);
    ResetTimer(&timerLuggageWait);
    ResetTimer(&timerSafety);
    ControlBuzzer(BUZZER_OFF, 0);
    SetAuthTimer();

    printf(" [IncreaseAuthCount] auth cnt EN:%d, EX:%d  \n", gGCUStatus.bAuthCount_EN, gGCUStatus.bAuthCount_EX);
}

void ClearAuthCount(void)
{
    gGCUStatus.bAuthCount_EN = 0;
    gGCUStatus.bAuthCount_EX = 0;
    gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
    gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
    ResetTimer(&timerAuthorize);

    if (!gGCUStatus.PassageAlarm.b.nFromEN && !gGCUStatus.PassageAlarm.b.nFromEX)
        InitPassageMode();
}

void ControlBarrier(BYTE bFDoorControl)
{
    gbBarrierCmd = bFDoorControl;

    if (gbBarrierCmd == BARRIER_OPEN_FOR_EN || gbBarrierCmd == BARRIER_OPEN_FOR_EX)
    {
        ResetTimer(&timerSafety);
        OpenBarrierForSwing(gbBarrierCmd);
    }
    else if (gbBarrierCmd == BARRIER_CLOSE)
    {
        if (gfTestMode == TRUE)
        {
            CloseBarrierForSwing();
        }
        else
        {
            if (!timerSafety.fStart)
            {
                SetTimer(&timerSafety);
            }
        }
    }
}

void ControlBuzzer(BYTE bBuzzerControl, BYTE bDuration)
{
    ResetTimer(&timerBuzzer);

    if (bBuzzerControl == BUZZER_OFF || bDuration == 0)
    {
        BuzzerOff();
    }
    else
    {
        BuzzerOn();
        gdwBuzzerTimeout = bDuration * TICK_COUNT_1SEC;
        SetTimer(&timerBuzzer);
    }
}

void ControlDirectionLED(BYTE bLEDControl_EN, BYTE bLEDControl_EX)
{
    if (gbLampCMD_EN != bLEDControl_EN)
        gbLampCMD_EN = bLEDControl_EN;
    if (gbLampCMD_EX != bLEDControl_EX)
        gbLampCMD_EX = bLEDControl_EX;

    ControlStatusLED_En(gbLampCMD_EN);
    ControlStatusLED_Ex(gbLampCMD_EX);
    ControlOverHeadDisplay_En(gbLampCMD_EN);
    ControlOverHeadDisplay_Ex(gbLampCMD_EX);
}

void ControlStatusLED(BYTE bLEDControl_EN, BYTE bLEDControl_EX)
{
    ControlStatusLED_En(bLEDControl_EN);
    ControlStatusLED_Ex(bLEDControl_EX);
}

void ControlOverheadDisplay(BYTE bLEDControl_EN, BYTE bLEDControl_EX)
{
    ControlOverHeadDisplay_En(bLEDControl_EN);
    ControlOverHeadDisplay_Ex(bLEDControl_EX);
}

void ControlIndicator(BYTE bLEDControl_EN, BYTE bLEDControl_EX, BYTE bDuration, bool isOneTime)
{
    if (isOneTime == FALSE)
    {
        if (gbIndicatorCMD_EN != bLEDControl_EN)
            gbIndicatorCMD_EN = bLEDControl_EN;
        if (gbIndicatorCMD_EX != bLEDControl_EX)
            gbIndicatorCMD_EX = bLEDControl_EX;
    }

    if (gCurGCUOpMode.bServiceMode_EN != NO_SERVICE || gCurGCUOpMode.bEmergencyMaint == EMERGENCY_MODE || gCurGCUOpMode.bEmergencyMaint == FREE_MODE)
    {
        ControlIndicatorLight_En(bLEDControl_EN);
    }
    else
    {
        ControlIndicatorLight_En(LAMP_RED_ON);
    }

    if (gCurGCUOpMode.bServiceMode_EX != NO_SERVICE || gCurGCUOpMode.bEmergencyMaint == EMERGENCY_MODE || gCurGCUOpMode.bEmergencyMaint == FREE_MODE)
    {
        ControlIndicatorLight_Ex(bLEDControl_EX);
    }
    else
    {
        ControlIndicatorLight_Ex(LAMP_RED_ON);
    }

    if (bDuration && (bLEDControl_EN || bLEDControl_EX))
    {
        gdwTimeoutIndicator = bDuration * TICK_COUNT_1SEC;
        SetTimer(&timerIndicator);
    }
    else
        ResetTimer(&timerIndicator);
}

void ControlIndicator4Test(BYTE bLEDControl_EN, BYTE bLEDControl_EX)
{
    ControlIndicatorLight_En(bLEDControl_EN);
    ControlIndicatorLight_Ex(bLEDControl_EX);
}

void ControlIndicatorBlink(BYTE isSet)
{
    if (FLG_SET == isSet)
    {
        gbIndicatorBlink = FLG_SET;
        gbIndicatorFlag = FLG_OFF;
        gdwTimeoutBlink = 5 * TICK_COUNT_100MS;
        SetTimer(&timerBlink);
    }
    else
    {
        gbIndicatorBlink = FLG_OFF;
        ResetTimer(&timerBlink);

        gbIndicatorFlag = FLG_OFF;
        ControlIndicator(LAMP_OFF, LAMP_OFF, 0, FALSE);
    }
}

void CheckUPSStatus(void)
{
    BYTE bUPSStatus = 0;

    bUPSStatus = (HAL_GPIO_ReadPin(nUPS_GPIO_Port, nUPS_CONN_Pin)) ? 0x01 : 0x00;      // Connection Failure, High active
    bUPSStatus |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port, nUPS_PWR_FAIL_Pin)) ? 0x02 : 0x00; // Power Failure, Low active
    bUPSStatus |= (HAL_GPIO_ReadPin(nUPS_GPIO_Port, nUPS_LOW_BAT_Pin)) ? 0x04 : 0x00;  // Low battery, Low active

    if (bUPSStatus != gbPrevUPSStatus)
    {
        switch (bUPSStatus)
        {
        case 0x01:
            gGCUStatus.bUPSStatus = bUPSStatus;
            printf(" [CheckUPSStatus] UPS Comm failure  \n");
            break;
        case 0x00:
        case 0x04:
            if (gbPowerFailFlag == FLG_OFF)
            {
                if (timerPowerFailureCheck.fStart)
                {
                    if (IsTimeout(&timerPowerFailureCheck, DEFAULT_POWER_FAIL_TIMEOUT * TICK_COUNT_1SEC))
                    {
                        ResetTimer(&timerPowerFailureCheck);
                        gbPowerFailFlag = FLG_SET;
                        ControlBarrier(BARRIER_OPEN_FOR_EX);
                        gGCUStatus.bUPSStatus = bUPSStatus;
                        ControlDirectionLED(DIR_RED, DIR_RED);
                        printf(" [CheckUPSStatus] Power failure  \n");
                    }
                }
                else
                {
                    SetTimer(&timerPowerFailureCheck);
                    ResetTimer(&timerPowerRecoveryCheck);
                }
            }
            else
            {
                if (timerPowerRecoveryCheck.fStart)
                {
                    ResetTimer(&timerPowerRecoveryCheck);
                }
            }
            break;
        case 0x06:
            if (gbPowerFailFlag == FLG_SET)
            {
                if (timerPowerRecoveryCheck.fStart)
                {
                    if (IsTimeout(&timerPowerRecoveryCheck, DEFAULT_POWER_FAIL_TIMEOUT * TICK_COUNT_1SEC))
                    {
                        if ((gCurGCUOpMode.bServiceMode_EN == NO_SERVICE) ||
                            (gGCUStatus.bAuthCount_EX && (gCurGCUOpMode.bServiceMode_EN == IN_SERVICE)))
                            gbLampCMD_EN = DIR_RED;
                        else
                            gbLampCMD_EN = DIR_GREEN;

                        if ((gCurGCUOpMode.bServiceMode_EX == NO_SERVICE) ||
                            (gGCUStatus.bAuthCount_EN && (gCurGCUOpMode.bServiceMode_EX == IN_SERVICE)))
                            gbLampCMD_EX = DIR_RED;
                        else
                            gbLampCMD_EX = DIR_GREEN;

                        gbPowerFailFlag = FLG_OFF;
                        gGCUStatus.bUPSStatus = bUPSStatus;
                        ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
                        printf(" [CheckUPSStatus] Power recovered  \n");
                    }
                }
                else
                {
                    SetTimer(&timerPowerRecoveryCheck);
                    ResetTimer(&timerPowerFailureCheck);
                }
            }
            else
            {
                gGCUStatus.bUPSStatus = bUPSStatus;

                if (timerPowerFailureCheck.fStart)
                {
                    ResetTimer(&timerPowerFailureCheck);
                }
            }

            break;
        }
    }

    gbPrevUPSStatus = bUPSStatus;
}

void CheckEmergencySignal(void)
{
    BYTE bNewEmergencySignal = OFF;
    BYTE bDipSwitch4 = ReadDipSwitch() & MASK_EMG_SIGNAL;
    T_GCU_OP_MODE newMode;
    int nTriggerCount;

    if (bDipSwitch4)
        bNewEmergencySignal = OFF;
    else
    {
        bNewEmergencySignal = (BYTE)IsEMGSignalOn();
    }

    // TODO: EMG signal must be checked with Prod board by Joseph 20231002
    // Oakland Board - Low active (EMG)
    if (bNewEmergencySignal)
        gGCUStatus.ModeStatus.b.nEmergencyMode = ON;
    else
        gGCUStatus.ModeStatus.b.nEmergencyMode = OFF;

    // Prod Board - High active (EMG) - Should check whether the board itself convert signal.
    // if (!bNewEmergencySignal)
    //     gGCUStatus.ModeStatus.b.nEmergencyMode = ON;
    // else
    //     gGCUStatus.ModeStatus.b.nEmergencyMode = OFF;

    if (gbPrevEmgSignal != bNewEmergencySignal)
    {
        gnSignalCount = 1;
        gbPrevEmgSignal = bNewEmergencySignal;
        printf(" [CheckEmergencySignal] bNewEmergencySignal: %d \n", bNewEmergencySignal);
    }
    else
    {
        if (gnSignalCount)
        {
            nTriggerCount = gGCUParameter.bEMGTimeout * TICK_COUNT_1SEC;

            // If nTriggerCount is zero (bEMGTimeout is zero), EMG is activated once receiving a FAI signal.
            if (!nTriggerCount)
            {
                nTriggerCount = 1;
            }

            if (gnSignalCount++ == nTriggerCount)
            {
                gnSignalCount = 0;

                if (gGCUStatus.ModeStatus.b.nEmergencyMode == ON)
                {
                    if (gGCUStatus.ModuleAlarm.b.nEMGSignal == OFF)
                    {
                        printf(" [CheckEmergencySignal] EMG on!  \n");
                        gGCUStatus.ModuleAlarm.b.nEMGSignal = ON;
                        newMode.bServiceMode_EN = NO_SERVICE;
                        newMode.bServiceMode_EX = NO_SERVICE;
                        newMode.bEmergencyMaint = gCurGCUOpMode.bEmergencyMaint | EMERGENCY_MODE;
                        newMode.bFlapDoorMode = FD_MODE_OPEN;
                        SetNewOpMode(&newMode);
                    }
                }
                else
                {
                    if (gGCUStatus.ModuleAlarm.b.nEMGSignal == ON)
                    {
                        printf(" [CheckEmergencySignal] EMG off!  \n");
                        gGCUStatus.ModuleAlarm.b.nEMGSignal = OFF;
                        newMode.bServiceMode_EN = gCurGCUOpMode.bServiceMode_EN;
                        newMode.bServiceMode_EX = gCurGCUOpMode.bServiceMode_EN;
                        newMode.bEmergencyMaint = gCurGCUOpMode.bEmergencyMaint & (BYTE)(~EMERGENCY_MODE);
                        if (newMode.bEmergencyMaint & MAINTENANCE_MODE)
                            newMode.bFlapDoorMode = FD_MODE_OPEN;
                        else
                            newMode.bFlapDoorMode = FD_MODE_CLOSE;
                        SetNewOpMode(&newMode);
                    }
                }
            }
        }
    }
}

void CheckBuzzerTimer(void)
{
    if (timerBuzzer.fStart)
    {
        if (IsTimeout(&timerBuzzer, gdwBuzzerTimeout))
        {
            ResetTimer(&timerBuzzer);
            BuzzerOff();
        }
    }
}

void CheckIndicatorTimer(void)
{
    if (timerIndicator.fStart)
    {
        if (IsTimeout(&timerIndicator, gdwTimeoutIndicator))
        {
            printf(" [CheckIndicatorTimer] Indicator time out!!  \n");
            ResetTimer(&timerIndicator);
            ControlIndicator(LAMP_OFF, LAMP_OFF, 0, FALSE);
        }
    }
}

void CheckPushTimer(BYTE command)
{
    if (timerPush.fStart)
    {
        if (IsTimeout(&timerPush, 5 * TICK_COUNT_1SEC))
        {
            gbPushFlag = 0;
            ResetTimer(&timerPush);
            ControlIndicator(LAMP_OFF, LAMP_OFF, 0, FALSE);
            ControlBarrier(BARRIER_CLOSE);
        }
    }
}

void CheckPassSenError(bool isSwing)
{
    int i;
    BYTE bSensorData = RD_PASSSEN1;

    for (i = 0; i < 16; i++)
    {
        if (i == 8)
            bSensorData = RD_PASSSEN2;

        if (bSensorData & (1 << (i % 8)))
        {
            if (mdwTickPSensorBlock[i] == (gGCUParameter.bSensorBlockTimeout * TICK_COUNT_1SEC * 10))
            {
                gdwPSensorError |= (1 << i);

                if (gGCUStatus.ModuleAlarm.b.nPSensor == OFF)
                {
                    if (gCurGCUOpMode.bEmergencyMaint == MODE_NONE)
                        gGCUStatus.ModuleAlarm.b.nPSensor = ON;

                    printf(" [CheckPassSenError] Sensor Error Detected!!  \n");
                }

                if (i >= 0 && i < 8)
                    psenError.B.bSen1 |= (1 << i);
                else if (i >= 8 && i < 16)
                    psenError.B.bSen2 |= (1 << (i - 8));

                if (gCurGCUOpMode.bEmergencyMaint == MODE_NONE)
                {
                    if (psenError.swing.safety_EN || psenError.swing.safety_EX)
                        gGCUStatus.ModuleAlarm.b.nSafetyErr = ON;
                }

                mdwTickPSensorBlock[i]++;
            }
            else if (mdwTickPSensorBlock[i] < (gGCUParameter.bSensorBlockTimeout * TICK_COUNT_1SEC * 10))
            {
                mdwTickPSensorBlock[i]++;
            }
        }
        else
        {
            mdwTickPSensorBlock[i] = 0;
            gdwPSensorError &= ~(1 << i);

            if (i >= 0 && i < 8)
                psenError.B.bSen1 &= ~(1 << i);
            else if (i >= 8 && i < 16)
                psenError.B.bSen2 &= ~(1 << (i - 8));
        }
    }

    if (!gdwPSensorError)
    {
        if (gGCUStatus.ModuleAlarm.b.nPSensor == ON)
            gGCUStatus.ModuleAlarm.b.nPSensor = OFF;
        if (gGCUStatus.ModuleAlarm.b.nSafetyErr == ON)
            gGCUStatus.ModuleAlarm.b.nSafetyErr = OFF;
    }
    else
    {
        if (gGCUStatus.ModuleAlarm.b.nSafetyErr == ON)
        {
            if (!psenError.swing.safety_EN && !psenError.swing.safety_EX)
                gGCUStatus.ModuleAlarm.b.nSafetyErr = OFF;
        }
    }
}
void InhibitPass(int nDir)
{
    printf(" [InhibitPass] Dir:%d  \n", nDir);
    ControlBuzzer(BUZZER_ON, gGCUParameter.bAlarmTimeout);
    ControlDirectionLED(DIR_RED, DIR_RED);

    if (nDir == FROM_ENTRY)
    {
        ControlIndicator(LAMP_RED_ON, gbIndicatorCMD_EX, gGCUParameter.bAlarmTimeout, TRUE);
    }
    else if (nDir == FROM_EXIT)
    {
        ControlIndicator(gbIndicatorCMD_EN, LAMP_RED_ON, gGCUParameter.bAlarmTimeout, TRUE);
    }
}

void ClearPassageAlarm()
{
    ResetTimer(&timerIllegalEnterEN);
    ResetTimer(&timerIllegalEnterEX);
    ResetTimer(&timerLuggageWait);

    // Passage clear timeout is reached, clear passage
    if (gGCUStatus.PassageAlarm.b.nFromEN)
        gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_NONE;

    if (gGCUStatus.PassageAlarm.b.nFromEX)
        gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_NONE;

    if (gGCUStatus.PassageAlarm.b.nPassOverFromEN || gGCUStatus.PassageAlarm.b.nPassOverFromEX)
    {
        gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
        gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
    }

    if (!psenNewSwing.side.entry && (0 == gGCUStatus.bAuthCount_EN))
    {
        gisEntryPassenger = FALSE;
    }

    if (!psenNewSwing.side.exit && (0 == gGCUStatus.bAuthCount_EX))
    {
        gisExitPassenger = FALSE;
    }

    gGCUStatus.IllegalPass.bIllegalPass = FLG_OFF;

    ClearIllegalPass();
}

void ClearIllegalPass(void)
{
    if (gGCUStatus.PassageAlarm.b.nFromEN)
    {
        if (!psenNewSwing.side.entry)
        {
            gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_NONE;

            if (gGCUStatus.IllegalPass.b.nCounter_EN == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nCounter_EN = FLG_OFF;
                printf(" [EN] Clear Counter Enter  \n");
            }

            if (gGCUStatus.IllegalPass.b.nIllegal_EN == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nIllegal_EN = FLG_OFF;
                printf(" [EN] Clear Illegal Enter  \n");
            }
            ResetTimer(&timerIllegalEnterEN);

            if (timerJumping.fStart == FALSE && timerTailgating.fStart == FALSE)
            {
                InitPassageMode();
                printf(" [EN] ClearIllegalPass - InitPassageMode  \n");
            }
        }
    }

    if (gGCUStatus.PassageAlarm.b.nFromEX)
    {
        if (!psenNewSwing.side.exit)
        {
            gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_NONE;

            if (gGCUStatus.IllegalPass.b.nCounter_EX == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nCounter_EX = FLG_OFF;
                printf(" [EX] Clear Counter Enter  \n");
            }

            if (gGCUStatus.IllegalPass.b.nIllegal_EX == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nIllegal_EX = FLG_OFF;
                printf(" [EX] Clear Illegal Enter  \n");
            }
            ResetTimer(&timerIllegalEnterEX);

            if (timerJumping.fStart == FALSE && timerTailgating.fStart == FALSE)
            {
                InitPassageMode();
                printf(" [EX] ClearIllegalPass - InitPassageMode  \n");
            }
        }
    }
}

void CheckAuthTimeout(void)
{
    if (timerAuthorize.fStart)
    {
        if (IsTimeout(&timerAuthorize, (gdwTimeoutAuthorize * TICK_COUNT_1SEC)))
        {
            printf(" [CheckAuthTimeout] ElapsedTime:%d  \n", gdwTimeoutAuthorize);
            ResetTimer(&timerAuthorize);
            gGCUStatus.bAuthCount_EN = 0;
            gGCUStatus.bAuthCount_EX = 0;
            gfisAuthTimeout = TRUE;
            ControlIndicator(LAMP_OFF, LAMP_OFF, 0, FALSE);
            if (gGCUStatus.PassageAlarm.bAlarm != 0)
            {
                ClearIllegalPass();
                gGCUStatus.PassageAlarm.bAlarm = 0;
            }

            InitPassageMode();
        }
    }
}

void CheckIllegalAlarmTimer(void)
{
    if (timerJumping.fStart)
    {
        if (IsTimeout(&timerJumping, gdwTimeoutJumping))
        {
            gGCUStatus.IllegalPass.b.nJumping_EN = FLG_OFF;
            gGCUStatus.IllegalPass.b.nJumping_EX = FLG_OFF;
            gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_NONE;
            gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_NONE;
            ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
            ControlIndicatorBlink(FLG_OFF);
            ControlBuzzer(BUZZER_OFF, 0);
            ResetTimer(&timerJumping);
        }
    }

    if (timerTailgating.fStart)
    {
        if (IsTimeout(&timerTailgating, DEFAULT_TAILGATING_TIMEOUT * TICK_COUNT_1SEC))
        {
            gGCUStatus.IllegalPass.b.nTailgating_EN = FLG_OFF;
            gGCUStatus.IllegalPass.b.nTailgating_EX = FLG_OFF;
            gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_NONE;
            gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_NONE;
            ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
            ControlIndicatorBlink(FLG_OFF);
            ControlBuzzer(BUZZER_OFF, 0);
            ResetTimer(&timerTailgating);
        }
    }
}

void CheckBlinkTimer(void)
{
    if (timerBlink.fStart)
    {
        if (IsTimeout(&timerBlink, gdwTimeoutBlink))
        {
            if (FLG_SET == gbIndicatorBlink)
            {
                if (gbIndicatorFlag)
                {
                    gbIndicatorFlag = FLG_OFF;
                    ControlIndicator(LAMP_OFF, LAMP_OFF, 0, TRUE);
                }
                else
                {
                    gbIndicatorFlag = FLG_SET;
                    ControlIndicator(LAMP_RED_ON, LAMP_RED_ON, 0, TRUE);
                }
            }
            ResetTimer(&timerBlink);
            SetTimer(&timerBlink);
        }
    }
}

void CheckIllegalEnterTimer(BYTE dir)
{
    if (FROM_ENTRY == dir)
    {
        if (timerIllegalEnterEN.fStart)
        {
            if (IsTimeout(&timerIllegalEnterEN, gdwTimeoutUnauthorize))
            {
                ResetTimer(&timerIllegalEnterEN);
                gGCUStatus.IllegalPass.b.nIllegal_EN = FLG_SET;
                gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_ILLEGAL_ENTER;
                InhibitPass(FROM_ENTRY);
                printf(" [EN] Inhibit Pass - Illegal Enter  \n");
            }
        }
        else
        {
            if (gGCUStatus.IllegalPass.b.nIllegal_EN == FLG_OFF)
            {
                if (gGCUParameter.bIllegalEntryTimeout)
                {
                    gdwTimeoutUnauthorize = gGCUParameter.bIllegalEntryTimeout * TICK_COUNT_1SEC;
                    SetTimer(&timerIllegalEnterEN);
                }
            }
        }
    }
    else if (FROM_EXIT == dir)
    {
        if (timerIllegalEnterEX.fStart)
        {
            if (IsTimeout(&timerIllegalEnterEX, gdwTimeoutUnauthorize))
            {
                ResetTimer(&timerIllegalEnterEX);
                gGCUStatus.IllegalPass.b.nIllegal_EX = FLG_SET;
                gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_ILLEGAL_ENTER;
                InhibitPass(FROM_EXIT);
                printf(" [EX] Inhibit Pass - Illegal Enter  \n");
            }
        }
        else
        {
            if (gGCUStatus.IllegalPass.b.nIllegal_EX == FLG_OFF)
            {
                if (gGCUParameter.bIllegalEntryTimeout)
                {
                    gdwTimeoutUnauthorize = gGCUParameter.bIllegalEntryTimeout * TICK_COUNT_1SEC;
                    SetTimer(&timerIllegalEnterEX);
                }
            }
        }
    }
}

void SetAuthTimer(void)
{
    gdwTimeoutAuthorize = gGCUParameter.bAuthTimeOut;
    gfisAuthTimeout = FALSE;
    if (gdwTimeoutAuthorize > MAX_AUTH_TIMEOUT)
        gdwTimeoutAuthorize = MAX_AUTH_TIMEOUT;

    SetTimer(&timerAuthorize);
}

void SetTailgatingTimer(void)
{
    SetTimer(&timerTailgatingDetection);
    SetTimer(&timerTailgating);
}

void InitPassageMode(void)
{
    InitPassageModeForSwing();
}

/******* COPYRIGHT �� 2022 STraffic Co., Ltd.  ********END OF FILE****/
