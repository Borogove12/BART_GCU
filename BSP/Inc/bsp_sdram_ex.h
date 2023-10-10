/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_sdram_ex.h
 *  모듈 설명    : 8M SDRAM(IS42S16400J) 제어 루틴 확장 헤더
 *                 STM32Cube Lib 참조
 *
 *  작성자       : 김정준
 *  작성일       : 2023/02/07
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_SDRAM_EX_H_
#define _BSP_SDRAM_EX_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "bsp_sdram.h"

#define IS42S16400J_TIMEOUT             ((uint32_t)0xFFFF)


typedef struct
{
  uint32_t TargetBank;           /*!< Target Bank                             */
  uint32_t RefreshMode;          /*!< Refresh Mode                            */
  uint32_t RefreshRate;          /*!< Refresh Rate                            */
  uint32_t BurstLength;          /*!< Burst Length                            */
  uint32_t BurstType;            /*!< Burst Type                              */
  uint32_t CASLatency;           /*!< CAS Latency                             */
  uint32_t OperationMode;        /*!< Operation Mode                          */
  uint32_t WriteBurstMode;       /*!< Write Burst Mode                        */
} IS42S16400J_Context_t;


#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(SDRAM_HandleTypeDef *);
  void (* pMspDeInitCb)(SDRAM_HandleTypeDef *);
} BSP_SDRAM_Cb_t;
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1) */



#define IS42S16400J_OK                (0)
#define IS42S16400J_ERROR             (-1)

#define BSP_SDRAM_IT_PRIORITY               15U


/* Register Mode */
#define IS42S16400J_BURST_LENGTH_1              0x00000000U
#define IS42S16400J_BURST_LENGTH_2              0x00000001U
#define IS42S16400J_BURST_LENGTH_4              0x00000002U
#define IS42S16400J_BURST_LENGTH_8              0x00000004U
#define IS42S16400J_BURST_TYPE_SEQUENTIAL       0x00000000U
#define IS42S16400J_BURST_TYPE_INTERLEAVED      0x00000008U
#define IS42S16400J_CAS_LATENCY_2               0x00000020U
#define IS42S16400J_CAS_LATENCY_3               0x00000030U
#define IS42S16400J_OPERATING_MODE_STANDARD     0x00000000U
#define IS42S16400J_WRITEBURST_MODE_PROGRAMMED  0x00000000U
#define IS42S16400J_WRITEBURST_MODE_SINGLE      0x00000200U

/* Command Mode */
#define IS42S16400J_NORMAL_MODE_CMD             0x00000000U
#define IS42S16400J_CLK_ENABLE_CMD              0x00000001U
#define IS42S16400J_PALL_CMD                    0x00000002U
#define IS42S16400J_AUTOREFRESH_MODE_CMD        0x00000003U
#define IS42S16400J_LOAD_MODE_CMD               0x00000004U
#define IS42S16400J_SELFREFRESH_MODE_CMD        0x00000005U
#define IS42S16400J_POWERDOWN_MODE_CMD          0x00000006U


#define SDRAM_INSTANCES_NBR                1U
#define SDRAM_DEVICE_ADDR                  0xD0000000U
#define SDRAM_DEVICE_SIZE                  0x02000000U  /* 32 MBytes */
#define SDRAM_MEMORY_WIDTH                 FMC_SDRAM_MEM_BUS_WIDTH_32

/* DMA definitions for SDRAM DMA transfer */
#define SDRAM_MDMAx_CLK_ENABLE             __HAL_RCC_MDMA_CLK_ENABLE
#define SDRAM_MDMAx_CLK_DISABLE            __HAL_RCC_MDMA_CLK_DISABLE
#define SDRAM_MDMAx_CHANNEL                MDMA_Channel0
#define SDRAM_MDMAx_IRQn                   MDMA_IRQn
#define SDRAM_MDMA_IRQHandler              MDMA_IRQHandler

extern SDRAM_HandleTypeDef hsdram[];

/* Local Device Function
 *
 * This file provides a set of functions needed to drive the IS42S16400J SDRAM memory.
 */
int32_t IS42S16400J_Init(SDRAM_HandleTypeDef *Ctx, IS42S16400J_Context_t *pRegMode);
int32_t IS42S16400J_ClockEnable(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
int32_t IS42S16400J_Precharge(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
int32_t IS42S16400J_ModeRegConfig(SDRAM_HandleTypeDef *Ctx, IS42S16400J_Context_t *pRegMode);
int32_t IS42S16400J_TimingConfig(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_TimingTypeDef *pTiming);
int32_t IS42S16400J_RefreshMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface, uint32_t RefreshMode);
int32_t IS42S16400J_RefreshRate(SDRAM_HandleTypeDef *Ctx, uint32_t RefreshCount);
int32_t IS42S16400J_EnterPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
int32_t IS42S16400J_ExitPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface);
int32_t IS42S16400J_Sendcmd(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_CommandTypeDef *SdramCmd);

int32_t BSP_SDRAM_Init(uint32_t Instance);
int32_t BSP_SDRAM_DeInit(uint32_t Instance);
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
int32_t BSP_SDRAM_RegisterDefaultMspCallbacks (uint32_t Instance)
int32_t BSP_SDRAM_RegisterMspCallbacks (uint32_t Instance, BSP_SDRAM_Cb_t *CallBacks)
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)  */
int32_t BSP_SDRAM_SendCmd(uint32_t Instance, FMC_SDRAM_CommandTypeDef *SdramCmd);
void BSP_SDRAM_IRQHandler(uint32_t Instance);
/* These functions can be modified in case the current settings need to be
changed for specific application needs */
HAL_StatusTypeDef MX_SDRAM_BANK2_Init(SDRAM_HandleTypeDef *hsdram);


#ifdef __cplusplus
}
#endif

#endif /* _BSP_SDRAM_EX_H_ */
