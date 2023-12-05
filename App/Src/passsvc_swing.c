/*******************************************************************************
 *       COPYRIGHT ?? 2022 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : passsvc.c
 * Author       : Joseph Yosep Choi
 * Date Created : 01/17/2020
 * Description  : Passage function implementaion
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gcu_def.h"
#include "userlib.h"
#include "common_func.h"
#include "repository.h"
#include "passsvc_swing.h"

T_PASS_SEN_SWING psenNewSwing;
T_PASS_SEN_SWING psenOldSwing;

BYTE bEnterPattern_EN = PASS_PATN_NONE;
BYTE bEnterPattern_EX = PASS_PATN_NONE;
BYTE bCountPattern_EN = PASS_PATN_NONE;
BYTE bCountPattern_EX = PASS_PATN_NONE;

// Setting time for Timer
DWORD dwCloseWaitTime = 1;      // unit: 100 ms
DWORD dwResetTime = 30;         // unit: 100 ms
DWORD dwTgFilterTime = 2;       // unit: 100 ms
DWORD dwTgDetectionTime = 10;   // unit: 100 ms
DWORD dwInverseCloseTime = 4;   // unit: 100 ms
DWORD dwBarrierCheckTime = 5;   // unit: 100 ms
DWORD dwBarrierStopTime = 5;    // unit: 100 ms
DWORD dwBarrierCmdWaitTime = 2; // unit: 100 ms
DWORD dwStopHoldingTime = 6;    // unit: 10 sec

bool isPassReady_EN = FALSE; // pass-ready state of direction Entry
bool isPassReady_EX = FALSE; // pass-ready state of direction Exit
bool isPassOver_EN = FALSE;  // pass-over state of direction A
bool isPassOver_EX = FALSE;  // pass-over state of direction B
bool isLuggageSet_EN = FALSE;
bool isLuggageSet_EX = FALSE;
bool isBarrierStop = FALSE;
bool isTailgating = FALSE;
bool isSafetyOn = FALSE;
bool isCounterEnter = FALSE;
bool isSafetyOn4Init = FALSE;
bool isCalibration = FALSE;
bool isCalibrationStart = FALSE;
bool isSentClose = FALSE;
bool isCountReady_EN = FALSE;
bool isCountReady_EX = FALSE;
bool isBarrierClosed = FALSE;

int nCalibrationCnt = 0;
int nBarrierErrorCnt = 0;

/*******************************************************************************
 * Passenger Service Functions
 *******************************************************************************/
