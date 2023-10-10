/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_sdram.h
 *  모듈 설명    : 8M SDRAM(IS42S16400J-7BLI) 제어 루틴 헤더
 *
 *  작성자       : 김정준
 *  작성일       : 2022/08/28
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_SDRAM_H_
#define _BSP_SDRAM_H_
#include "bsp.h"
#include "bsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define SDRAM_START_PTR     0xD0000000
#define SDRAM_SIZE          0x800000        // 8 * 1024 * 1024

#define SDRAM_BANK1_ADDR    0xD0000000
#define SDRAM_BANK2_ADDR    0xD0200000
#define SDRAM_BANK3_ADDR    0xD0400000
#define SDRAM_BANK4_ADDR    0xD0800000

#define REFRESH_COUNT                               ((uint32_t)1874)


void bsp_InitSdram(void);
void SDRAM_Init(SDRAM_HandleTypeDef *hsdram);
void Test_Sdram(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _BSP_SDRAM_H_ */
