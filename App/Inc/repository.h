/*******************************************************************************
*       COPYRIGHT �� 2023 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : repository.h
* Author       : Joseph Yosep Choi
* Date Created : 2023/09/14
* Description  : Definition of global repository
*******************************************************************************/

#ifndef __REPOSITORY_H_
#define __REPOSITORY_H_

#include "gcu_def.h"
#include "barrier.h"
#include "userlib.h"

/* Global variables ----------------------------------------------------------*/
extern BYTE gbPrevDir;
extern BYTE gbBarrierCmd;

extern bool gisEntryPassenger;
extern bool gisExitPassenger;
extern bool gfTailgating;
extern bool gfFreeMode;
extern bool gfTestMode;
extern bool gfPeopleCount_EN;
extern bool gfPeopleCount_EX;
extern bool gfAISafetyOn;

extern int gnPassPattern;
extern int gnSignalCount;

extern DWORD gdwPSensorError;
extern DWORD mdwTickPSensorBlock[20];

extern T_PASS_SEN_ERROR psenError;

extern T_GCU_PARAMETER gGCUParameter;
extern T_GCU_STATUS gGCUStatus;

extern T_GCU_OP_MODE gCurGCUOpMode;
extern T_GCU_OP_MODE gNewGCUOpMode;

extern B_BARRIER_STATUS gMainBarrierStatus;
extern B_BARRIER_STATUS gSubBarrierStatus;

#endif  // __REPOSITORY_H_
/******* COPYRIGHT �� 2022 STraffic Co., Ltd.  ********END OF FILE****/
