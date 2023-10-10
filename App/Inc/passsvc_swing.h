/********************************************************************************
* File Name    : passsvc_swing.h
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : Definitions for GCU
*******************************************************************************/

#ifndef __PASSSVC_SWING_H_
#define __PASSSVC_SWING_H_

/* Functions prototypes ------------------------------------------------------*/
void OpenBarrierForSwing(BYTE bDir);
void InitPassageModeForSwing(void);
void PassageProcessForSwing(void);
void StopBarrierForSwing(bool isStop); 
void CloseBarrierForSwing(void);
void CheckBarrierOperation(void);
void ClearPassHistory(void);
void CheckEntranceSwing(void);
void CheckPassReadySwing(void);
void CheckPassingSwing(void);
void CheckCounterEnterTimer(void); 
void CheckIllegalEnterSwing(void);
void CheckTGTimerForSwing(void);
void CheckForceOpen(void);
void CheckBarrierStop(void);
void CheckSafetyTimerForSwing(void);
void CheckResetTimer(void);
void CheckLuggageWaitTimerSwing(void);
void CheckTailgateFilterTimer(void);
void CheckCloseWaitTimer(void);

#endif // __PASSSVC_SWING_H_
    /*****END OF FILE****/
