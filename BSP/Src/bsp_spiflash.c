/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_spiflash.c
 *  모듈 설명    : SPI Flash Data Memory 처리 루틴
 *                 AT25DF641 - 8M Bytes SPI Flash
 *
 *  작성자       : 김정준
 *  작성일       : 2022/12/01 -
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#include "bsp.h"

extern SPI_HandleTypeDef hspi5;


#define SF_HANDLE       &hspi5
#define SF_ENABLE_PORT  GPIOF
#define SF_ENABLE_PIN   GPIO_PIN_6

#define SF_ENABLE()     HAL_GPIO_WritePin(SF_ENABLE_PORT, SF_ENABLE_PIN, GPIO_PIN_RESET);
#define SF_DISABLE()    HAL_GPIO_WritePin(SF_ENABLE_PORT, SF_ENABLE_PIN, GPIO_PIN_SET);

#define AT25DF_DUMMY_BYTE           0x00
// Read Copmmands
#define AT25DF_READ_ARRAY_FAST      0x1B    // up to 100Mhz
#define AT25DF_READ_ARRAY           0x0B    // up to 85Mhz
#define AT25DF_READ_ARRAY_LOW       0x03    // up to 40Mhz
#define AT25DF_READ_ARRAY_DUAL      0x3B    // up to 65Mhz
// Program and Erase Commands
#define AT25DF_BLOCK_ERASE_4KB      0x20    // Block Erase (4KB)
#define AT25DF_BLOCK_ERASE_32KB     0x52    // Block Erase (32KB)
#define AT25DF_BLOCK_ERASE_64KB     0xD8    // Block Erase (64KB)
#define AT25DF_CHIP_ERASE           0x60    // Chip Erase (or 0xC7)
#define AT25DF_BYTE_PROGRAM         0x02    // Byte/Page Program (1 to 256 bytes : 0x1 ~ 0x100)
#define AT25DF_BYTE_PROGRAM_DUAL    0xA2    // Dual-Input Byte/Page Program (1 to 256 Bytes)
#define AT25DF_PROGRAM_SUSPEND      0xB0    // Program/Erase Suspend
#define AT25DF_PROGRAM_RESUME       0xD0    // Program/Erase Resume
// Protection Commands
#define AT25DF_WRITE_ENABLE         0x06    // Write Enable
#define AT25DF_WRITE_DISABLE        0x04    // Write Disable
#define AT25DF_PROTECT_SECTOR       0x36    // Protect Sector
#define AT25DF_UNPROTECT_SECTOR     0x39    // Unprotect Sector
#define AT25DF_RDSPR                0x3C    // Read Sector Protection Registers
// Security Commands - Not Used

// Status Register Commands
#define AT25DF_READ_STATUS          0x05    // Read Status Register
#define AT25DF_WRITE_STATUS_BYTE1   0x01    // Write Status Register Byte 1
#define AT25DF_WRITE_STATUS_BYTE2   0x31    // Write Status Register Byte 2
// Miscellaneous Commands
#define AT25DF_RESET                0xF0    // Reset
#define AT25DF_READ_INFO            0x9F    // Read Manufacturer and Device ID
#define AT25DF_DEEP_POWER_DOWN      0xB9    // Deep Power-Down
#define AT25DF_RESUME_POWER_DOWN    0xAB    // Resume from Deep Power-Down


#define AT25DF_STATUS_DONE_MASK     0x01

DWORD g_FlashId;

static BYTE s_BlockBuf[BLOCKSIZ];
static BYTE s_PageBuf[PAGESIZ];

BOOL g_fDumpSF = TRUE;



uint8_t SF_RW_Byte(BYTE data)
{
    BYTE read;
    HAL_SPI_TransmitReceive(SF_HANDLE, &data, &read, 1, 100);
    return read;
}


