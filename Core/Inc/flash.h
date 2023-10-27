/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : flash.h
 *  모듈 설명    : Flash 처리 정의 헤더
 *
 *  작성자       : 선영칠
 *  작성일       : 2022/04/15
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "typedefs.h"
#include "time.h"

#if 1	// AT25DF321 - Atmel Serial Data Flash
#define BANKSIZ					0x400000		/* 4194304(4M)Bytes(BLOCKSIZ x BLOCKS_PER_BANK) */
#define BLOCKS_PER_BANK         1024			/* Block No */
#define	BLOCKSIZ				0x1000			/* 4096(4K)Bytes (PAGESIZ x PAGES_PER_BLOCK) Erase Unit	*/
#define	PAGES_PER_BLOCK         16
#define PAGESIZ					256
#define HALFPAGESIZ             128



#define SSGBASEADR		0	 		// offset: 0, Header(4K) + APP(988K)
#define SSG_APPSIZ      (1024*896)	// MCU boot 영역 : 128K, MCU app 영역 : 896K(0xE0000)

#define PGMT_SSG		5

#define	SSG_XFNAME		"ssagm.spg1"
#define XYM_MAGIC		"XY-MODEM"
#define ID_AT25DF641            0x1F4800
#define ID_AT25DF321			0x1f470100

#define BKP_XYMOD		0xAA55A151U
#define BKP_XNAND		0xAA55A252U
#define BKP_XFILE		0xAA55A353U

/* Data Flash Command */
#define SF_WREN 				0x06
#define SF_WRDI 				0x04
#define SF_RDSR 				0x05
#define SF_WRSR 				0x01

//#define SF_RDSR 			0x05	// Read Status Register
#define SF_WRSR1 			0x01	// Write Status Register Byte1
#define SF_WRSR2 			0x31	// Write Status Register Byte2

#define SF_READ 				0x0b			// Read Array (Fast Frequency - Max 70MHz Clock)
#define SF_READ_LOW 			0x03			// Read Array (Low Frequency - Max 33MHz Clock)
#define SF_PROGRAM 				0x02			// Byte/Page Program (1 to 256 Bytes)
#define SF_BLK_ERASE_4K 		0x20			// Block Erase (4KBytes)
#define SF_BLK_ERASE_32K 		0x52			// Block Erase (32KBytes)
#define SF_BLK_ERASE_64K 		0xd8			// Block Erase (64KBytes)
#define SF_CHIP_ERASE 			0x60			// Chip Erase
#define SF_RDID 				0x9f			// Read Manufacturer and Device ID
#define SF_PROTECT				0x36			// Protect Sector
#define SF_UNPROTECT			0x39			// Unprotect Sector
#define SF_RDSPR				0x3c			// Read Sector Protection Registers
#define SF_DPD					0xb9			// Deep Power-Down
#define SF_RDPD					0xab			// Resume from Deep Power-Down

// status register bit define
#define bSPRL   7	// Sector Protection Registers Locked
			// 0 Sector Protection Registers are unlocked (default).
			// 1 Sector Protection Registers are locked.
#define bEPE	5	// Erase/Program Error
			// 0 Erase or program operation was successful.
			// 1 Erase or program error detected.
#define bWPP	4	// Write Protect(/WP) Pin Status
			// 0 WP is asserted.
			// 1 WP is deasserted.
#define bSWP1	3	// Software Protection Status
#define bSWP0	2	// Software Protection Status
			//	00	All sectors are software unprotected (all Sector Protection Registers are 0).
			//	01	Some sectors are software protected.
			//		Read individual Sector Protection Registers to determine which sectors are protected.
			//	10	Reserved for future use.
			//	11	All sectors are software protected (all Sector Protection Registers are 1 ? default).
#define bWEL	1	// Write Enable Latch Status
			// 0 Device is not write enabled (default).
			// 1 Device is write enabled.
