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
T_PASS_SEN_SWING psenNew;				// T_PASS_SEN structure does not exist		pms

BYTE bEnterPattern_EN = PASS_PATN_NONE;
BYTE bEnterPattern_EX = PASS_PATN_NONE;
BYTE bPassPattern_EN = PASS_PATN_NONE;
BYTE bPassPattern_EX = PASS_PATN_NONE;
BYTE bCountPattern_EN = PASS_PATN_NONE;
BYTE bCountPattern_EX = PASS_PATN_NONE;

// Setting time for Timer
DWORD dwCloseWaitTime = 2;		 // unit: 100ms
DWORD dwResetTime = 30;			 // unit: 100ms
DWORD dwTgFilterTime = 2;		 // unit: 100 ms
DWORD dwTgDetectionTime = 10;	 // unit: 100 ms
DWORD dwInverseCloseTime = 4;	 // unit: 100 ms
DWORD dwBarrierCheckTime = 2;	 // unit: 100 ms
DWORD dwBarrierStopTime = 5;	 // unit: 100 ms
DWORD dwForceOpenClearTime = 10; // unit: 100 ms

bool isPassReady_EN = FALSE; // pass-ready state of direction Entry
bool isPassReady_EX = FALSE; // pass-ready state of direction Exit
bool isPassOver_EN = FALSE;	 // pass-over state of direction A
bool isPassOver_EX = FALSE;	 // pass-over state of direction B
bool isLuggageSet_EN = FALSE;
bool isLuggageSet_EX = FALSE;
bool isForceOpenReady_EN = FALSE;
bool isForceOpenReady_EX = FALSE;
bool isBarrierStop = FALSE;
bool isForceOpen = FALSE;
bool isSentForceOpen = FALSE;
bool isTailgating = FALSE;
bool isSafetyOn = FALSE;
bool isCounterEnter = FALSE;
bool isSafetyOn4Init = FALSE;
bool isCalibration = FALSE;
bool isCalibrationStart = FALSE;
bool isSentClose = FALSE;
bool isCountReady_EN = FALSE;
bool isCountReady_EX = FALSE;

int nCalibrationCnt = 0;
int nBarrierErrorCnt = 0;
int nForceOpenCnt = 0;

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
	if (gisEntryPassenger)
	{
		if (gGCUParameter.bCriticalZone == ALARM_ZONE1)
			return TRUE;
		else if (gGCUParameter.bCriticalZone == ALARM_ZONE2 && psenNewSwing.dirEntry.passage)
			return TRUE;
		else if (gGCUParameter.bCriticalZone == ALARM_ZONE3 && (psenNewSwing.b.s03))
			return TRUE;
	}
	else if (gisExitPassenger)
	{
		if (gGCUParameter.bCriticalZone == ALARM_ZONE1)
			return TRUE;
		else if (gGCUParameter.bCriticalZone == ALARM_ZONE2 && psenNewSwing.dirExit.passage)
			return TRUE;
		else if (gGCUParameter.bCriticalZone == ALARM_ZONE3 && (psenNewSwing.b.s11))
			return TRUE;
	}

	return FALSE;
}

void UpdateSensorData()
{
	psenNew.B.bSen1 = RD_PASSSEN1 & (~psenError.B.bSen1);
	psenNew.B.bSen2 = RD_PASSSEN2 & (~psenError.B.bSen2);
}

void StopBarrierForSwing(bool isStop)
{
	isBarrierStop = isStop;
	Brr_StopBarrier(isStop);
}