void SF_CommandAndReadN(BYTE cmd, BYTE* buffer, int n)
{
    uint8_t i;

    SF_ENABLE();

    SF_RW_Byte(cmd);
    for (i = 0; i < n; i++)
        buffer[i] = SF_RW_Byte(AT25DF_DUMMY_BYTE);

    SF_DISABLE();
}


void SF_CommandAndWriteN(BYTE cmd, BYTE* data, int n)
{
    uint8_t i;

    SF_ENABLE();

    SF_RW_Byte(cmd);
    for (i = 0; i < n; i++)
        SF_RW_Byte(data[i]);

    SF_DISABLE();
}


void SF_Command(BYTE cmd)
{
    SF_CommandAndReadN(cmd, 0, 0);
}


//static BYTE SF_ReadByte(uint32_t addr)
//{
//    uint8_t rByte;
//
//    SF_ENABLE();
//
//    SF_RW_Byte(AT25DF_READ_ARRAY);
//
//    SF_RW_Byte((uint8_t)((addr & 0xff0000) >> 16));
//    SF_RW_Byte((uint8_t)((addr & 0xff00) >> 8));
//    SF_RW_Byte((uint8_t)(addr & 0xff));
//
//    rByte =  SF_RW_Byte(0xff);
//
//    SF_DISABLE();
//
//    return (rByte);
//}




void SF_ChipErase(void)
{
    SF_ENABLE();
    SF_RW_Byte(AT25DF_CHIP_ERASE);
    SF_DISABLE();
}


void SF_EraseBlock4K(DWORD startAddr)
{
    SF_ENABLE();

    SF_RW_Byte(AT25DF_BLOCK_ERASE_4KB);
    SF_RW_Byte((uint8_t)((startAddr%0xff0000) >> 16));
    SF_RW_Byte((uint8_t)((startAddr%0xff00) >> 8));
    SF_RW_Byte((uint8_t)(startAddr&0xff));

    SF_DISABLE();
}


void SpiFlash_ResetAndResume(void)
{
    SF_ENABLE();

    SF_RW_Byte(AT25DF_RESET);
    SF_RW_Byte(AT25DF_PROGRAM_RESUME);

    SF_DISABLE();
}


void SpiFlash_WriteEnable(void)
{
    SF_ENABLE();
    SF_RW_Byte(AT25DF_WRITE_ENABLE);
    SF_DISABLE();
}


void SpiFlash_WriteDisable(void)
{
    SF_ENABLE();
    SF_RW_Byte(AT25DF_WRITE_DISABLE);
    SF_DISABLE();
}


void SpiFlash_GlobalUnprotectSector(void)
{
    SF_ENABLE();

    SF_RW_Byte(AT25DF_WRITE_STATUS_BYTE1);
    SF_RW_Byte(0x00);

    SF_DISABLE();
}



WORD SpiFlash_ReadStatusRegister(void)
{
    BYTE status1, status2;

    SF_ENABLE();

    SF_RW_Byte(AT25DF_READ_STATUS);
    status1 = SF_RW_Byte(0xFF);
    status2 = SF_RW_Byte(0xFF);

    SF_DISABLE();

    // printf("ReadStatusRegister - [Status1:%02X Status2:%02X]\n", status1, status2);

    return ((status2 << 8) | status1);
}


BOOL SpiFlash_Ready(DWORD dwTimeout)
{
    BOOL  fTimeout = TRUE;
    DWORD   dwCurTick = bsp_GetTickCount();
    BYTE    status;

    SF_ENABLE();
    SF_RW_Byte(AT25DF_READ_STATUS);
    do
    {
        status = SF_RW_Byte(0xFF);
        if (!(status & 0x01))   // device is ready
        {
            // printf("SPI Flash : Status[%02X]\n", status);
            fTimeout = FALSE;
            break;
        }

    } while (!IsTimeoutMs(&dwCurTick, dwTimeout));
    SF_DISABLE();

    return (fTimeout == FALSE);
}