bool CheckBarrierClosedStatus()
{
    if (gbPowerFailFlag == FLG_OFF && gbEmergencyFlag == FLG_OFF && gfFreeMode == FALSE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool CheckZeroAuthCount()
{
    if (gGCUStatus.bAuthCount_EN == 0 && gGCUStatus.bAuthCount_EX == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool CheckCritical()
{
    if (gisEntryPassenger && psenNewSwing.b.s05)
    {
        return TRUE;
    }
    else if (gisExitPassenger && psenNewSwing.b.s13)
    {
        return TRUE;
    }

    return FALSE;
}

void UpdateSensorData()
{
    psenNewSwing.B.bSen1 = RD_PASSSEN1 & (~psenError.B.bSen1);
    psenNewSwing.B.bSen2 = RD_PASSSEN2 & (~psenError.B.bSen2);
}

void InitPassageModeForSwing(void)
{
    if (isBarrierStop == TRUE)
    {
        printf(" [InitPassageMode] Release the barrier stop \n");
        StopBarrierForSwing(FALSE);
    }

    if (gCurGCUOpMode.bEmergencyMaint && gfTestMode == FALSE)
    {
        ResetTimer(&timerReset);
        gbEmergencyFlag = FLG_SET;
        gGCUStatus.bAuthCount_EN = 0;
        gGCUStatus.bAuthCount_EX = 0;
        gbAuthDirection = FROM_NONE;
        gisEntryPassenger = FALSE;
        gisExitPassenger = FALSE;
        gnPassPattern = PASS_PATN_NONE;
        isPassOver_EN = FALSE;
        isPassOver_EX = FALSE;
        timerAuthorize.fStart = FALSE;
        timerPassageClear.fStart = FALSE;
        timerSafety.fStart = FALSE;

        printf(" [InitPassageMode] bEmergencyMaint: %02X \n", gCurGCUOpMode.bEmergencyMaint);

        Brr_SetEmergency(TRUE);

        if (gCurGCUOpMode.bEmergencyMaint & MASK_MODE_EMERGENCY)
        {
            ControlDirectionLED(DIR_RED, DIR_GREEN);
            ControlIndicatorBlink(FLG_SET);
        }
        else if (gCurGCUOpMode.bEmergencyMaint & MAINTENANCE_MODE)
        {
            ControlDirectionLED(DIR_RED, DIR_RED);
            ControlIndicatorBlink(FLG_OFF);
        }
        else if (gCurGCUOpMode.bEmergencyMaint & FREE_MODE)
        {
            ControlDirectionLED(DIR_GREEN, DIR_GREEN);
            ControlIndicatorBlink(FLG_OFF);
        }
    }
    else
    {
        if (FLG_SET == gbEmergencyFlag && gfTestMode == FALSE)
        {
            printf(" [InitPassageMode] Safety Check for mode change!! \n");
            ControlBuzzer(BUZZER_ON, 2);
            dwResetTime = 30;
            SetTimer(&timerReset);
        }
        else
        {
            if ((gCurGCUOpMode.bServiceMode_EN == NO_SERVICE) ||
                ((gCurGCUOpMode.bServiceMode_EN == FREE_SERVICE) && !psenNewSwing.side.entry))
                gGCUStatus.bAuthCount_EN = 0;

            if ((gCurGCUOpMode.bServiceMode_EX == NO_SERVICE) ||
                ((gCurGCUOpMode.bServiceMode_EX == FREE_SERVICE) && !psenNewSwing.side.exit))
                gGCUStatus.bAuthCount_EX = 0;

            if (gGCUStatus.bAuthCount_EN || gGCUStatus.bAuthCount_EX)
            {
                if (gGCUStatus.bAuthCount_EN)
                    ControlBarrier(BARRIER_OPEN_FOR_EN);
                else if (gGCUStatus.bAuthCount_EX)
                    ControlBarrier(BARRIER_OPEN_FOR_EX);
            }
            else
            {
                if (gbPowerFailFlag == FLG_SET)
                    ControlBarrier(BARRIER_OPEN_FOR_EX);
                else
                    ControlBarrier(BARRIER_CLOSE);
            }

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

            ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
            ControlIndicator(gbIndicatorCMD_EN, gbIndicatorCMD_EX, 0, FALSE);

            if (CheckZeroAuthCount())
            {
                printf(" [InitPassageMode] ResetAuthTimer!! \n");
                ResetTimer(&timerAuthorize);
            }
        }
    }
}

void StopBarrierForSwing(bool isStop)
{
    if (isBarrierStop != isStop)
    {
        isBarrierStop = isStop;

        if (isBarrierStop == TRUE)
        {
            SetTimer(&timerStopHolding);
        }
        else
        {
            ResetTimer(&timerStopHolding);
        }

        Brr_StopBarrier(isStop);
    }
}

void OpenBarrierForSwing(BYTE bDir)
{
    if (isBarrierStop == TRUE)
    {
        StopBarrierForSwing(FALSE);
    }

    if (gfTestMode == TRUE)
    {
        Brr_OpenBarrier(bDir);
    }
    else
    {
        gbAuthDirection = bDir;

        if (bDir == BARRIER_OPEN_FOR_EN)
        {
            isPassReady_EX = FALSE;
            isPassOver_EX = FALSE;
            if (!psenNewSwing.side.exit)
                Brr_OpenBarrier(BARRIER_OPEN_FOR_EN);
        }
        else if (bDir == BARRIER_OPEN_FOR_EX)
        {
            isPassReady_EN = FALSE;
            isPassOver_EN = FALSE;
            if (!psenNewSwing.side.entry)
                Brr_OpenBarrier(BARRIER_OPEN_FOR_EX);
        }
    }

    SetTimer(&timerBarrierCmdWait);
}

void CloseBarrierForSwing(void)
{
    if (gGCUStatus.ModuleAlarm.b.nSafetyErr == OFF && (gfTestMode == TRUE || CheckBarrierClosedStatus()))
    {
        if (isBarrierStop == TRUE)
        {
            StopBarrierForSwing(FALSE);
        }

        Brr_CloseBarrier();
        SetTimer(&timerBarrierCmdWait);
    }
}

void CheckBarrierOperation(void)
{
    if (timerBarrierCmdWait.fStart)
    {
        if (IsTimeout(&timerBarrierCmdWait, dwBarrierCmdWaitTime * TICK_COUNT_100MS))
        {
            ResetTimer(&timerBarrierCmdWait);
        }
    }
    else if (gfTestMode == FALSE && timerBarrierCheck.fStart)
    {
        if (IsTimeout(&timerBarrierCheck, dwBarrierCheckTime * TICK_COUNT_100MS))
        {
            ResetTimer(&timerBarrierCheck);

            if (gMainBarrierStatus.param.actStatus == 0 && gSubBarrierStatus.param.actStatus == 0)
            {
                if (gbBarrierCmd == BARRIER_OPEN_FOR_EN || gbBarrierCmd == BARRIER_OPEN_FOR_EX || gbEmergencyFlag == FLG_SET)
                {
                    if (gMainBarrierStatus.param.posStatus != SDOOR_OPEN || gSubBarrierStatus.param.posStatus != SDOOR_OPEN)
                    {
                        if (isCounterEnter == FALSE)
                        {
                            ControlBarrier(gbBarrierCmd);
                        }
                    }
                    else if (isSafetyOn4Init == FALSE)
                    {
                        nBarrierErrorCnt = 0;
                        gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_NONE;
                        gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_NONE;
                        gGCUStatus.bBarrierSw = 0x11;

                        if (CheckBarrierClosedStatus() && CheckZeroAuthCount())
                        {
                            printf(" [CheckBarrier] Current Status is open \n");
                            ControlBarrier(BARRIER_CLOSE);
                        }
                    }
                }
                else if (gbBarrierCmd == BARRIER_CLOSE)
                {
                    if (gMainBarrierStatus.param.posStatus != SDOOR_CLOSE || gSubBarrierStatus.param.posStatus != SDOOR_CLOSE)
                    {
                        if (CheckBarrierClosedStatus())
                        {
                            printf(" [CheckBarrier] pos:%02X/%02X \n", gMainBarrierStatus.param.posStatus, gSubBarrierStatus.param.posStatus);

                            if (isCalibrationStart == FALSE && (isSafetyOn == FALSE || gfisAuthTimeout == TRUE))
                            {
                                nBarrierErrorCnt++;

                                if (nBarrierErrorCnt == 8) // After 4 sec, Barrier fault is generated.
                                {
                                    if (gGCUStatus.ModuleAlarm.b.nFDoor1 == FDOOR_ALARM_NONE && gGCUStatus.ModuleAlarm.b.nFDoor2 == FDOOR_ALARM_NONE)
                                    {
                                        ControlBuzzer(BUZZER_ON, gGCUParameter.bAlarmTimeout);
                                    }

                                    if (gMainBarrierStatus.param.posStatus != SDOOR_CLOSE)
                                        gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_CLOSE_FAULT;
                                    if (gSubBarrierStatus.param.posStatus != SDOOR_CLOSE)
                                        gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_CLOSE_FAULT;

                                    nBarrierErrorCnt = 0;
                                    StopBarrierForSwing(FALSE);
                                }
                            }
                        }
                    }
                    else if (isSafetyOn4Init == FALSE)
                    {
                        nBarrierErrorCnt = 0;
                        gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_NONE;
                        gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_NONE;
                        gGCUStatus.bBarrierSw = 0x22;

                        if (gGCUStatus.bAuthCount_EN)
                        {
                            ControlBarrier(BARRIER_OPEN_FOR_EN);
                        }
                        else if (gGCUStatus.bAuthCount_EX)
                        {
                            ControlBarrier(BARRIER_OPEN_FOR_EX);
                        }
                        else if (gbPowerFailFlag == FLG_SET)
                        {
                            ControlBarrier(BARRIER_OPEN_FOR_EX);
                        }
                        else
                        {
                            if (gGCUParameter.bGateType == WIDE)
                            {
                                // Total Locking 20230823
                                if (!psenNewSwing.dirExit.lower && !psenNewSwing.dirEntry.lower)
                                {
                                    StopBarrierForSwing(FALSE);
                                }
                                else
                                {
                                    printf(" [CheckBarrier] Total Locking - 1!!! \n");
                                    StopBarrierForSwing(TRUE);
                                }
                            }
                            else
                            {
                                // Total Locking 20230823
                                if (!psenNewSwing.dirExit.lower && !psenNewSwing.dirEntry.lower)
                                {
                                    StopBarrierForSwing(FALSE);
                                }
                                else
                                {
                                    printf(" [CheckBarrier] Total Locking - 2!!! \n");
                                    StopBarrierForSwing(TRUE);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (!timerTailgating.fStart && gGCUStatus.bAuthCount_EN == 0 && gGCUStatus.bAuthCount_EX == 0 && CheckBarrierClosedStatus())
                {
                    if (isCalibrationStart == FALSE)
                    {
                    }
                }
                else
                {
                    nBarrierErrorCnt = 0;
                }
            }

            Brr_Inq_Status();
        }
    }
    else
    {
        SetTimer(&timerBarrierCheck);
    }
}

void ClearPassHistory(void)
{
    if (gGCUStatus.PassageAlarm.b.nFromEN)
    {
        if (!psenNewSwing.side.entry)
        {
            gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_NONE;

            if (gGCUStatus.IllegalPass.b.nCounter_EN == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nCounter_EN = FLG_OFF;
                printf(" [EN] Clear Counter Enter \n");
            }

            if (gGCUStatus.IllegalPass.b.nIllegal_EN == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nIllegal_EN = FLG_OFF;
                printf(" [EN] Clear Illegal Enter \n");
            }
            ResetTimer(&timerIllegalEnterEN);
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
                printf(" [EX] Clear Counter Enter \n");
            }

            if (gGCUStatus.IllegalPass.b.nIllegal_EX == FLG_SET)
            {
                gGCUStatus.IllegalPass.b.nIllegal_EX = FLG_OFF;
                printf(" [EX] Clear Illegal Enter \n");
            }
            ResetTimer(&timerIllegalEnterEN);
        }
    }

    if (FLG_OFF == gbEmergencyFlag)
    {
        if (gCurGCUOpMode.bServiceMode_EN == NO_SERVICE || (gGCUStatus.bAuthCount_EX && gCurGCUOpMode.bServiceMode_EN == IN_SERVICE))
        {
            gbLampCMD_EN = DIR_RED;
            gbIndicatorCMD_EN = LAMP_OFF;
        }
        else
        {
            gbLampCMD_EN = DIR_GREEN;
            gbIndicatorCMD_EX = LAMP_OFF;
        }

        if (gCurGCUOpMode.bServiceMode_EX == NO_SERVICE || (gGCUStatus.bAuthCount_EN && gCurGCUOpMode.bServiceMode_EX == IN_SERVICE))
        {
            gbLampCMD_EX = DIR_RED;
            gbIndicatorCMD_EX = LAMP_OFF;
        }
        else
        {
            gbLampCMD_EX = DIR_GREEN;
            gbIndicatorCMD_EN = LAMP_OFF;
        }

        ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
        ControlIndicator(gbIndicatorCMD_EN, gbIndicatorCMD_EX, 0, FALSE);
    }
}

void CheckEntranceSwing(void)
{
    if (gisEntryPassenger)
    {
        if (gGCUStatus.bAuthCount_EX)
        {
            if (!psenNewSwing.section.enter_EN && !psenNewSwing.section.lower_EN)
            {
                ResetTimer(&timerIllegalEnterEN);
                // isPassReady_EN = FALSE;
                bEnterPattern_EN = PASS_PATN_NONE;
                gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
                isCounterEnter = FALSE;
                ClearPassHistory();
                // ControlBarrier(BARRIER_OPEN_FOR_EX);
                printf(" [EN] Enter Clear - Inverse entering\n");
            }
        }
        else
        {
            if (!psenNewSwing.side.entry)
            {
                ResetTimer(&timerIllegalEnterEN);
                // isPassReady_EN = FALSE;
                bEnterPattern_EN = PASS_PATN_NONE;
                gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
                isCounterEnter = FALSE;
                ClearPassHistory();
                printf(" [EN] Enter Clear - Normal \n");
            }
        }
    }
    else
    {
        // TODO: Check needs of this alarm
        if (gGCUStatus.PassageAlarm.b.nPassOverFromEX != ON)
        {
            if (psenNewSwing.dirEntry.enter)
            {
                bEnterPattern_EN |= psenNewSwing.dirEntry.enter;

                if (bEnterPattern_EN >= PASS_PATN_ENTER)
                {
                    gisEntryPassenger = TRUE;

                    printf(" [EN] Enter - [%02X]\n", bEnterPattern_EN);
                }
            }
        }
    }

    if (gisExitPassenger)
    {
        if (gGCUStatus.bAuthCount_EN)
        {
            if (!psenNewSwing.section.enter_EX && !psenNewSwing.section.lower_EX)
            {
                ResetTimer(&timerIllegalEnterEX);
                // isPassReady_EX = FALSE;
                bEnterPattern_EX = PASS_PATN_NONE;
                gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
                isCounterEnter = FALSE;
                ClearPassHistory();
                // ControlBarrier(BARRIER_OPEN_FOR_EN);
                printf(" [EX] Enter Clear - Inverse entering \n");
            }
        }
        else
        {
            if (!psenNewSwing.side.exit)
            {
                ResetTimer(&timerIllegalEnterEX);
                // isPassReady_EX = FALSE;
                bEnterPattern_EX = PASS_PATN_NONE;
                gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
                isCounterEnter = FALSE;
                ClearPassHistory();
                printf(" [EX] Enter Clear - Normal \n");
            }
        }
    }
    else
    {
        // TODO: Check needs of this alarm
        if (gGCUStatus.PassageAlarm.b.nPassOverFromEN != ON)
        {
            if (psenNewSwing.dirExit.enter)
            {
                bEnterPattern_EX |= psenNewSwing.dirExit.enter;

                if (bEnterPattern_EX >= PASS_PATN_ENTER)
                {
                    gisExitPassenger = TRUE;

                    printf(" [EX] Enter - [%02X]\n", bEnterPattern_EX);
                }
            }
        }
    }
}

void CheckPassReadySwing(void)
{
    if (gisExitPassenger == TRUE && gGCUStatus.bAuthCount_EX)
    {
        if (isPassReady_EN == FALSE)
        {
            if (psenNewSwing.b.s12 || psenNewSwing.b.s15 || psenNewSwing.b.s16)
            {
                printf(" [EX] Pass Ready \n");
                isPassReady_EX = TRUE;
                gisExitPassenger = FALSE;
            }
        }

        // Luggage check (Stroller, Bicycle, Luggage)
        if (gGCUStatus.bAuthCount_EX && psenNewSwing.dirExit.enter && psenNewSwing.b.s13 && psenNewSwing.b.s14 && !psenNewSwing.b.s11)
        {
            isLuggageSet_EX = TRUE;
            printf(" [EX] Set Luggage Flag\n");
        }
    }

    if (gisEntryPassenger == TRUE && gGCUStatus.bAuthCount_EN)
    {
        if (isPassReady_EX == FALSE)
        {
            if (psenNewSwing.b.s04 || psenNewSwing.b.s07 || psenNewSwing.b.s08)
            {
                printf(" [EN] Pass Ready \n");
                isPassReady_EN = TRUE;
                gisEntryPassenger = FALSE;
            }
        }

        // Luggage check (Stroller, Bicycle, Luggage)
        if (gGCUStatus.bAuthCount_EN && psenNewSwing.dirEntry.enter && psenNewSwing.b.s03 && psenNewSwing.b.s04 && !psenNewSwing.b.s01)
        {
            isLuggageSet_EN = TRUE;
            printf(" [EN] Set Luggage Flag  \n");
        }
    }
}

void CheckPassingSwing(void)
{
    // To prevent the counter-enter, I utilize two boolean variable "isPassReady" and "isPassOver"
    if (isPassReady_EN)
    {
        if (isPassOver_EN)
        {
            // if (psenNewSwing.b.s10 && !psenNewSwing.b.s13)
            if (CheckPassOverSensor())
            {
                printf(" [EN] Pass over [%d]\n", gGCUStatus.bAuthCount_EN);
                isPassReady_EN = FALSE;
                isPassOver_EN = FALSE;

                if (gGCUStatus.bAuthCount_EN == 1)
                {
                    if (psenNewSwing.section.upper_EN)
                    {
                        isPassReady_EN = TRUE;
                        isPassOver_EN = TRUE;

                        if (!psenNewSwing.b.s13 && !psenNewSwing.b.s14 && !psenNewSwing.b.s15)
                        {
                            if (gGCUStatus.IllegalPass.b.nTailgating_EN == FLG_OFF && timerTgFilter.fStart == FALSE)
                            {
                                SetTimer(&timerTgFilter);
                            }
                        }
                    }
                    else
                    {
                        gGCUStatus.bAuthCount_EN--;
                        ControlBarrier(BARRIER_CLOSE);
                        // SetTimer(&timerCloseWait);
                        ResetTimer(&timerAuthorize);
                        printf(" [EN] Barrier close \n");
                    }
                }
                else if (gGCUStatus.bAuthCount_EN > 1)
                {
                    gGCUStatus.bAuthCount_EN--;
                    SetAuthTimer();
                    isPassReady_EN = TRUE;
                }
            }
        }
        else
        {
            if (!psenNewSwing.b.s13 && psenNewSwing.dirEntry.lower)
            {
                printf(" [EN] passing before \n");

                if (gGCUStatus.bAuthCount_EN || gGCUStatus.IllegalPass.b.nTailgating_EN == FLG_SET)
                {
                    isPassOver_EN = TRUE;
                    gGCUStatus.PassageAlarm.b.nPassOverFromEN = ON;
                    printf(" [EN] passing \n");
                }
            }
        }
    }

    if (isPassReady_EX)
    {
        if (isPassOver_EX)
        {
            // if (psenNewSwing.b.s02 && !psenNewSwing.b.s05)
            if (CheckPassOverSensor())
            {
                printf(" [EX] Pass over [%d]\n", gGCUStatus.bAuthCount_EX);
                isPassReady_EX = FALSE;
                isPassOver_EX = FALSE;

                if (gGCUStatus.bAuthCount_EX == 1)
                {
                    if (psenNewSwing.section.upper_EX)
                    {
                        isPassReady_EX = TRUE;
                        isPassOver_EX = TRUE;

                        if (!psenNewSwing.b.s05 && !psenNewSwing.b.s06 && !psenNewSwing.b.s07)
                        { // If not detected on the upper sensors, no tailgating
                            if (gGCUStatus.IllegalPass.b.nTailgating_EX == FLG_OFF && timerTgFilter.fStart == FALSE)
                            {
                                SetTimer(&timerTgFilter);
                            }
                        }
                    }
                    else
                    {
                        gGCUStatus.bAuthCount_EX--;
                        ControlBarrier(BARRIER_CLOSE);
                        // SetTimer(&timerCloseWait);
                        ResetTimer(&timerAuthorize);
                        printf(" [EX] Barrier close \n");
                    }
                }
                else if (gGCUStatus.bAuthCount_EX > 1)
                {
                    gGCUStatus.bAuthCount_EX--;
                    SetAuthTimer();
                    isPassReady_EX = TRUE;
                }
            }
        }
        else
        {
            if (!psenNewSwing.b.s05 && psenNewSwing.dirExit.lower)
            {
                printf(" [EX] passing before \n");

                if (gGCUStatus.bAuthCount_EX || gGCUStatus.IllegalPass.b.nTailgating_EX == FLG_SET)
                {
                    isPassOver_EX = TRUE;
                    gGCUStatus.PassageAlarm.b.nPassOverFromEX = ON;
                    printf(" [EX] passing\n");
                }
            }
        }
    }
}

bool CheckPassOverSensor(void)
{
    if (gGCUParameter.bGateType == WIDE)
    {
        if (isPassOver_EN)
        {
            if (psenNewSwing.b.s10 && !psenNewSwing.b.s13)
            {
                printf(" [EN] Check PassOver - WIDE\n");
                return  TRUE;
            }
        }
        else if (isPassOver_EX)
        {
            if (psenNewSwing.b.s02 && !psenNewSwing.b.s05)
            {
                printf(" [EX] Check PassOver - WIDE\n");
                return  TRUE;
            }
        }
    }
    else
    {
        if (isPassOver_EN)
        {
            if ((psenNewSwing.b.s10 || psenNewSwing.b.s13) && !psenNewSwing.b.s11 && !psenNewSwing.b.s14)
            {
                printf(" [EN] Check PassOver - STANDARD\n");
                return  TRUE;
            }
        }
        else if (isPassOver_EX)
        {
            if ((psenNewSwing.b.s02 || psenNewSwing.b.s05) && !psenNewSwing.b.s03 && !psenNewSwing.b.s06) 
            {
                printf(" [EX] Check PassOver - STANDARD\n");
                return  TRUE;
            }
        }
    }

    return FALSE;
}

void CheckCounterEnterTimer(void)
{
    if (timerInverseEnter.fStart)
    {
        if (IsTimeout(&timerInverseEnter, dwInverseCloseTime * TICK_COUNT_100MS))
        {
            ResetTimer(&timerInverseEnter);

            if (gGCUStatus.IllegalPass.b.nCounter_EN == FLG_SET)
            {
                if (psenNewSwing.section.enter_EX || psenNewSwing.section.lower_EX)
                {
                    StopBarrierForSwing(TRUE);
                    SetTimer(&timerInverseEnter);
                }
                else
                {
                    StopBarrierForSwing(FALSE);
                    Brr_CloseBarrier();
                    isCounterEnter = FALSE;
                }
            }
            else if (gGCUStatus.IllegalPass.b.nCounter_EX == FLG_SET)
            {
                if (psenNewSwing.section.enter_EN || psenNewSwing.section.lower_EN)
                {
                    StopBarrierForSwing(TRUE);
                    SetTimer(&timerInverseEnter);
                }
                else
                {
                    StopBarrierForSwing(FALSE);
                    Brr_CloseBarrier();
                    isCounterEnter = FALSE;
                }
            }
        }
    }
}

void CheckIllegalEnter(void)
{
    if (gisEntryPassenger && gGCUStatus.PassageAlarm.b.nPassOverFromEX != ON)
    {
        if (gGCUStatus.bAuthCount_EX)
        {
            if (gGCUStatus.IllegalPass.b.nCounter_EX == FLG_OFF && isPassReady_EX == FALSE && psenNewSwing.b.s02)
            {
                if (isCounterEnter == FALSE)
                {
                    // SetTimer(&timerInverseEnter);
                    // CloseBarrierForSwing();
                    isCounterEnter = TRUE;
                    InhibitPass(FROM_ENTRY);
                    gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_COUNTER_ENTER;
                    gGCUStatus.IllegalPass.b.nCounter_EX = FLG_SET;
                    printf(" [EX] Inhibit Pass - Inverse Enter: %d \n", gGCUStatus.bAuthCount_EX);
                }
            }
        }
        else if (!gGCUStatus.bAuthCount_EN)
        {
            CheckIllegalEnterTimer(FROM_ENTRY);
        }
    }

    if (gisExitPassenger && gGCUStatus.PassageAlarm.b.nPassOverFromEN != ON)
    {
        if (gGCUStatus.bAuthCount_EN)
        {
            if (gGCUStatus.IllegalPass.b.nCounter_EN == FLG_OFF && isPassReady_EN == FALSE && psenNewSwing.b.s12)
            {
                if (isCounterEnter == FALSE)
                {
                    // SetTimer(&timerInverseEnter);
                    // CloseBarrierForSwing();
                    isCounterEnter = TRUE;
                    InhibitPass(FROM_EXIT);
                    gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_COUNTER_ENTER;
                    gGCUStatus.IllegalPass.b.nCounter_EN = FLG_SET;
                    printf(" [EN] Inhibit Pass - Inverse Enter: %d \n", gGCUStatus.bAuthCount_EN);
                }
            }
        }
        else if (!gGCUStatus.bAuthCount_EX)
        {
            CheckIllegalEnterTimer(FROM_EXIT);
        }
    }
}

void CheckTGTimerForSwing(void)
{
    if (timerTailgatingDetection.fStart)
    {
        if (IsTimeout(&timerTailgatingDetection, dwTgDetectionTime * TICK_COUNT_100MS)) // 1.5 s
        {
            ResetTimer(&timerTailgatingDetection);

            if (gGCUStatus.bAuthCount_EN || gGCUStatus.bAuthCount_EX)
            {
                if (!psenNewSwing.section.lower_EN && !psenNewSwing.section.lower_EX)
                {
                    printf(" [CheckTGTimerForSwing] No follower [%d/%d] \n", gGCUStatus.bAuthCount_EN, gGCUStatus.bAuthCount_EX);

                    if (gGCUStatus.bAuthCount_EN > 0)
                    {
                        gGCUStatus.bAuthCount_EN--;
                    }
                    else if (gGCUStatus.bAuthCount_EX > 0)
                    {
                        gGCUStatus.bAuthCount_EX--;
                    }

                    if (CheckZeroAuthCount())
                    {
                        CloseBarrierForSwing();
                        printf(" [CheckTGTimerForSwing] Close barrier! \n");
                    }
                }
                else
                {
                    printf(" [CheckTGTimerForSwing] Something still detected.\n");
                    SetTimer(&timerTailgatingDetection);
                }
            }
        }
    }
}

void CheckSafetyTimerForSwing(void)
{
    if (timerBarrierStop.fStart)
    {
        if (IsTimeout(&timerBarrierStop, dwBarrierStopTime * TICK_COUNT_100MS))
        {
            printf(" [CheckSafetyTimerForSwing] Safety Stop Expired \n");
            StopBarrierForSwing(FALSE);
            ResetTimer(&timerBarrierStop);
            SetTimer(&timerSafety);
        }
    }
    else if (timerSafety.fStart)
    {
        if (IsTimeout(&timerSafety, gdwTimeoutSafety * TICK_COUNT_100MS))
        {
            printf(" [CheckSafetyTimerForSwing] Safety Time Out! \n");
            ResetTimer(&timerSafety);
            gbAuthDirection = FROM_NONE;
            isSafetyOn = FALSE;
            isSentClose = FALSE;
        }
        else
        {
            if (gbEmergencyFlag == FLG_OFF || gfTestMode == TRUE)
            {
                if (gbAuthDirection == FROM_ENTRY)
                {
                    gGCUStatus.bSafetyDetection = FLG_SET;

                    if (gGCUParameter.bGateType == WIDE)
                    {
                        if (psenNewSwing.section.lower_EX || psenNewSwing.b.s04 || psenNewSwing.b.s08)
                        {
                            printf(" [EN] Safety Activation [%02X/%02X/%02X] \n", psenNewSwing.section.lower_EX, psenNewSwing.b.s04, psenNewSwing.b.s08);
                            StopBarrierForSwing(TRUE);
                            ResetTimer(&timerSafety);
                            SetTimer(&timerBarrierStop);
                            isSafetyOn = TRUE;
                            isSentClose = FALSE;
                        }
                        else if (gfAISafetyOn == FALSE && isSentClose == FALSE)
                        {
                            gGCUStatus.bSafetyDetection = FLG_OFF;
                            isSentClose = TRUE;
                            CloseBarrierForSwing();
                        }
                    }
                    else
                    {
                        if (psenNewSwing.b.s14 || psenNewSwing.b.s15 || psenNewSwing.b.s16 || psenNewSwing.b.s04 || psenNewSwing.b.s08)
                        {
                            printf(" [EN] Safety Activation [%02X/%02X/%02X] \n", psenNewSwing.section.lower_EX, psenNewSwing.b.s04, psenNewSwing.b.s08);
                            StopBarrierForSwing(TRUE);
                            ResetTimer(&timerSafety);
                            SetTimer(&timerBarrierStop);
                            isSafetyOn = TRUE;
                            isSentClose = FALSE;
                        }
                        else if (gfAISafetyOn == FALSE && isSentClose == FALSE)
                        {
                            gGCUStatus.bSafetyDetection = FLG_OFF;
                            isSentClose = TRUE;
                            CloseBarrierForSwing();
                        }
                    }
                }
                else if (gbAuthDirection == FROM_EXIT)
                {
                    gGCUStatus.bSafetyDetection = FLG_SET;

                    if (gGCUParameter.bGateType == WIDE)
                    {
                        if (psenNewSwing.section.lower_EN || psenNewSwing.b.s12 || psenNewSwing.b.s16)
                        {
                            printf(" [EX] Safety Activation [%02X/%02X/%02X] \n", psenNewSwing.section.lower_EN, psenNewSwing.b.s12, psenNewSwing.b.s16);
                            StopBarrierForSwing(TRUE);
                            ResetTimer(&timerSafety);
                            SetTimer(&timerBarrierStop);
                            isSafetyOn = TRUE;
                            isSentClose = FALSE;
                        }
                        else if (gfAISafetyOn == FALSE && isSentClose == FALSE)
                        {
                            gGCUStatus.bSafetyDetection = FLG_OFF;
                            isSentClose = TRUE;
                            CloseBarrierForSwing();
                        }
                    }
                    else
                    {
                        if (psenNewSwing.b.s06 || psenNewSwing.b.s07 || psenNewSwing.b.s08 || psenNewSwing.b.s12 || psenNewSwing.b.s16)
                        {
                            printf(" [EX] Safety Activation [%02X/%02X/%02X] \n", psenNewSwing.section.lower_EN, psenNewSwing.b.s12, psenNewSwing.b.s16);
                            StopBarrierForSwing(TRUE);
                            ResetTimer(&timerSafety);
                            SetTimer(&timerBarrierStop);
                            isSafetyOn = TRUE;
                            isSentClose = FALSE;
                        }
                        else if (gfAISafetyOn == FALSE && isSentClose == FALSE)
                        {
                            gGCUStatus.bSafetyDetection = FLG_OFF;
                            isSentClose = TRUE;
                            CloseBarrierForSwing();
                        }
                    }
                }
                else
                {
                    // For barrier initialization
                    if (psenNewSwing.section.lower_EN || psenNewSwing.section.lower_EN || psenNewSwing.b.s02 || psenNewSwing.b.s10)
                    {
                        printf(" [EN/EX] Safety Activation \n");
                        gGCUStatus.bSafetyDetection = FLG_SET;
                        StopBarrierForSwing(TRUE);
                        ResetTimer(&timerSafety);
                        SetTimer(&timerBarrierStop);
                        isSafetyOn = TRUE;
                        isSentClose = FALSE;
                    }
                    else if (gfAISafetyOn == FALSE && isSentClose == FALSE)
                    {
                        gGCUStatus.bSafetyDetection = FLG_OFF;
                        isSentClose = TRUE;
                        CloseBarrierForSwing();
                    }
                }
            }
        }
    }
}

void CheckResetTimer(void)
{
    static clock_t ms1;

    if (timerReset.fStart)
    {
        if (!mstimeout(&ms1, 1000))
            return;

        if (IsTimeout(&timerReset, dwResetTime * TICK_COUNT_100MS))
        {
            ResetTimer(&timerReset);

            if (gbEmergencyFlag == FLG_SET)
            {
                if (!psenNewSwing.dirEntry.enter && !psenNewSwing.section.lower_EN && !psenNewSwing.side.exit)
                {
                    printf(" [CheckResetTimer] Check point 1 \n");
                    isCalibrationStart = TRUE;
                    isSafetyOn4Init = FALSE;
                    Brr_SetEmergency(FALSE);
                    gbEmergencyFlag = FLG_OFF;
                    ControlIndicatorBlink(FLG_OFF);
                    gbBarrierCmd = BARRIER_CLOSE;
                    dwResetTime = 90;
                    SetTimer(&timerReset);
                }
                else
                {
                    printf(" [CheckResetTimer] Check point 2 \n");

                    isSafetyOn4Init = TRUE;
                    gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_CLOSE_FAULT;
                    gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_CLOSE_FAULT;
                    InitPassageModeForSwing();
                }
            }
            else
            {
                printf(" [CheckResetTimer] Check point 3 \n");

                // Brr_SetSerialComm(TRUE);
                isCalibration = TRUE;
                nCalibrationCnt = 0;
            }
        }
    }
    else
    {
        if (isCalibration == TRUE)
        {
            if (nCalibrationCnt <= 300)
            {
                if (nCalibrationCnt % 20 == 0)
                    Brr_CloseBarrier();
                nCalibrationCnt++;
            }
            else
            {
                nCalibrationCnt = 0;
                isCalibration = FALSE;
                isCalibrationStart = FALSE;
                InitPassageModeForSwing();
            }
        }
    }
}

void CheckLuggageWaitTimerSwing(void)
{
    if (IsTimeout(&timerLuggageWait, gdwTimeoutLuggage))
    {
        ResetTimer(&timerLuggageWait);

        if (0 == gGCUStatus.bAuthCount_EN && 0 == gGCUStatus.bAuthCount_EX)
        {
            printf(" [CheckLuggageWaitTimer] Luggage time over. Door closing.\n");
        }
        else
        {
            printf(" [CheckLuggageWaitTimer] Not passing through yet. Timer running again. \n");
        }
    }
}

void CheckTailgateFilterTimer(void)
{
    if (timerTgFilter.fStart)
    {
        if (IsTimeout(&timerTgFilter, dwTgFilterTime * TICK_COUNT_100MS))
        {
            ResetTimer(&timerTgFilter);

            if (isLuggageSet_EN == TRUE)
            {
                printf(" [CheckTailgateFilterTimer] Luggage sensors detected. No EN TG!! \n");
            }
            else if (isLuggageSet_EX == TRUE)
            {
                printf(" [CheckTailgateFilterTimer] Luggage sensors detected. No EX TG!! \n");
            }
            else
            {
                ControlDirectionLED(DIR_RED, DIR_RED);
                ControlIndicatorBlink(FLG_SET);
                ControlBuzzer(BUZZER_ON, gGCUParameter.bAlarmTimeout);
                SetTailgatingTimer();

                if (gGCUStatus.bAuthCount_EN)
                {
                    gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_TAILGATING;
                    gGCUStatus.IllegalPass.b.nTailgating_EN = FLG_SET;
                    printf(" [EN] Tailgating detected! \n");
                }
                else if (gGCUStatus.bAuthCount_EX)
                {
                    gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_TAILGATING;
                    gGCUStatus.IllegalPass.b.nTailgating_EX = FLG_SET;
                    printf(" [EX] Tailgating detected! \n");
                }
            }
        }
        else
        {
            if (gGCUStatus.bAuthCount_EN && psenNewSwing.b.s13)
            {
                printf(" [CheckTailgateFilterTimer] Something detected. No EN TG!!\n");
                ResetTimer(&timerTgFilter);
            }
            else if (gGCUStatus.bAuthCount_EX && psenNewSwing.b.s05)
            {
                printf(" [CheckTailgateFilterTimer] Something detected. No EX TG!! \n");
                ResetTimer(&timerTgFilter);
            }
        }
    }
}

void CheckCloseWaitTimer(void)
{
    if (IsTimeout(&timerCloseWait, dwCloseWaitTime * TICK_COUNT_100MS))
    {
        ResetTimer(&timerCloseWait);
        ControlBarrier(BARRIER_CLOSE);
    }
}

void CheckStopHoldingTimer(void)
{
    if (IsTimeout(&timerStopHolding, dwStopHoldingTime * TICK_COUNT_10SEC))
    {
        ResetTimer(&timerStopHolding);
        Brr_StopBarrier(FALSE);
    }
}

/*******************************************************************************
 * Main Passage Process for swing gate
 *******************************************************************************/

void PassageProcessForSwing()
{
    UpdateSensorData();

    // SCADA data
    gGCUStatus.bSCADA = (*(BYTE *)READ03_ADR);

    CheckBarrierOperation();
    CheckSafetyTimerForSwing();
    CheckIndicatorTimer();
    CheckBlinkTimer();
    CheckBuzzerTimer();
    CheckEmergencySignal();
    CheckUPSStatus();
    CheckResetTimer();
    CheckCloseWaitTimer();
    CheckStopHoldingTimer();

    if (memcmp(&psenNewSwing, &psenOldSwing, sizeof(T_PASS_SEN_SWING)))
    {
        // check timers
        ResetTimer(&timerPassageClear);

        if (!gCurGCUOpMode.bEmergencyMaint && gfFreeMode == FALSE)
        {
            if ((gCurGCUOpMode.bServiceMode_EN != NO_SERVICE) || (gCurGCUOpMode.bServiceMode_EX != NO_SERVICE))
            {
                CheckEntranceSwing();
                CheckPassReadySwing();
                CheckPassingSwing();
            }
        }
    }
    else
    {
        if (psenNewSwing.dirEntry.enter || psenNewSwing.section.lower_EN || psenNewSwing.side.exit)
        {
            // Update Timer
            ResetTimer(&timerPassageClear);
        }
        else
        {
            if (timerPassageClear.fStart)
            {
                if (IsTimeout(&timerPassageClear, (DEFAULT_PASSAGE_CLEAR_TIMEOUT * TICK_COUNT_1SEC)))
                {
                    isPassReady_EN = FALSE;
                    isPassReady_EX = FALSE;
                    isLuggageSet_EN = FALSE;
                    isLuggageSet_EX = FALSE;
                    ClearPassageAlarm();
                    ResetTimer(&timerPassageClear);
                }
            }
            else
            {
                // Last update 20230107
                // if (gGCUStatus.PassageAlarm.b.nFromEN || gGCUStatus.PassageAlarm.b.nFromEX )
                SetTimer(&timerPassageClear);
            }
        }

        CheckAuthTimeout();
        CheckModeChange();
    }

    if (!gCurGCUOpMode.bEmergencyMaint)
    {
        CheckTailgateFilterTimer();
        CheckCounterEnterTimer();
        CheckIllegalEnter();
        CheckTGTimerForSwing();
        CheckIllegalAlarmTimer();
    }

    // update sensor state
    memcpy(&psenOldSwing, &psenNewSwing, sizeof(T_PASS_SEN_SWING));

    CheckPassSenError(TRUE);
}

/********END OF FILE********/