void InitPassageModeForSwing(void)
{
	if (isBarrierStop == TRUE)
	{
		//PRINTL(" [InitPassageMode] Release the barrier stop");
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
		bPassPattern_EN = PASS_PATN_NONE;
		bPassPattern_EX = PASS_PATN_NONE;

		//PRINTL(" [InitPassageMode] bEmergencyMaint: %d", gCurGCUOpMode.bEmergencyMaint);.
		printf(" [InitPassageMode] bEmergencyMaint: %d \n", gCurGCUOpMode.bEmergencyMaint);

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
		bPassPattern_EN = PASS_PATN_NONE;
		bPassPattern_EX = PASS_PATN_NONE;

		if (FLG_SET == gbEmergencyFlag && gfTestMode == FALSE)
		{
			//PRINTL(" [InitPassageMode] Safety Check for mode change!!");
			printf(" [InitPassageMode] Safety Check for mode change!! \n");
			ControlBuzzer(BUZZER_NO_MAIN, BUZZER_ON, 2);
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
				else //  if (gCurGCUOpMode.bEmergencyMaint == MAINTENANCE_MODE)
					ControlBarrier(BARRIER_CLOSE_S);
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

			ControlBuzzer(BUZZER_NO_MAIN, BUZZER_OFF, 0);
			ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
			ControlIndicator(gbIndicatorCMD_EN, gbIndicatorCMD_EX, 0, FALSE);

			if (CheckZeroAuthCount())
			{
				//PRINTL(" [InitPassageMode] ResetAuthTimer!!");
				printf(" [InitPassageMode] ResetAuthTimer!! \n");
				ResetTimer(&timerAuthorize);
			}
		}
	}
}

void CloseBarrierForSwing(void)
{
	if (gfTestMode == TRUE || CheckBarrierClosedStatus())
	{
		if (isBarrierStop == TRUE)
		{
			StopBarrierForSwing(FALSE);
		}

		isForceOpen = FALSE;
		Brr_CloseBarrier();
	}
}