void SpiFlash_WaitReady(void)
{
    int     nCnt = 0;
    BYTE    status;

    SF_ENABLE();

    SF_RW_Byte(AT25DF_READ_STATUS);
    do
    {
        status = SF_RW_Byte(0xFF);
        nCnt++;
    } while (status & 0x01);

    SF_DISABLE();

    // printf("SFlash is ready - [Status:%02X, Cnt:%d]\n", status, nCnt);
}


void SpiFlash_WaitStatus(BYTE bMask)
{
    int     nCnt = 0;
    WORD    status;

    do
    {
        status = SpiFlash_ReadStatusRegister();
        nCnt++;
    } while ((status & bMask) != bMask);

    // printf("SpiFlash_WaitStatus - [Mask:%02X Status:%04X, Cnt:%d]\n", bMask, status, nCnt);
}


void SpiFlash_WaitSameStatus(WORD wComp)
{
    int     nCnt = 0;
    WORD    status;

    do
    {
        status = SpiFlash_ReadStatusRegister();
        nCnt++;
    } while (status != wComp);

    // printf("SpiFlash_WaitSameStatus - [Mask:%04X Status:%04X, Cnt:%d]\n", wComp, status, nCnt);
}



DWORD SpiFlash_ReadInfo(void)
{
    BYTE    bInfo[4];
    DWORD   retInfo;

    SF_CommandAndReadN(AT25DF_READ_INFO, bInfo, 3);

    retInfo = ((bInfo[0] << 16) | (bInfo[1] << 8) | bInfo[2]);

    printf("SpiFlash Info %#lx\n", retInfo);

    g_FlashId = retInfo;

    return (retInfo);
}


BOOL SpiFlash_Init(void)
{
    DWORD info;

    // SpiFlash_ResetAndResume();
    SpiFlash_WriteEnable();
    SpiFlash_GlobalUnprotectSector();
    SpiFlash_Ready(5);

    info = SpiFlash_ReadInfo();
    if (info == ID_AT25DF641)
    {
        printf("SPI Flash is normal\n");
        return (TRUE);
    }
    else
    {
        printf("SPI Flash is Abnormal\n");
        return (FALSE);
    }
}


/*
   Ref: Datasheet AT25DF641A page 35

   bit 7 : SPRL(Sector Protection Registers Locked)
   bit 6 : RES(Reserved for future use)
   bit 5 : EPE(Erase/Program Error)
   bit 4 : WPP(Write Protect(#WP) Pin Status)
   bit 3~2 : SWP(Software Protection Status)
   bit 1 : WEL(Write Enable Latch Status)
   bit 0 : RDY/BSY(Ready/Busy Status)
 */
int SpiFlash_ReadStatus(void)
{
    BYTE Status[2];

    SF_CommandAndReadN(AT25DF_READ_STATUS, Status, 2);

    return (Status[0]);
}



int SpiFlash_ReadBuffer(DWORD beginAddr, BYTE* Buf, UINT nLen)
{
    UINT   n;
    BYTE    Offset;
    BYTE    *pb = Buf;
    int     opRead, nTotRead = 0;

    if ((beginAddr+nLen) > BANKSIZ)   // check range validity
    {
        printf("Flash Read(a:0x%#lX, s:%ld) - invalid range\n", beginAddr, nLen);
        return (-1);
    }

    opRead = nLen;
    while (opRead > 0)
    {
        Offset = beginAddr % PAGESIZ;   // offset addr in the bank
        n = PAGESIZ - Offset;
        n = min(n, opRead);
        SpiFlash_ReadPage(beginAddr, pb, n);
        beginAddr += n;
        opRead -= n;
        pb += n;
        nTotRead += n;
    }

    return (nTotRead);
}



