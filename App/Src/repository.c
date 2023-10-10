/*******************************************************************************
 *       COPYRIGHT �� 2023 STraffic Co., Ltd. All rights reserved
 ********************************************************************************
 * File Name    : repository.c
 * Author       : Joseph Yosep Choi
 * Date Created : 2023/09/14
 * Description  : Implementation of global repository
 *******************************************************************************/

#include "repository.h"
#include "global.h"

// Previous direction
BYTE gbPrevDir = 0;

// Barrier command
BYTE gbBarrierCmd = BARRIER_CLOSE_S;

// Barrier force open
BYTE gbDoorForecedOpen = OFF;

// Barrier type
BYTE gbBarrierType = BARRIER_SWING;

bool gisEntryPassenger = FALSE; // passenger detection state at enter of Entry
bool gisExitPassenger = FALSE;  // passenger detection state at enter of Exit
bool gfTailgating = FALSE;      // Detecting trying tailgating
bool gfFreeMode = FALSE;        // For Free mode
bool gfTestMode = FALSE;        // For Test mode
bool gfPeopleCount_EN = FALSE;
bool gfPeopleCount_EX = FALSE;
bool gfAISafetyOn = FALSE;

int gnPassPattern = PASS_PATN_NONE; // pass pattern at safety zone
int gnSignalCount = 0;

DWORD gdwPSensorError = 0;
DWORD mdwTickPSensorBlock[20] = {0,};

T_PASS_SEN_ERROR psenError;

T_GCU_PARAMETER gGCUParameter;
T_GCU_STATUS gGCUStatus;

T_GCU_OP_MODE gCurGCUOpMode;
T_GCU_OP_MODE gNewGCUOpMode;

B_BARRIER_STATUS gMainBarrierStatus;
B_BARRIER_STATUS gSubBarrierStatus;

bool gfMainBarrierRunning;
bool gfSubBarrierRunning;

/******* COPYRIGHT �� 2022 STraffic Co., Ltd.  ********END OF FILE****/