void CheckBarrierOperation(void)
{
	if (gfTestMode == FALSE && timerBarrierCheck.fStart)
	{
		if (IsTimeout(&timerBarrierCheck, dwBarrierCheckTime * TICK_COUNT_100MS))
		{
			ResetTimer(&timerBarrierCheck);

			// PRINTL(" =[%d]= ", gbBarrierCmd);
			if (gGCUStatus.ModuleAlarm.b.nFDoor1 != FDOOR_ALARM_NONE || gGCUStatus.ModuleAlarm.b.nFDoor2 != FDOOR_ALARM_NONE)
			{
				//PRINTL(" [BarrierStatus] BNC:%d/%d", gGCUStatus.ModuleAlarm.b.nFDoor1, gGCUStatus.ModuleAlarm.b.nFDoor2);
				printf(" [BarrierStatus] BNC:%d/%d \n", gGCUStatus.ModuleAlarm.b.nFDoor1, gGCUStatus.ModuleAlarm.b.nFDoor2);
			}

			// PRINTL(" [BarrierStatus] act:%d/%d", gMainBarrierStatus.param.actStatus, gSubBarrierStatus.param.actStatus);
			// PRINTL(" [BarrierStatus] Running:%d/%d", gfMainBarrierRunning, gfSubBarrierRunning);

			if ((gfMainBarrierRunning == FALSE || gMainBarrierStatus.param.actStatus == 0) &&
				(gfSubBarrierRunning == FALSE || gSubBarrierStatus.param.actStatus == 0))
			{
				if (gbBarrierCmd == BARRIER_OPEN_FOR_EN || gbBarrierCmd == BARRIER_OPEN_FOR_EX || gbEmergencyFlag == FLG_SET)
				{
					if ((gfMainBarrierRunning == TRUE && gMainBarrierStatus.param.posStatus != SDOOR_OPEN) ||
						(gfSubBarrierRunning == TRUE && gSubBarrierStatus.param.posStatus != SDOOR_OPEN))
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
							//PRINTL(" [CheckBarrier] Current Status is open");
							printf(" [CheckBarrier] Current Status is open \n");
							ControlBarrier(BARRIER_CLOSE_S);
						}
					}
				}
				else if (gbBarrierCmd == BARRIER_CLOSE_S)
				{
					if ((gfMainBarrierRunning == TRUE && gMainBarrierStatus.param.posStatus != SDOOR_CLOSE) ||
						(gfSubBarrierRunning == TRUE && gSubBarrierStatus.param.posStatus != SDOOR_CLOSE))
					{
						if (CheckBarrierClosedStatus())
						{
							//PRINTL(" [CheckBarrier] pos:%d/%d", gMainBarrierStatus.param.posStatus, gSubBarrierStatus.param.posStatus);
							printf(" [CheckBarrier] pos:%d/%d \n", gMainBarrierStatus.param.posStatus, gSubBarrierStatus.param.posStatus);

							if (isCalibrationStart == FALSE && (isSafetyOn == FALSE || gfisAuthTimeout == TRUE))
							{
								nBarrierErrorCnt++;

								if (nBarrierErrorCnt == 20) // After 4 sec, Barrier fault is generated.
								{
									if (gGCUStatus.ModuleAlarm.b.nFDoor1 == FDOOR_ALARM_NONE && gGCUStatus.ModuleAlarm.b.nFDoor2 == FDOOR_ALARM_NONE)
									{
										ControlBuzzer(BUZZER_NO_MAIN, BUZZER_ON, gGCUParameter.bAlarmTimeout);
									}

									if (gfMainBarrierRunning == TRUE && gMainBarrierStatus.param.posStatus != SDOOR_CLOSE)
										gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_CLOSE_FAULT;
									if (gfSubBarrierRunning == TRUE && gSubBarrierStatus.param.posStatus != SDOOR_CLOSE)
										gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_CLOSE_FAULT;

									nBarrierErrorCnt = 0;
									ControlBarrier(BARRIER_CLOSE_S);
								}
							}
						}
					}
					else if (isSafetyOn4Init == FALSE)
					{
						nForceOpenCnt = 0;
						nBarrierErrorCnt = 0;
						gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_NONE;
						gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_NONE;
						gGCUStatus.bBarrierSw = 0x22;
						isForceOpen = FALSE;

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
					}
				}
			}
			else
			{
				if (!timerTailgating.fStart && gGCUStatus.bAuthCount_EN == 0 && gGCUStatus.bAuthCount_EX == 0 && CheckBarrierClosedStatus())
				{
					if (isCalibrationStart == FALSE)
					{
						if (isForceOpenReady_EN == TRUE || isForceOpenReady_EX == TRUE)
						{
							nForceOpenCnt++;

							if (nForceOpenCnt >= 2)
							{
								//PRINTL(" [CheckBarrier] Force Open Detected!!");
								printf(" [CheckBarrier] Force Open Detected!!\n");
								isForceOpen = TRUE;
							}
						}
					}
				}
				else
				{
					nBarrierErrorCnt = 0;
				}
			}

			Brr_Inq_Status();			//확인 필요 pms
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
			gisEntryPassenger = FALSE;

			if (gGCUStatus.IllegalPass.b.nCounter_EN == FLG_SET)
			{
				gGCUStatus.IllegalPass.b.nCounter_EN = FLG_OFF;
				//PRINTL(" [EN] Clear Counter Enter");
				printf(" [EN] Clear Counter Enter \n");
			}

			if (gGCUStatus.IllegalPass.b.nIllegal_EN == FLG_SET)
			{
				gGCUStatus.IllegalPass.b.nIllegal_EN = FLG_OFF;
				//PRINTL(" [EN] Clear Illegal Enter");
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
			gisExitPassenger = FALSE;

			if (gGCUStatus.IllegalPass.b.nCounter_EX == FLG_SET)
			{
				gGCUStatus.IllegalPass.b.nCounter_EX = FLG_OFF;
				//PRINTL(" [EX] Clear Counter Enter");
				printf(" [EX] Clear Counter Enter \n");
			}

			if (gGCUStatus.IllegalPass.b.nIllegal_EX == FLG_SET)
			{
				gGCUStatus.IllegalPass.b.nIllegal_EX = FLG_OFF;
				//PRINTL(" [EX] Clear Illegal Enter");
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

		ControlBuzzer(BUZZER_NO_MAIN, BUZZER_OFF, 0);
		ControlDirectionLED(gbLampCMD_EN, gbLampCMD_EX);
		ControlIndicator(gbIndicatorCMD_EN, gbIndicatorCMD_EX, 0, FALSE);
	}
}

void CheckEntranceSwing(void)
{
	if (gisEntryPassenger)
	{
		if (gGCUStatus.bAuthCount_EN)
		{
			if (!psenNewSwing.side.entry)
			{
				ResetTimer(&timerIllegalEnterEN);
				gisEntryPassenger = FALSE;
				isPassReady_EN = FALSE;
				bEnterPattern_EN = PASS_PATN_NONE;
				bPassPattern_EN = PASS_PATN_NONE;
				gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
				isCounterEnter = FALSE;
				ClearPassHistory();
				//PRINTL(" [EN] Enter Clear 1");
				printf(" [EN] Enter Clear 1\n");
			}
		}
		else
		{
			if (!psenNewSwing.section.enter_EN && !psenNewSwing.section.lower_EN)
			{
				ResetTimer(&timerIllegalEnterEN);
				gisEntryPassenger = FALSE;
				isPassReady_EN = FALSE;
				bEnterPattern_EN = PASS_PATN_NONE;
				bPassPattern_EN = PASS_PATN_NONE;
				gGCUStatus.PassageAlarm.b.nPassOverFromEX = OFF;
				isCounterEnter = FALSE;
				ClearPassHistory();
				if (gGCUStatus.bAuthCount_EX)
				{
					ControlBarrier(BARRIER_OPEN_FOR_EX);
				}
				//PRINTL(" [EN] Enter Clear 2");
				printf(" [EN] Enter Clear 2\n");
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

					//PRINTL(" [EN] Enter - [%d]", bEnterPattern_EN);
					printf(" [EN] Enter - [%d]\n", bEnterPattern_EN);
				}
			}
		}
	}

	if (gisExitPassenger)
	{
		if (gGCUStatus.bAuthCount_EX)
		{
			if (!psenNewSwing.side.exit)
			{
				ResetTimer(&timerIllegalEnterEX);
				gisExitPassenger = FALSE;
				isPassReady_EX = FALSE;
				bEnterPattern_EX = PASS_PATN_NONE;
				bPassPattern_EX = PASS_PATN_NONE;
				gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
				isCounterEnter = FALSE;
				ClearPassHistory();
				//PRINTL(" [EX] Enter Clear 1");
				printf(" [EX] Enter Clear 1\n");
			}
		}
		else
		{
			if (!psenNewSwing.section.enter_EX && !psenNewSwing.section.lower_EX)
			{
				ResetTimer(&timerIllegalEnterEX);
				gisExitPassenger = FALSE;
				isPassReady_EX = FALSE;
				bEnterPattern_EX = PASS_PATN_NONE;
				bPassPattern_EX = PASS_PATN_NONE;
				gGCUStatus.PassageAlarm.b.nPassOverFromEN = OFF;
				isCounterEnter = FALSE;
				ClearPassHistory();
				if (gGCUStatus.bAuthCount_EN)
				{
					ControlBarrier(BARRIER_OPEN_FOR_EN);
				}
				//PRINTL(" [EX] Enter Clear 2");
				printf(" [EX] Enter Clear 2\n");
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

					//PRINTL(" [EX] Enter - [%d]", bEnterPattern_EX);
					printf(" [EX] Enter - [%d]\n", bEnterPattern_EX);
				}
			}
		}
	}
}