BOOL SpiFlash_WriteBuffer(DWORD beginAddr, BYTE* Buf, UINT nLen)
{
    DWORD addr = beginAddr;
    DWORD realWrite, retry = 0;
    BYTE* pb = Buf;
    INT opSize;

    if ((beginAddr+nLen) > BANKSIZ)   // check range validity
    {
        printf("Flash Write(a:0x%#lX, s:%ld) - invalid range\n", beginAddr, nLen);
        return(FALSE);
    }

    opSize = nLen;
    while (opSize > 0)
    {
        if ((addr%BLOCKSIZ) == 0)
        {
            SpiFlash_EraseBlock4K(addr);
        }

        realWrite = PAGESIZ - (addr % PAGESIZ);
        realWrite = min(realWrite, opSize);

        SpiFlash_WritePage(addr, pb,  realWrite);

        SpiFlash_ReadPage(addr, s_PageBuf, realWrite);
        if (memcmp(pb, s_PageBuf, realWrite) != 0)
        {
            retry++;
            printf("Flash WriterPage Error[addr:0x%lX, size:%ld, retry:%ld]\n", addr, realWrite, retry);
            printf("Write Buf\n");
            hexdump(addr, pb, realWrite);
            printf("Read Buf\n");
            hexdump(addr, s_PageBuf, realWrite);
            if (retry <= 3)
            {
                HAL_Delay(2);
                continue;
            }
            else
            {
                return FALSE;
            }
        }

        retry = 0;
        addr += realWrite;
        pb += realWrite;
        opSize -= realWrite;
    }

    return TRUE;
}


void SpiFlash_ChipErase(void)
{
    SpiFlash_WriteEnable();
    SF_ChipErase();
    printf("Waiting... Chip erase time is 36 ~ 56 sec\n");
    SpiFlash_Ready(57000); // Chip Erase Time : Typ(36 sec), Max(56 sec)
    printf("Complete Chip erase \n");
}


void SpiFlash_EraseBlock4K(DWORD startAddr)
{
    SpiFlash_WriteEnable();
    SpiFlash_WaitSameStatus(0x12);  // Wait Write Enable

    SF_EraseBlock4K(startAddr);

    SpiFlash_WaitSameStatus(0x10);      // Wait Complete Erase
    SpiFlash_WriteDisable();

    printf("SpiFlash : Erase 4k Complete[Addr:%lx]\n", startAddr);
}


int SpiFlash_Erase(DWORD beginAddr, DWORD toErase)
{
    DWORD   realAddr;
    int     realSize;

    if (beginAddr == 0 && toErase == 0)     // full domain erase
    {
        SpiFlash_ChipErase();
        return (0);
    }

    if ((beginAddr+toErase) > BANKSIZ)  // check range validity
    {
        printf("Flash_Erase(a:0x%lx, s:%ld) - invalid range\n", beginAddr, toErase);
        return(E_ADDR);
    }

    // adjust addr to the start of block.
    realAddr = beginAddr - (beginAddr % BLOCKSIZ);
    realSize = toErase + (beginAddr % BLOCKSIZ);

    while (realSize > 0)
    {
        SpiFlash_EraseBlock4K(realAddr);
        realAddr += BLOCKSIZ;
        realSize -= BLOCKSIZ;
    }

    return (0);
}


BOOL bsp_InitSpiFlash(void)
{
    SF_DISABLE();

    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi5.Init.NSS = SPI_NSS_SOFT;
    // hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;     // 60Mhz
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 0x0;
    hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi5.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi5.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_02CYCLE;
    hspi5.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_02CYCLE;
    hspi5.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_ENABLE;
    hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    if (HAL_SPI_Init(&hspi5) != HAL_OK)
    {
        return FALSE;
    }

    return (SpiFlash_Init());
}


