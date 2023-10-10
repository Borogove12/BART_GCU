/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_nand.h
 *  모듈 설명    : NAND Flash 헤더
 *
 *  작성자       : 김정준
 *  작성일       : 2022/09/10
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef INC_BSP_NAND_H_
#define INC_BSP_NAND_H_

/*
 *
 */

#define NAND_PAGE_PER_BLOCK		64
#define NAND_BLOCK_PER_ZONE		1024
#define NAND_PAGE_SPARE_SIZE	64


#define NAND_PAGE_SIZE 			((uint16_t)0x0800) 		/* 2 * 예비 영역 없이 페이지당 1024바이트 */
#define NAND_BLOCK_SIZE 		((uint16_t)0x0040) 		/* 블록당 64페이지 */
#define NAND_ZONE_SIZE 			((uint16_t)0x0400) 		/* 영역당 1024 블록 */
#define NAND_SPARE_AREA_SIZE 	((uint16_t)0x0040) 		/* 예비 영역으로 마지막 64바이트 */
#define NAND_MAX_ZONE 			((uint16_t)0x0001) 		/* 1024 블록의 1개 영역 */
#define NAND_ADDR_5 			0		 				/* 0은 4바이트의 주소만 전송하고 1은 5를 의미합니다. */

/* 명령어 코드 정의 */
#define NAND_CMD_COPYBACK_A 	((uint8_t)0x00) 		/* PAGE COPY-BACK 명령 시퀀스 */
#define NAND_CMD_COPYBACK_B 	((uint8_t)0x35)
#define NAND_CMD_COPYBACK_C 	((uint8_t)0x85)
#define NAND_CMD_COPYBACK_D		((uint8_t)0x10)

// #define NAND_CMD_STATUS ((uint8_t)0x70) /* NAND 플래시의 상태 워드 읽기 */

#define MAX_PHY_BLOCKS_PER_ZONE 1024 	/* 각 영역의 최대 물리적 블록 수 */
#define MAX_LOG_BLOCKS_PER_ZONE 1000 	/* 각 영역의 최대 논리 블록 수 */

#define NAND_BLOCK_COUNT 1024 			/* 블록 수 */
#define NAND_PAGE_TOTAL_SIZE 	(NAND_PAGE_SIZE + NAND_SPARE_AREA_SIZE) // 총 페이지 크기(2112)


#define NAND_BAD_BLOCK_FLAG 0x00
#define NAND_USED_BLOCK_FLAG 0xF0

#define BI_OFFSET       0
#define USED_OFFSET     1
#define LBN0_OFFSET     2
#define LBN1_OFFSET     3
#define VALID_SPARE_SIZE 4

/* FSMC NAND 메모리 주소 계산 */
#define ADDR_1st_CYCLE(ADDR) (uint8_t)((ADDR)& 0xFF) 				/* 첫 번째 주소 지정 주기 */
#define ADDR_2nd_CYCLE(ADDR) (uint8_t)(((ADDR)& 0xFF00) >> 8) 		/* 두 번째 주소 지정 주기 */
#define ADDR_3rd_CYCLE(ADDR) (uint8_t)(((ADDR)& 0xFF0000) >> 16) 	/* 세 번째 주소 지정 주기 */
#define ADDR_4th_CYCLE(ADDR) (uint8_t)(((ADDR)& 0xFF000000) >> 24) 	/* 네 번째 주소 지정 주기 */


#define NAND_OK   0
#define NAND_FAIL 1

#define FREE_BLOCK (1 << 12 )
#define BAD_BLOCK (1 << 13 )
#define VALID_BLOCK (1 << 14 )
#define USED_BLOCK (1 << 15 )

uint8_t bsp_InitNand(void);

uint32_t Nand_ReadID(void);
uint32_t Nand_ReadStatus(void);
uint32_t Nand_GetStatus(void);
uint8_t  Nand_Reset(void);
uint8_t  Nand_ReadPage(uint8_t *pBuffer, uint32_t ulPageNo);
uint8_t  Nand_ReadSparePage(uint8_t *pBuffer, uint32_t ulPageNo);
uint8_t  Nand_WritePage(uint8_t *pBuffer, uint32_t ulPageNo);
uint8_t  Nand_WriteSparePage(uint8_t *pBuffer, uint32_t ulPageNo);

uint32_t Nand_EraseBlock(uint32_t _ulBlockNo);


void Test_NandFlash(void);


#endif /* INC_BSP_NAND_H_ */