void CheckPassReadySwing(void)
{
	if (gisExitPassenger == TRUE)
	{
		if (isPassReady_EN == FALSE)
		{
			bPassPattern_EX |= psenNewSwing.dirExit.passage;

			if (bPassPattern_EX >= PASS_PATN_PASS_READY)
			{
				//PRINTL(" [EX] Pass Ready");
				printf(" [EX] Pass Ready \n");
				isPassReady_EX = TRUE;

				if (gGCUStatus.bAuthCount_EX)
				{
					gisExitPassenger = FALSE;
				}
				else if (CheckZeroAuthCount() && gGCUStatus.IllegalPass.b.nTailgating_EX == FLG_OFF && gGCUStatus.PassageAlarm.b.nPassOverFromEX == OFF)
				{
					// if nobody is in the opposite side
					if (!psenNewSwing.section.lower_EN)
					{
						isForceOpenReady_EX = TRUE;
						//PRINTL(" [EX] Force Open Ready");
						printf(" [EX] Force Open Ready\n");
					}
				}
			}
		}

		// Luggage check (Stroller, Bicycle, Luggage)
		if (gGCUStatus.bAuthCount_EX && psenNewSwing.dirExit.enter && psenNewSwing.b.s13 && psenNewSwing.b.s14 && !psenNewSwing.b.s11)
		{
			isLuggageSet_EX = TRUE;
			//PRINTL(" [EX] Set Luggage Flag");
			printf(" [EX] Set Luggage Flag\n");
		}
	}
	else if (gisEntryPassenger == TRUE)
	{
		if (isPassReady_EX == FALSE)
		{
			bPassPattern_EN |= psenNewSwing.dirEntry.passage;

			if (bPassPattern_EN >= PASS_PATN_PASS_READY)
			{
				//PRINTL(" [EN] Pass Ready");
				printf(" [EN] Pass Ready \n");
				isPassReady_EN = TRUE;

				if (gGCUStatus.bAuthCount_EN)
				{
					gisEntryPassenger = FALSE;
					// bEnterPattern_EN = PASS_PATN_NONE;
				}
				else if (CheckZeroAuthCount() && gGCUStatus.IllegalPass.b.nTailgating_EN == FLG_OFF && gGCUStatus.PassageAlarm.b.nPassOverFromEN == OFF)
				{
					// if nobody is in the opposite side
					if (!psenNewSwing.section.lower_EX)
					{
						isForceOpenReady_EN = TRUE;
						//PRINTL(" [EN]  Force Open Ready");
						printf(" [EN]  Force Open Ready \n");
					}
				}
			}
		}

		// Luggage check (Stroller, Bicycle, Luggage)
		if (gGCUStatus.bAuthCount_EN && psenNewSwing.dirEntry.enter && psenNewSwing.b.s03 && psenNewSwing.b.s04 && !psenNewSwing.b.s01)
		{
			isLuggageSet_EN = TRUE;
			//PRINTL(" [EN] Set Luggage Flag");
			printf(" [EN] Set Luggage Flag  \n");
		}
	}
}

