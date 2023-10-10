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

#include "bsp.h"


/* for SDRAM */
/* Step 8: Set the refresh rate counter */
/* (15.62 us x Freq) - 20 */
/* Set the device refresh counter */
// refresh rate = refresh period / number of rows
// refresh rate = 64ms / 4096 = 15.62us
// refresh count = (refresh rate x SDRAM clock frequency) - 20
// refresh count = 15.62us x 25MHz - 20 = 369
// refresh count = 15.62us x 56MHz - 20 = 855
// refresh count = 15.62us x 42MHz - 20 = 636
// refresh count = 15.62us x 84MHz - 20 = 1292
// refresh count = 15.62us x 100MHz - 20 = 1542
// refresh count = 15.62us x 120MHz - 20 = 1874
// #define REFRESH_COUNT                               ((uint32_t)1874)

#define SDRAM_TIMEOUT                               ((uint32_t)0xFFFF)
#define SDRAM_MODEREG_BURST_LENGTH_1                ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2                ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4                ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8                ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL         ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED        ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2                 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3                 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD       ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE        ((uint16_t)0x0200)


extern SDRAM_HandleTypeDef hsdram1;


// SDRAM common clock: 2
// -> SDRAM clock: HCLK/2 = 240MHz/2 = 120MHz
// -> SDRAM clock period: 1/120MHz = 8.33ns

// Load mode register to active delay (TMRD): 2 clock
// Exit self-refresh delay (TXSR): 9 clock (9 x 8.33ns > 70ns)
// Self-refresh time (TRAS): 6 clock (6 x 8.33ns > 42ns)
// SDRAM common row cycle delay (TRC): 8 clock (8 x 8.33ns > 63ns)
// Write recovery time (TWR): 2 clock
// SDRAM common row precharge delay (TRP): 2 clock (2 x 8.33ns > 15ns)
// Row to column delay (TRCD): 2 clock (2 x 8.33ns > 15ns)
void SDRAM_Init(SDRAM_HandleTypeDef *hsdram)
{
    __IO FMC_SDRAM_CommandTypeDef command;

    /* Step 3:  Configure a clock configuration enable command */
    command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    command.AutoRefreshNumber = 1;
    command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, (FMC_SDRAM_CommandTypeDef*)&command, SDRAM_TIMEOUT);

    /* Step 4: Insert 100 ms delay */
    bsp_Delay(100);

    /* Step 5: Configure a PALL (precharge all) command */
    command.CommandMode = FMC_SDRAM_CMD_PALL;
    command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    command.AutoRefreshNumber = 1;
    command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, (FMC_SDRAM_CommandTypeDef*)&command, SDRAM_TIMEOUT);
    // HAL_Delay(1);

    /* Step 6 : Configure a Auto-Refresh command */
    command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    command.AutoRefreshNumber = 4;
    command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, (FMC_SDRAM_CommandTypeDef*)&command, SDRAM_TIMEOUT);
    // HAL_Delay(1);

    /* Step 7: Program the external memory mode register */
    command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    command.AutoRefreshNumber = 1;
    command.ModeRegisterDefinition = (uint32_t)(SDRAM_MODEREG_BURST_LENGTH_2 |
                                                SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                                                SDRAM_MODEREG_CAS_LATENCY_2 |
                                                SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                                SDRAM_MODEREG_WRITEBURST_MODE_SINGLE);

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, (FMC_SDRAM_CommandTypeDef*)&command, SDRAM_TIMEOUT);

    /* Step 8: Set the refresh rate counter */
    /* (15.62 us x Freq) - 20 */
    /* Set the device refresh counter */
    // refresh rate = refresh period / number of rows
    // refresh rate = 64ms / 4096 = 15.62us
    // refresh count = (refresh rate x SDRAM clock frequency) - 20
    // refresh count = 15.62us x 25MHz - 20 = 369
    // refresh count = 15.62us x 56MHz - 20 = 855
    // refresh count = 15.62us x 42MHz - 20 = 636
    // refresh count = 15.62us x 84MHz - 20 = 1292
    // refresh count = 15.62us x 100MHz - 20 = 1542
    // refresh count = 15.62us x 120MHz - 20 = 1842

    HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT);
}



void bsp_InitSdram(void)
{
    SDRAM_Init(&hsdram1);
}


void Test_Sdram(void)
{
    uint32_t addr;
    DWORD   dwStart, dwEnd;


    printf("SDRAM 8M size test....\n");
    dwStart = bsp_GetTickCount();

    for( addr = SDRAM_START_PTR; addr < SDRAM_START_PTR + SDRAM_SIZE; addr += 4 )
    {
        *(uint32_t *)addr = addr;
    }

    for( addr = SDRAM_START_PTR; addr < SDRAM_START_PTR + SDRAM_SIZE; addr += 4 )
    {
        if (*(uint32_t *)addr != addr)
    {
            break;
    }
    }

    dwEnd = bsp_GetTickCount();

    if( addr >= SDRAM_START_PTR + SDRAM_SIZE )
    {
        printf("SDRAM 8M size test OK. [Diff:%d]\n", dwEnd-dwStart);
    }
    else
    {
        printf("SDRAM 8M size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X [Diff:%d]\n",
                (unsigned int)addr, (unsigned int)*(uint32_t *)addr, (unsigned int)addr, dwEnd-dwStart);
    }
    printf("\n");
}

