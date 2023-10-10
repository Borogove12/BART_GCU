/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_nand.h
 *  모듈 설명    : NAND Flash의 저수준 인터페이스 기능
 *
 *  작성자       : 김정준
 *  작성일       : 2022/09/10
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#include "bsp.h"

#define Bank_NAND_ADDR     ((uint32_t)0x80000000UL)

/* NAND 플래시를 작동하는 데 사용되는 3개의 매크로 정의 */
#define NAND_CMD_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA)
#define NAND_ADDR_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA)
#define NAND_DATA_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA)


#define BUSY_GPIO   GPIOC
#define BUSY_PIN    GPIO_PIN_6

/* 기다리지 않으면 읽은 데이터가 비정상적이며 여기서 타임아웃을 판단해야 함*/
#define WAIT_BUSY() {   \
    uint16_t k;     \
    for (k = 0; k < 200; k++)   \
    {   \
        if ((BUSY_GPIO->IDR & BUSY_PIN) == 0) break;    \
    }   \
    for (k = 0; k < 2000; k++)  \
    {   \
        if ((BUSY_GPIO->IDR & BUSY_PIN) != 0) break;    \
    }   \
}



extern NAND_HandleTypeDef hnand1;

/*
**  지정한 블록이 불량인지 여부를 감지합니다.
**
*/
BOOLEAN Nand_IsBadBlock(uint16_t _usBlockNo)
{
    uint8_t buf[VALID_SPARE_SIZE];
    NAND_AddressTypeDef RWAddr;

    RWAddr.Plane = 0;
    RWAddr.Block = _usBlockNo;
    RWAddr.Page = 0;

    HAL_NAND_Read_SpareArea(&hnand1, &RWAddr, buf, 1);
    if (buf[BI_OFFSET] != 0xFF)
    {
        return TRUE;
    }

    RWAddr.Page = 1;
    HAL_NAND_Read_SpareArea(&hnand1, &RWAddr, buf, 1);
    if (buf[BI_OFFSET] != 0xFF)
    {
        return TRUE;
    }

    return FALSE;   // Good Block
}


uint32_t Nand_ReadID(void)
{
    __IO uint32_t data = 0;
    uint32_t deviceaddress;

	/* Identify the device address */
	deviceaddress = NAND_DEVICE;

    /* Send Read ID command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_READID;
    __DSB();
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;
    __DSB();

    data = *(__IO uint32_t *)deviceaddress;

	return data;
}



uint32_t Nand_ReadStatus(void)
{
    uint32_t data;
    uint32_t deviceaddress;

    /* Identify the device address */
    deviceaddress = NAND_DEVICE;

    /* Send Read status operation command */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_STATUS;

    /* Read status register data */
    data = *(__IO uint8_t *)deviceaddress;

    /* Return the status */
    if ((data & NAND_ERROR) == NAND_ERROR)
    {
        return NAND_ERROR;
    }
    else if ((data & NAND_READY) == NAND_READY)
    {
        return NAND_READY;
    }
    else
    {
        return NAND_BUSY;
    }
}


// 일정시간동안 READY될때까지 기다림.
uint32_t Nand_GetStatus(void)
{
    uint32_t ulTimeout = 0x20000;
    uint32_t Status = NAND_READY;

    Status = Nand_ReadStatus();

    while ((Status != NAND_READY) &&( ulTimeout != 0x00))
    {
        Status = Nand_ReadStatus();
        ulTimeout--;
    }

    if (ulTimeout == 0x00)
    {
        Status =  NAND_TIMEOUT_ERROR;
    }

    return (Status);
}



uint8_t Nand_Reset(void)
{
    uint32_t deviceaddress;

    /* Identify the device address */
    deviceaddress = NAND_DEVICE;

    /* Send Read status operation command */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_RESET;

    if (Nand_GetStatus() == NAND_READY)
    {
        return NAND_OK;
    }

    return NAND_FAIL;
}


/*
**  NandFlash의 지정된 페이지의 지정된 위치의 데이터를 읽어온다.
**
**  pBuffer: 읽어올 데이터를 보관할 버퍼(크기2048)
**  ulPageNo: 페이지 번호(0-65535)
*/
uint8_t Nand_ReadPage(uint8_t *pBuffer, uint32_t ulPageNo)
{
    HAL_StatusTypeDef halStatus;
    NAND_AddressTypeDef addr;
    uint32_t nandStatus;

    addr.Plane = 0;
    addr.Block = ulPageNo / NAND_BLOCK_SIZE;
    addr.Page = ulPageNo % NAND_BLOCK_SIZE;;

    halStatus = HAL_NAND_Read_Page_8b(&hnand1, &addr, pBuffer, 1);
    if (halStatus != HAL_OK)
    {
        return NAND_FAIL;
    }

    nandStatus = Nand_GetStatus();
    if (nandStatus != NAND_READY)
        return NAND_FAIL;

    return NAND_OK;
}


/*
**  NandFlash의 지정된 페이지 Spare 영역의 데이터를 읽어온다.
**
**  pBuffer: 읽어올 데이터를 보관할 버퍼(크기64)
**  ulPageNo: 페이지 번호(0-65535)
*/
uint8_t Nand_ReadSparePage(uint8_t *pBuffer, uint32_t ulPageNo)
{
    HAL_StatusTypeDef halStatus;
    NAND_AddressTypeDef addr;
    uint32_t nandStatus;

    addr.Plane = 0;
    addr.Block = ulPageNo / NAND_BLOCK_SIZE;
    addr.Page = ulPageNo % NAND_BLOCK_SIZE;;

    halStatus = HAL_NAND_Read_SpareArea_8b(&hnand1, &addr, pBuffer, 1);
    if (halStatus != HAL_OK)
    {
        return NAND_FAIL;
    }

    nandStatus = Nand_GetStatus();
    if (nandStatus != NAND_READY)
        return NAND_FAIL;


    return NAND_OK;
}