// JOB DONE
void CheckPassingSwing(void)
{
	// To prevent the counter-enter, I utilize two boolean variable "isPassReady" and "isPassOver"
	if (isPassReady_EN)
	{
		if (isPassOver_EN)
		{
			if (psenNewSwing.dirEntry.end)
			{
				//PRINTL(" [EN] Pass over");
				printf(" [EN] Pass over  \n");
				isPassReady_EN = FALSE;
				isPassOver_EN = FALSE;
				bPassPattern_EN = PASS_PATN_NONE;

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
						SetTimer(&timerCloseWait);
						ResetTimer(&timerAuthorize);
						//PRINTL(" [EN] Barrier close");
						printf(" [EN] Barrier close \n");
					}
				}
				else if (gGCUStatus.bAuthCount_EN > 1)
				{
					gGCUStatus.bAuthCount_EN--;
					SetAuthTimer();
				}
			}
		}
		else
		{
			if (psenNewSwing.dirEntry.lower)
			{
				//PRINTL(" [EN] passing before");
				printf(" [EN] passing before \n");

				if (gGCUStatus.bAuthCount_EN || gGCUStatus.IllegalPass.b.nTailgating_EN == FLG_SET)
				{
					isPassOver_EN = TRUE;
					gGCUStatus.PassageAlarm.b.nPassOverFromEN = ON;
					//PRINTL(" [EN] passing");
					printf(" [EN] passing \n");
				}
			}
		}
	}

	if (isPassReady_EX)
	{
		if (isPassOver_EX)
		{
			if (psenNewSwing.dirExit.end)
			{
				//PRINTL(" [EX] Pass over");
				printf(" [EX] Pass over \n");
				isPassReady_EX = FALSE;
				isPassOver_EX = FALSE;
				bPassPattern_EX = PASS_PATN_NONE;

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
						SetTimer(&timerCloseWait);
						ResetTimer(&timerAuthorize);
						//PRINTL(" [EX] Barrier close");
						printf(" [EX] Barrier close \n");
					}
				}
				else if (gGCUStatus.bAuthCount_EX > 1)
				{
					gGCUStatus.bAuthCount_EX--;
					SetAuthTimer();
				}
			}
		}
		else
		{
			if (psenNewSwing.dirExit.lower)
			{
				//PRINTL(" [EX] passing before");
				printf(" [EX] passing before \n");

				if (gGCUStatus.bAuthCount_EX || gGCUStatus.IllegalPass.b.nTailgating_EX == FLG_SET)
				{
					isPassOver_EX = TRUE;
					gGCUStatus.PassageAlarm.b.nPassOverFromEX = ON;
					//PRINTL(" [EX] passing");
					printf(" [EX] passing\n");
				}
			}
		}
	}
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

