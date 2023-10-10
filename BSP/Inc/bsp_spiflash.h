/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_spiflash.h
 *  모듈 설명    : SPI Flash Data Memory 처리 루틴 헤더
 *                 AT25DF641 - 8M Bytes SPI Flash
 *
 *  작성자       : 김정준
 *  작성일       : 2022/11/10 -
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */
#ifndef _BSP_SPIFLASH_H_
#define _BSP_SPIFLASH_H_

#include "bsp_types.h"

#define BANKSIZ             0x800000    /* 8,388,608(8M)Bytes(BLOCKSIZ x BLOCKS_PER_BANK) */
#define BLOCKS_PER_BANK     2048        /* Block No */
#define BLOCKSIZ            0x1000      /* 4096(4K)Bytes (PAGESIZ x PAGES_PER_BLOCK) Erase Unit */
#define PAGES_PER_BLOCK     16
#define PAGESIZ             256         /* 256 Bytes */
#define HALFPAGESIZ         128
#define ID_AT25DF641        0x1F4800



// status register format - byte1 define
#define bSPRL   7   // (R/W) Sector Protection Registers Locked
                    // 0 Sector Protection Registers are unlocked (default).
                    // 1 Sector Protection Registers are locked.
#define bEPE    5   // (R) Erase/Program Error
                    // 0 Erase or program operation was successful.
                    // 1 Erase or program error detected.
#define bWPP    4   // (R) Write Protect(/WP) Pin Status
                    // 0 WP is asserted.
                    // 1 WP is deasserted.
#define bSWP1   3   // (R) Software Protection Status
#define bSWP0   2   // (R) Software Protection Status
                    //  00  All sectors are software unprotected
                    //  01  Some sectors are software protected.
                    //  10  Reserved for future use.
                    //  11  All sectors are software protected
#define bWEL    1   // (R) Write Enable Latch Status
                    // 0 Device is not write enabled (default).
                    // 1 Device is write enabled.
#define bBUSY   0   // (R) Ready/Busy Status
                    // 0 Device is ready.
                    // 1 Device is busy with an internal operation.

// status register format - byte2 define
#define bRSTE   4   // (R/W) Reset Enabled
#define bSLE    3   // (R/W) Sectoe Lockdown Enabled
#define bPS     2   // (R) Program Suspend Status
#define bES     1   // (R) Erase Suspend Status
#define bRBS    0   // (R) Ready/Busy Status

/*  return error code   */
#define E_ADDR  1   /* address error    */
#define E_TIME  2   /* timeout error    */
#define E_FAIL  3   /* failure error    */
#define E_VERF  4   /* verify error     */
#define E_WP    5   /* write protect    */



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void SpiFlash_ReadPage(uint32_t addr, BYTE* Buf, UINT nLen);
void SpiFlash_WritePage(DWORD addr, BYTE* Buf, UINT nLen);
void SpiFlash_EraseBlock4K(DWORD startAddr);
void SpiFlash_GlobalUnprotectSector(void);
BOOL SpiFlash_Ready(DWORD dwTimeout);
void SpiFlash_WaitReady(void);
void SpiFlash_WaitStatus(BYTE bMask);
DWORD SpiFlash_ReadInfo(void);
void SpiFlash_WriteEnable(void);
void SpiFlash_WriteDisable(void);
WORD SpiFlash_ReadStatusRegister(void);

int  SpiFlash_ReadBuffer(DWORD beginAddr, BYTE* Buf, UINT nLen);
BOOL SpiFlash_WriteBuffer(DWORD beginAddr, BYTE* Buf, UINT nLen);



int SpiFlash_ReadStatus(void);
void SpiFlash_ResetAndResume(void);
void SpiFlash_ChipErase(void);
int SpiFlash_Erase(DWORD beginAddr, DWORD toErase);

BOOL SpiFlash_Init(void);

BOOL bsp_InitSpiFlash(void);

void Test_SpiFlash(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // _BSP_SPIFLASH_H_