/*
**  NandFlash의 지정된 페이지의 지정된 위치에 데이터를 기록한다.
**
**  pBuffer: 기록할 데이터를 보관할 버퍼
**  ulPageNo: 페이지 번호(0-65535)
*/
uint8_t Nand_WritePage(uint8_t *pBuffer, uint32_t ulPageNo)
{
    HAL_StatusTypeDef halStatus;
    NAND_AddressTypeDef addr;
    uint32_t nandStatus;

    addr.Plane = 0;
    addr.Block = ulPageNo / NAND_BLOCK_SIZE;
    addr.Page = ulPageNo % NAND_BLOCK_SIZE;;

    halStatus = HAL_NAND_Write_Page_8b(&hnand1, &addr, pBuffer, 1);
    if (halStatus != HAL_OK)
    {
        return NAND_FAIL;
    }

    nandStatus = Nand_GetStatus();
    if (nandStatus != NAND_READY)
        return NAND_FAIL;

    return NAND_OK;
}


uint8_t Nand_WriteSparePage(uint8_t *pBuffer, uint32_t ulPageNo)
{
    HAL_StatusTypeDef halStatus;
    NAND_AddressTypeDef addr;
    uint32_t nandStatus;

    addr.Plane = 0;
    addr.Block = ulPageNo / NAND_BLOCK_SIZE;
    addr.Page = ulPageNo % NAND_BLOCK_SIZE;;

    halStatus = HAL_NAND_Read_SpareArea_8b(&hnand1, &addr, pBuffer, 1);
    if (halStatus != HAL_OK)
    {
        return NAND_FAIL;
    }

    nandStatus = Nand_GetStatus();
    if (nandStatus != NAND_READY)
        return NAND_FAIL;

    return NAND_OK;
}


/*
**  NandFlash의 지정된 Block을 삭제한다.
**
**  ulBlockNo: 삭제할 블럭 번호(0 - 1023)
**
*/
uint32_t Nand_EraseBlock(uint32_t ulBlockNo)
{
    uint32_t deviceaddress;

    deviceaddress = NAND_DEVICE;

    ulBlockNo <<= 6;    // 블록 번호를 페이지 번호로 변환

    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE0;
    __DSB();
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ulBlockNo;
    __DSB();
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ulBlockNo >> 8;
    __DSB();
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE1;
    __DSB();
    WAIT_BUSY();

    return (Nand_GetStatus());
}


static uint8_t ReadPageBuf[NAND_PAGE_SIZE+NAND_PAGE_SPARE_SIZE], WritePageBuf[NAND_PAGE_SIZE+NAND_PAGE_SPARE_SIZE];


void Test_NandFlash(void)
{
    uint32_t nandStatus;
    uint32_t nandID;
    uint32_t ulPageNo = 128;

    printf("\n%lu : NAND Test Start...\n", bsp_GetTickCount());

    nandStatus = Nand_Reset();
    if (nandStatus == NAND_OK)
    {
        printf("%lu : NAND Reset is O.K.\n", bsp_GetTickCount());
    }

    nandID = Nand_ReadID();
    printf("%lu : NAND ID : %08lX\n", bsp_GetTickCount(), nandID);

    nandStatus = Nand_ReadPage(ReadPageBuf, ulPageNo);
    if (Nand_GetStatus() == NAND_READY)
    {
        Uart_HexaDump(COM1, ReadPageBuf, NAND_PAGE_SIZE, "\n(Read)PageNo : %lu", ulPageNo);
    }

    nandStatus = Nand_EraseBlock(2);
    if (nandStatus == NAND_READY)
    {
        printf("%lu : NAND Erase is O.K.\n", bsp_GetTickCount());
    }

    nandStatus = Nand_ReadPage(ReadPageBuf, ulPageNo);
    if (Nand_GetStatus() == NAND_READY)
    {
        Uart_HexaDump(COM1, ReadPageBuf, NAND_PAGE_SIZE, "\n(Read)PageNo : %lu", ulPageNo);
    }

    for (int i = 0; i < NAND_PAGE_SIZE; i++)
    {
        WritePageBuf[i] = (uint8_t)i;
    }

    nandStatus = Nand_WritePage(WritePageBuf, ulPageNo);
    if (Nand_GetStatus() == NAND_READY)
    {
        Uart_HexaDump(COM1, WritePageBuf, NAND_PAGE_SIZE, "\n(Write)PageNo : %lu", ulPageNo);
    }
    nandStatus = Nand_ReadPage(ReadPageBuf, ulPageNo);
    if (Nand_GetStatus() == NAND_READY)
    {
        Uart_HexaDump(COM1, ReadPageBuf, NAND_PAGE_SIZE, "\n(Read)PageNo : %lu", ulPageNo);
    }
    if (memcmp(ReadPageBuf, WritePageBuf, NAND_PAGE_SIZE) != 0)
    {
        printf("%lu : page[%lu] is not same\n", bsp_GetTickCount(), ulPageNo);
    }
    else
    {
        printf("%lu : page[%lu] is same\n", bsp_GetTickCount(), ulPageNo);
    }

    printf("%lu : NAND Test Finished.\n\n", bsp_GetTickCount());
}