void CheckIllegalEnterSwing(void)
{
	if (gisEntryPassenger && gGCUStatus.PassageAlarm.b.nPassOverFromEX != ON)
	{
		if (gGCUStatus.bAuthCount_EX)
		{
			if (gGCUStatus.IllegalPass.b.nCounter_EX == FLG_OFF && isPassReady_EX == FALSE && psenNewSwing.b.s02)
			{
				if (isCounterEnter == FALSE)
				{
					SetTimer(&timerInverseEnter);
					CloseBarrierForSwing();
					isCounterEnter = TRUE;
					InhibitPass(FROM_ENTRY);
					gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_COUNTER_ENTER;
					gGCUStatus.IllegalPass.b.nCounter_EX = FLG_SET;
					//PRINTL(" [EX] Inhibit Pass - Inverse Enter:%d", gGCUStatus.bAuthCount_EX);
					printf(" [EX] Inhibit Pass - Inverse Enter:%d \n", gGCUStatus.bAuthCount_EX);
				}
			}
		}
		else if (!gGCUStatus.bAuthCount_EN)
		{
			if (CheckCritical())
			{
				CheckIllegalEnterTimer(FROM_ENTRY);
			}
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
					SetTimer(&timerInverseEnter);
					CloseBarrierForSwing();
					isCounterEnter = TRUE;
					InhibitPass(FROM_EXIT);
					gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_COUNTER_ENTER;
					gGCUStatus.IllegalPass.b.nCounter_EN = FLG_SET;
					//PRINTL(" [EN] Inhibit Pass - Inverse Enter:%d", gGCUStatus.bAuthCount_EN);
					printf(" [EN] Inhibit Pass - Inverse Enter:%d \n", gGCUStatus.bAuthCount_EN);
				}
			}
		}
		else if (!gGCUStatus.bAuthCount_EX)
		{
			if (CheckCritical())
			{
				CheckIllegalEnterTimer(FROM_EXIT);
			}
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
					//PRINTL(" [CheckTGTimerForSwing] No follower [%d/%d]", gGCUStatus.bAuthCount_EN, gGCUStatus.bAuthCount_EX);
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
						//PRINTL(" [CheckTGTimerForSwing] Close barrier!");
						printf(" [CheckTGTimerForSwing] Close barrier! \n");
					}
				}
				else
				{
					//PRINTL(" [CheckTGTimerForSwing] Something still detected.");
					printf(" [CheckTGTimerForSwing] Something still detected.\n");
					SetTimer(&timerTailgatingDetection);
				}
			}
		}
	}
}

void CheckForceOpen(void)
{
	if (timerForceOpenClear.fStart)
	{
		if (IsTimeout(&timerForceOpenClear, dwForceOpenClearTime * TICK_COUNT_100MS))
		{
			//PRINTL(" [CheckForceOpen] Flag Clear!!!!");
			printf(" [CheckForceOpen] Flag Clear!!!! \n");
			ResetTimer(&timerForceOpenClear);
			isForceOpen = FALSE;
			gGCUStatus.IllegalPass.b.nJumping_EN = FLG_OFF;
			gGCUStatus.IllegalPass.b.nJumping_EX = FLG_OFF;
			gGCUStatus.bDoorForcedOpen = FROM_NONE;
		}
	}
	else
	{
		if (isForceOpen == TRUE && gGCUStatus.bAuthCount_EN == 0 && gGCUStatus.bAuthCount_EX == 0)
		{
			if (isForceOpenReady_EN == TRUE && (psenNewSwing.b.s13 || psenNewSwing.b.s14))
			{
				isForceOpenReady_EN = FALSE;
				gGCUStatus.bDoorForcedOpen = FROM_ENTRY;
				ControlBuzzer(BUZZER_NO_MAIN, BUZZER_ON, gGCUParameter.bAlarmTimeout);
				InhibitPass(FROM_EXIT);
				SetTimer(&timerForceOpenClear);
				isForceOpen = FALSE;
				//PRINTL(" [EN] Force Open!");
				printf(" [EN] Force Open! \n");
			}

			if (isForceOpenReady_EX == TRUE && (psenNewSwing.b.s05 || psenNewSwing.b.s06))
			{
				isForceOpenReady_EX = FALSE;
				gGCUStatus.bDoorForcedOpen = FROM_EXIT;
				ControlBuzzer(BUZZER_NO_MAIN, BUZZER_ON, gGCUParameter.bAlarmTimeout);
				InhibitPass(FROM_ENTRY);
				SetTimer(&timerForceOpenClear);
				isForceOpen = FALSE;
				//PRINTL(" [EX] Force Open!");
				printf(" [EX] Force Open! \n");
			}
		}
	}
}