#define bBUSY	0	// Ready/Busy Status
			// 0 Device is ready.
			// 1 Device is busy with an internal operation.

#else	// K9F1208U0A
#define BANKSIZ			0x4000000	/* 67108864(64M)Bytes(BLOCKSIZ x BLOCKS_PER_BANK) */
#define BLOCKS_PER_BANK         4096
#define	BLOCKSIZ		0x4000		/* 16384(16K)Bytes (PAGESIZ x PAGES_PER_BLOCK)	*/
#define MAX_PAGE_ADDR           0x20000		/* 131072(128K Pages) */
#define	PAGES_PER_BLOCK         32
#define PAGESIZ			512
#define HALFPAGESIZ             256
#endif

/*	return error code	*/
#define	E_ADDR			1	/* address error	*/
#define	E_TIME			2	/* timeout error	*/
#define	E_FAIL			3	/* failure error	*/
#define	E_VERF			4	/* verify error		*/
#define	E_WP			5	/* write protect	*/

typedef	long	off_t;		/* integer 4 byte */
typedef long   	siz_t;		/* integer 4 byte */

extern byte BlockBuf[BLOCKSIZ];
extern const char FlashHelp[];
int 	FlashSer(void);
int 	FlashInit(void);

void 	flash_pwr_down(void);
void 	flash_pwr_resume(void);
int 	flash_ready(int ms);
int 	flash_is(void);
int 	flash_stat(void);
siz_t 	flash_size(int force);
int 	flash_read(siz_t addr, byte *buf, siz_t size);
int 	flash_erase(siz_t addr, siz_t size);
int 	flash_write(siz_t addr, byte *buf, siz_t size);

#define APP_HEADSIZ	4096

#define IOBBASEADR	0	 	// offset: 0, Header(4K) + APP(296K) = 300K
#define IOB_APPSIZ      (1024*296)

#define	SMSBASEADR	0x4C000	 	// offset: 300K, 492K
#define SMS_APPSIZ	(1024*492)

#define MDBBASEADR	0xC8000		// offset: 800k, Header(4K) + APP(192K)
#define MDB_APPSIZ	(1024*192)

#define PGMT_IOB	0
#define PGMT_MDB	1

#define	XFNAMSIZ	10
typedef struct { // 상위 sms단으로 부터 다운로드 하여 퓨징하는 경우의 헤더 정보
	char	crc32[8];	// crc32: hex-ascii
	char	dot0;
	char	size[8];	// file size: hex-ascii
	char	dot1;
	char	name[XFNAMSIZ];	// file name("s2agm.iob1" or "s2agm.mdb1")
	char	dot2;
	char	vers[3];	// VRM
	char	dot3;
	char	build[12];	// build time: YYMMDDHHMMSS
	char	eol;
} __attribute__((packed)) xfile_t;	// 46Bytes

typedef struct { // xmodem/ymodem를 통해서 퓨징하는 경우의 헤더 정보
	char 	magic[8];	// 8.  "XYMODEMP"
	char	name[15];	// 23. "MDB100-0.2A.BIN", or "IOB100-1.2A.BIN"
	char	pgmt;		// 24. 프로그램 타입(0:PGMT_IOB, 1:PGMT_MDB)
	dword	dcrc;		// 28. 데이터에 대한 crc
	dword	dsiz;           // 32. 데이터 사이즈
	dword	sadr;		// 36. serial flash 저장 어드레스
	dword	fadr;		// 40. MCU 응용 프로그램 시작 주소(퓨징 어드레스)
	time_t	utim;		// 44. 업데이트 요청 시간
	byte	rfu[2];		// 예비
} __attribute__((packed)) xnand_t;	// 46Bytes

typedef union {
	byte	head[46];
	xfile_t	xfile;
	xnand_t	xnand;
} __attribute__((packed)) xhead_t;



// SPI 장치 제어
#define SF_EN()    (HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET))
#define SF_DE()    (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET))




#endif /* INC_FLASH_H_ */