void SpiFlash_ReadPage(uint32_t addr, BYTE* Buf, UINT nLen)
{
    BYTE addr1, addr2, addr3;
    UINT offset = (addr%PAGESIZ);

    if (offset > 0)
    {
        nLen = PAGESIZ - offset;
    }
    addr1 = (BYTE)((addr&0xff0000) >> 16);
    addr2 = (BYTE)((addr&0xff00) >> 8);
    addr3 = (BYTE)(addr&0xff);

    SF_ENABLE();

    SF_RW_Byte(AT25DF_READ_ARRAY_LOW);      // 1B - Dummy 2, 0B - Dummy 1, 03 - Dummy 0
    SF_RW_Byte(addr1);
    SF_RW_Byte(addr2);
    SF_RW_Byte(addr3);
    // SF_RW_Byte(0x00);    // Dummy
    HAL_SPI_Receive(SF_HANDLE, Buf, nLen, 100);
    // HAL_SPI_TransmitReceive(SF_HANDLE, Tx, Rx, dwToRead, 100);

    SF_DISABLE();

//    if (g_fDumpSF)
//    {
//        printf("SpiFlash_ReadPage() - [addr:%lx(%02X%02X%02X), size:%ld]\n", addr, addr1, addr2, addr3, nLen);
//        hexdump(addr, Buf, nLen);
//    }
}


void SpiFlash_WritePage(DWORD addr, BYTE* Buf, UINT nLen)
{
    BYTE addr1, addr2, addr3;
    UINT offset = (addr%PAGESIZ);

    if (offset > 0)
    {
        nLen = PAGESIZ - offset;
    }
    addr1 = (BYTE)((addr&0xff0000) >> 16);
    addr2 = (BYTE)((addr&0xff00) >> 8);
    addr3 = (BYTE)(addr&0xff);

    SpiFlash_WriteEnable();
    SpiFlash_WaitSameStatus(0x12);

    SF_ENABLE();

    SF_RW_Byte(AT25DF_BYTE_PROGRAM);
    SF_RW_Byte(addr1);
    SF_RW_Byte(addr2);
    SF_RW_Byte(addr3);
    HAL_SPI_Transmit(SF_HANDLE, Buf, nLen, 100);
    // HAL_SPI_TransmitReceive(SF_HANDLE, Buf, DummyRx, writeN, 100);

    SF_DISABLE();

    SpiFlash_WaitSameStatus(0x10);  // Page Program Time 256byte : Typ(1.0 ms), Max(3.0 ms), Ref(Nand Flash  200~500 μs)
    SpiFlash_WriteDisable();

//    if (g_fDumpSF)
//    {
//        printf("SpiFlash_WritePage() - [addr:%lx(%02X%02X%02X), size:%ld]\n", addr, addr1, addr2, addr3, nLen);
//        hexdump(addr, Buf, PAGESIZ);
//    }
}


void Test_SpiFlash(void)
{
    BYTE    PageBuf[PAGESIZ];
    BYTE    wBlockBuf[BLOCKSIZ], rBlockBuf[BLOCKSIZ];
    DWORD   BlockNo, PageNo, Addr = 0x00;

    DWORD sfInfo = SpiFlash_ReadInfo();
    printf("SPI Flash Info : %lX\n", sfInfo);

    for (int n = 2; n < PAGESIZ; n++)
    {
        PageBuf[n] = 0xFF - n;
    }

    for (BlockNo = 0; BlockNo < 1; BlockNo++)
    {
        for (PageNo = 0; PageNo < 16; PageNo++)
        {
            PageBuf[0] = BlockNo + 1;
            PageBuf[1] = PageNo + 1;
            memcpy(&wBlockBuf[PageNo*PAGESIZ], PageBuf, PAGESIZ);
        }
        // SpiFlash_WriteBuffer(Addr, wBlockBuf, BLOCKSIZ);
        Addr += BLOCKSIZ;
    }

    SpiFlash_Ready(100);
    Addr = 0x00;
    for (BlockNo = 0; BlockNo < 10; BlockNo++)
    {
        SpiFlash_ReadBuffer(Addr, rBlockBuf, BLOCKSIZ);
        printf("Addr : %08lX\n", Addr);
        for (int n = 0; n < BLOCKSIZ; n++)
        {
            printf("%02X ", *(rBlockBuf+n));
            if ( n % 32 == 31)
                printf("\n");
        }
        printf("\n");
        Addr += BLOCKSIZ;
    }

}