void CheckBarrierStop(void)
{
	if (timerBarrierStop.fStart)
	{
		if (IsTimeout(&timerBarrierStop, dwBarrierStopTime * TICK_COUNT_100MS))
		{
			//PRINTL(" [CheckBarrierStop] Safety Stop Expired");
			printf(" [CheckBarrierStop] Safety Stop Expired \n");
			StopBarrierForSwing(FALSE);
			ResetTimer(&timerBarrierStop);
			SetTimer(&timerSafety);
		}
	}
}

void CheckSafetyTimerForSwing(void)
{
	if (timerSafety.fStart)
	{
		if (IsTimeout(&timerSafety, gdwTimeoutSafety * TICK_COUNT_100MS))
		{
			//PRINTL(" [CheckSafetyTimerForSwing] Safety Time Out!");
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

					if (psenNewSwing.section.lower_EX || psenNewSwing.b.s04 || psenNewSwing.b.s08 || gfAISafetyOn == TRUE)
					{
						//PRINTL(" [EN] Safety Activation [%d/%d/%d]", psenNewSwing.section.lower_EX, psenNewSwing.b.s04, psenNewSwing.b.s08);
						printf(" [EN] Safety Activation [%d/%d/%d] \n", psenNewSwing.section.lower_EX, psenNewSwing.b.s04, psenNewSwing.b.s08);
						StopBarrierForSwing(TRUE);
						ResetTimer(&timerSafety);
						SetTimer(&timerBarrierStop);
						isSafetyOn = TRUE;
						isSentClose = FALSE;
					}
					else
					{
						if (!psenNewSwing.b.s08 && isSentClose == FALSE)
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

					if (psenNewSwing.section.lower_EN || psenNewSwing.b.s12 || psenNewSwing.b.s16 || gfAISafetyOn == TRUE)
					{
						//PRINTL(" [EX] Safety Activation [%d/%d/%d]", psenNewSwing.section.lower_EN, psenNewSwing.b.s12, psenNewSwing.b.s16);
						printf(" [EX] Safety Activation [%d/%d/%d] \n", psenNewSwing.section.lower_EN, psenNewSwing.b.s12, psenNewSwing.b.s16);
						StopBarrierForSwing(TRUE);
						ResetTimer(&timerSafety);
						SetTimer(&timerBarrierStop);
						isSafetyOn = TRUE;
						isSentClose = FALSE;
					}
					else
					{
						if (!psenNewSwing.b.s16 && isSentClose == FALSE)
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
					if (psenNewSwing.section.lower_EN || psenNewSwing.section.lower_EN || psenNewSwing.b.s02 || psenNewSwing.b.s10 || gfAISafetyOn == TRUE)
					{
						//PRINTL(" [EN/EX] Safety Activation");
						printf(" [EN/EX] Safety Activation \n");
						gGCUStatus.bSafetyDetection = FLG_SET;
						StopBarrierForSwing(TRUE);
						ResetTimer(&timerSafety);
						SetTimer(&timerBarrierStop);
						isSafetyOn = TRUE;
						isSentClose = FALSE;
					}
					else if (isSentClose == FALSE)
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
	if (timerReset.fStart)
	{
		if (IsTimeout(&timerReset, dwResetTime * TICK_COUNT_100MS))
		{
			ResetTimer(&timerReset);

			if (gbEmergencyFlag == FLG_SET)
			{
				if (!psenNewSwing.dirEntry.enter && !psenNewSwing.section.lower_EN && !psenNewSwing.side.exit)
				{
					isCalibrationStart = TRUE;
					isSafetyOn4Init = FALSE;
					Brr_SetEmergency(FALSE);
					gbEmergencyFlag = FLG_OFF;
					ControlIndicatorBlink(FLG_OFF);
					gbBarrierCmd = BARRIER_CLOSE_S;
					dwResetTime = 90;
					SetTimer(&timerReset);
				}
				else
				{
					isSafetyOn4Init = TRUE;
					gGCUStatus.ModuleAlarm.b.nFDoor1 = FDOOR_ALARM_CLOSE_FAULT;
					gGCUStatus.ModuleAlarm.b.nFDoor2 = FDOOR_ALARM_CLOSE_FAULT;
					InitPassageModeForSwing();
				}
			}
			else
			{
				Brr_SetSerialComm(TRUE);
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
			//PRINTL(" [CheckLuggageWaitTimer] Luggage time over. Door closing.");
			printf(" [CheckLuggageWaitTimer] Luggage time over. Door closing.\n");
			// ControlBarrier(BARRIER_CLOSE_S);
		}
		else
		{
			//PRINTL(" [CheckLuggageWaitTimer] Not passing through yet. Timer running again.");
			printf(" [CheckLuggageWaitTimer] Not passing through yet. Timer running again. \n");
			// SetTimer(&timerLuggageWait);
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
				//PRINTL(" [CheckTailgateFilterTimer] Luggage sensors detected. No EN TG!!");
				printf(" [CheckTailgateFilterTimer] Luggage sensors detected. No EN TG!! \n");
			}
			else if (isLuggageSet_EX == TRUE)
			{
				//PRINTL(" [CheckTailgateFilterTimer] Luggage sensors detected. No EX TG!!");
				printf(" [CheckTailgateFilterTimer] Luggage sensors detected. No EX TG!! \n");
			}
			else
			{
				ControlDirectionLED(DIR_RED, DIR_RED);
				ControlIndicatorBlink(FLG_SET);
				ControlBuzzer(BUZZER_NO_MAIN, BUZZER_ON, gGCUParameter.bAlarmTimeout);
				SetTailgatingTimer();

				if (gGCUStatus.bAuthCount_EN)
				{
					gGCUStatus.PassageAlarm.b.nFromEN = PASS_ALARM_TAILGATING;
					gGCUStatus.IllegalPass.b.nTailgating_EN = FLG_SET;
					//PRINTL(" [EN] Tailgating detected!");
					printf(" [EN] Tailgating detected! \n");
				}
				else if (gGCUStatus.bAuthCount_EX)
				{
					gGCUStatus.PassageAlarm.b.nFromEX = PASS_ALARM_TAILGATING;
					gGCUStatus.IllegalPass.b.nTailgating_EX = FLG_SET;
					//PRINTL(" [EX] Tailgating detected!");
					printf(" [EX] Tailgating detected! \n");
				}
			}
		}
		else
		{
			if (gGCUStatus.bAuthCount_EN && psenNewSwing.b.s13)
			{
				//PRINTL(" [CheckTailgateFilterTimer] Something detected. No EN TG!!");
				printf(" [CheckTailgateFilterTimer] Something detected. No EN TG!!\n");
				ResetTimer(&timerTgFilter);
			}
			else if (gGCUStatus.bAuthCount_EX && psenNewSwing.b.s05)
			{
				//PRINTL(" [CheckTailgateFilterTimer] Something detected. No EX TG!!");
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
		ControlBarrier(BARRIER_CLOSE_S);
	}
}

/*******************************************************************************
 * Main Passage Process for swing gate
 *******************************************************************************/

void PassageProcessForSwing()				//used barrierTest from WMATA		pms
{
	UpdateSensorData();

	// SCADA data
	gGCUStatus.bSCADA = (*(BYTE*)READ03_ADR);

	if ((RD_SWITCH & MASK_PUSH) == MASK_PUSH)			//RD_SWITCH1 -> RD_SWITCH
	{
		Brr_SetEmergency(TRUE);
	}

	CheckBarrierOperation();
	CheckBarrierStop();
	CheckSafetyTimerForSwing();
	CheckIndicatorTimer();
	CheckBlinkTimer();					//function does not exist
	CheckBuzzerTimer();
	CheckEmergencySignal();
	CheckUPSStatus();
	CheckForceOpen();
	CheckResetTimer();
	CheckCloseWaitTimer();

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
		// if (psenNewSwing.B.bSen1 || psenNewSwing.B.bSen2 || psenNewSwing.B.bSen3)
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
					isForceOpenReady_EN = FALSE;
					isForceOpenReady_EX = FALSE;
					isPassReady_EN = FALSE;
					isPassReady_EX = FALSE;
					isLuggageSet_EN = FALSE;
					isLuggageSet_EX = FALSE;
					isForceOpen = FALSE;
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
		CheckIllegalEnterSwing();
		CheckTGTimerForSwing();
		CheckIllegalAlarmTimer();
	}

	// update sensor state
	memcpy(&psenOldSwing, &psenNewSwing, sizeof(T_PASS_SEN_SWING));

	CheckPassSenError(TRUE);
}

/********END OF FILE********/
