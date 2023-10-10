
#ifndef _BSP_H_
#define _BSP_H_

#include "bsp_config.h"
#include "bsp_macro.h"
#include "bsp_types.h"
#include "bsp_errno.h"

/* 주석을 제거하거나 추가하여 기본 드라이버 모듈을 포함할지 여부를 제어합니다. */
#include "bsp_util.h"
#include "bsp_uart.h"
#include "bsp_iuart.h"
#include "bsp_timer.h"
#include "bsp_rtc.h"
#include "bsp_sdram.h"
#include "bsp_sdram_ex.h"
#include "bsp_nand.h"
#include "bsp_spiflash.h"
#include "bsp_fmc.h"

void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);
void System_Halt(char *file, uint32_t line);

#endif	// _BSP_H_
