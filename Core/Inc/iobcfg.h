/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : iobcfg.h
 *  모듈 설명    : IO Address Configuration에 대한 정의 헤더
 *
 *  작성자       : 선영칠
 *  작성일       : 2022/04/15
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef INC_IOBCFG_H_
#define INC_IOBCFG_H_

#include "bsp_types.h"
#include "bsp_uart.h"

// PORT GPIOA
#define SPR_INT			GPIO_PIN_0
#define EEPR_CS			GPIO_PIN_1
//#define COM2_TX			GPIO_PIN_2
//#define COM2_RX			GPIO_PIN_3
#define SF_CS			GPIO_PIN_4
#define HUB_CS			GPIO_PIN_4    //change pms_test
#define SPI1_CLK		GPIO_PIN_5
#define SPI1_MISO		GPIO_PIN_6
#define SPI1_MOSI		GPIO_PIN_7
#define COM1_TX			GPIO_PIN_9
#define COM1_RX			GPIO_PIN_10
#define USB_DM			GPIO_PIN_11
#define USB_DP			GPIO_PIN_12
#define USB_VBUS		GPIO_PIN_13
#define STS_LED			GPIO_PIN_14
#define BOD_BUZ			GPIO_PIN_15

// PORT GPIOB
#define USB_INT			GPIO_PIN_1
#define BOOT1			GPIO_PIN_2
#define BID_IN0			GPIO_PIN_3
#define BID_IN1			GPIO_PIN_4
#define BID_IN2			GPIO_PIN_5
#define I2C_SCL			GPIO_PIN_6
#define I2C_SDA			GPIO_PIN_7
#define USB_RST			GPIO_PIN_9
//#define COM3_TX			GPIO_PIN_10
//#define COM3_RX			GPIO_PIN_11
#define USB_CS			GPIO_PIN_12
#define SPI2_CLK		GPIO_PIN_13
#define SPI2_MISO		GPIO_PIN_14
#define SPI2_MOSI		GPIO_PIN_15

// PORT GPIOC
#define DIP_SW1			GPIO_PIN_0
#define DIP_SW2			GPIO_PIN_1
#define DIP_SW3			GPIO_PIN_2
#define DIP_SW4			GPIO_PIN_3
#define HUB_RST			GPIO_PIN_3 //change pms_test
#define DIP_SW5			GPIO_PIN_4
#define DIP_SW6			GPIO_PIN_5
#define COM6_TX			GPIO_PIN_6
#define COM6_RX			GPIO_PIN_7
#define DIP_SW7			GPIO_PIN_8
#define COM4_TX			GPIO_PIN_10
#define COM4_RX			GPIO_PIN_11
#define COM5_TX			GPIO_PIN_12
#define NAND_WP			GPIO_PIN_13
#define OSC32_IN		GPIO_PIN_14
#define OSC32_OUT		GPIO_PIN_15

// PORT GPIOD
#define FSMC_D2			GPIO_PIN_0
#define FSMC_D3			GPIO_PIN_1
#define COM5_RX			GPIO_PIN_2
#define FSMC_NOE		GPIO_PIN_4
#define FSMC_NWE		GPIO_PIN_5
#define NAND_NWAIT		GPIO_PIN_6
#define FSMC_NE1		GPIO_PIN_7
#define DIP_SW8			GPIO_PIN_8
#define DIP_SW9			GPIO_PIN_9
#define DIP_SW10		GPIO_PIN_10
#define FSMC_A16		GPIO_PIN_11
#define FSMC_A17		GPIO_PIN_12
#define FSMC_A18		GPIO_PIN_13
#define FSMC_D0			GPIO_PIN_14
#define FSMC_D1			GPIO_PIN_15

// PORT GPIOE
#define DIP_SW11		GPIO_PIN_0
#define DIP_SW12		GPIO_PIN_1
#define DIP_SW13		GPIO_PIN_2
#define LAN_RST			GPIO_PIN_3
#define LAN_CS		    GPIO_PIN_4
#define FSMC_A21		GPIO_PIN_5
#define FSMC_A22		GPIO_PIN_6
#define FSMC_D4			GPIO_PIN_7
#define FSMC_D5			GPIO_PIN_8
#define FSMC_D6			GPIO_PIN_9
#define FSMC_D7			GPIO_PIN_10
#define DIP_SW15		GPIO_PIN_11
//#define PUSH_SW1		GPIO_PIN_12
//#define PUSH_SW2		GPIO_PIN_13
//#define PUSH_SW3		GPIO_PIN_14
//#define PUSH_SW4		GPIO_PIN_15

// PORT GPIOF
#define HUB_DIO			GPIO_PIN_0
#define HUB_CLK			GPIO_PIN_1
#define FSMC_A2			GPIO_PIN_2
#define FSMC_A3			GPIO_PIN_3
#define FSMC_A4			GPIO_PIN_4
#define UART_INTA		GPIO_PIN_5
#define UART_INTB		GPIO_PIN_6
//#define UART_INTC		GPIO_PIN_7
#define S_FLASH_CS		GPIO_PIN_7		//change pms_test
#define UART_INTD		GPIO_PIN_8
#define HUB_CONF		GPIO_PIN_15	// add 2017.03.17

// PORT GPIOG
#define LED_COLA		GPIO_PIN_0
#define LED_COLB		GPIO_PIN_1
#define LED_COLC		GPIO_PIN_2
#define LED_COL0		GPIO_PIN_3
#define LED_COL1		GPIO_PIN_4
#define LED_COL2		GPIO_PIN_5
#define NAND_RB			GPIO_PIN_6

#define LED_CTL			GPIO_PIN_8
#define FSMC_NE2		GPIO_PIN_9	// I/O chip select
#define FSMC_NE3		GPIO_PIN_10	// External UART chip select
#define UART_RST		GPIO_PIN_11
#define FND_DIG1		GPIO_PIN_12
#define FND_DIG2		GPIO_PIN_13
#define FND_DIG3		GPIO_PIN_14
#define FND_DIG4		GPIO_PIN_15

#define MAXFND			4

// 인터럽트 처리 라인


// 내부 UART
#define COM1_UART      	USART1
#define COM1_IRQn       USART1_IRQn
#define COM2_UART       USART2
#define COM2_IRQn       USART2_IRQn
#define COM3_UART       USART3
#define COM3_IRQn       USART3_IRQn
#define COM4_UART       UART4
#define COM4_IRQn       UART4_IRQn
#define COM5_UART       UART5
#define COM5_IRQn       UART5_IRQn


#define HI(port, idx) do{(port)->BSRR = 1UL<<(idx);}while(0)
#define LO(port, idx) do{(port)->BSRR = 1UL<<(16+idx);}while(0)


/* 외부 IO 장치 어드레스 맵 */
/*
Bank1 : 4 x 64MB  NOR/PSRM Memory Map(0x60000000 ~ 0x6FFFFFFF)
0x60000000 : Bank1_NORSRAM1_ADDR
0x64000000 : Bank1_NORSRAM2_ADDR
0x68000000 : Bank1_NORSRAM3_ADDR
0x6C000000 : Bank1_NORSRAM4_ADDR
*/
#define EXIO_BASE     	0x64000000  	// Bank1_NORSRAM2_ADDR
#define UART_BASE     	0x68000000  	// Bank1_NORSRAM3_ADDR

//#define EXP_MAX			17		// 최대 I/O 포트 개수 : william, 최대 I/O 포트는 12개이며 아래 정의 되어 있어서 삭제함. 

#define MTX_MAX		12
#define EXP_MAX		12				// 최대 I/O 포트 개수
#define EXP_IN		7
#define EXP_OUT		5

// AIOB 보드 I/O 메모리 맵(EMI / 삼성병합 / 삼성장애인 게이트)
#define AMTX_MAX		12
#define AEXP_MAX		12
#define AEXP_IN			7
#define AEXP_OUT		5

#define AEI01_ADR      	(EXIO_BASE+0x000000) 	// EXP00-IN,  SENSOR_CS1(LED #A)
#define AEI02_ADR      	(EXIO_BASE+0x010000)	// EXP01-IN,  SENSOR_CS2(LED #B)
#define AEI03_ADR      	(EXIO_BASE+0x020000) 	// EXP02-IN,  SENSOR_CS3(LED #C)
#define AEI04_ADR      	(EXIO_BASE+0x110000)	// EXP03-IN,  MODESW_CS(LED #D)
#define AEI05_ADR      	(EXIO_BASE+0x120000) 	// EXP04-IN,  DOORSW_CS(LED #E)
#define AEI06_ADR      	(EXIO_BASE+0x130000) 	// EXP05-IN,  FLAP12SW_CS(LED #F)
#define AEI07_ADR      	(EXIO_BASE+0x150000) 	// EXP06-IN,  FLAP32SW_CS(LED #G)

#define AEO08_ADR 		(EXIO_BASE+0x050000)	// EXP07-OUT, LAMPM_CS(LED #H)
#define AEO09_ADR      	(EXIO_BASE+0x060000) 	// EXP08-OUT, LAMPS_CS(LED #I)
#define AEO10_ADR      	(EXIO_BASE+0x140000)	// EXP09-OUT, FLAP12_CS(LED #J)
#define AEO11_ADR      	(EXIO_BASE+0x160000) 	// EXP10-OUT, FLAP34_CS(LED #K)
#define AEO12_ADR      	(EXIO_BASE+0x170000) 	// EXP11-OUT, Virtual(LED #L)

#define AEXP_LED   		(EXIO_BASE+0x030000) 	// OUT, LED_ROW_CS
#define AEXP_FND    	(EXIO_BASE+0x040000) 	// OUT, FND_CS
#define AEXP_SIO    	(EXIO_BASE+0x200000) 	// I/O, SPARE_IOCS

#define AEXP_UARTA    	(UART_BASE+0x10000) 	// I/O, EXT UART-A
#define AEXP_UARTB    	(UART_BASE+0x20000) 	// I/O, EXT UART-B
#define AEXP_UARTC    	(UART_BASE+0x30000) 	// I/O, EXT UART-C
#define AEXP_UARTD    	(UART_BASE+0x40000) 	// I/O, EXT UART-D

// BIOB 보드 I/O 메모리 맵(C&C A-Type 게이트)
#define BMTX_MAX		4
#define BEXP_MAX		11
#define BEXP_IN			4
#define BEXP_OUT		7

#define BEI01_ADR   	(EXIO_BASE+0x000000)	// EXP00-IN,  UTKEY_IN_CS
#define BEI02_ADR   	(EXIO_BASE+0x400000)	// EXP01-IN,  EMFA_IN_CS
#define BEI03_ADR   	(EXIO_BASE+0x410000)	// EXP02-IN,  FSTS_IN_CS
#define BEI04_ADR   	(EXIO_BASE+0x440000)	// EXP03-IN,  DOOR_IN_CS

#define BEO05_ADR   	(EXIO_BASE+0x100000)	// EXP04-OUT, FDATA_OUT_CS
#define BEO06_ADR   	(EXIO_BASE+0x110000)	// EXP05-OUT, FCTRL_OUT_CS
#define BEO07_ADR   	(EXIO_BASE+0x120000)	// EXP06-OUT, UTKEY_OUT_CS
#define BEO08_ADR   	(EXIO_BASE+0x130000)	// EXP07-OUT, LCD_OUT_CS
#define BEO09_ADR   	(EXIO_BASE+0x420000)	// EXP08-OUT, FSTS_OUT_CS
#define BEO10_ADR   	(EXIO_BASE+0x430000)	// EXP09-OUT, CTRL_OUT_CS
#define BEO11_ADR   	(EXIO_BASE+0x450000)	// EXP10-OUT, BAR_OUT_CS

#define BEXP_LED   		(EXIO_BASE+0x470000) 	// OUT, LED_ROW_CS
#define BEXP_FND    	(EXIO_BASE+0x460000) 	// OUT, FND_CS
#define BEXP_SIO    	(EXIO_BASE+0x200000) 	// I/O, SPARE_IOCS

// DIOB 보드 I/O 메모리 맵(삼성 전용 게이트)
#define DMTX_MAX		0
#define DEXP_MAX		17
#define DEXP_IN			6
#define DEXP_OUT		11

//#define DEI01_ADR   	(EXIO_BASE+0x000000)	// EXP00-IN,  UT_KEY_IN(Not Use - floating)
//#define DEI07_ADR   	(EXIO_BASE+0x060000)	// EXP06-IN,  IN_CS7(Not Use - floating)
//#define DEI08_ADR   	(EXIO_BASE+0x070000)	// EXP07-IN,  IN_CS8(Not Use - floating)
#define DEI01_ADR   	(EXIO_BASE+0x010000)	// EXP00-IN,  SENSOR_IN1
#define DEI02_ADR   	(EXIO_BASE+0x020000)	// EXP01-IN,  SENSOR_IN2
#define DEI03_ADR   	(EXIO_BASE+0x030000)	// EXP02-IN,  EN_FLAPSW_IN
#define DEI04_ADR   	(EXIO_BASE+0x040000)	// EXP03-IN,  EX_FLAPSW_IN
#define DEI05_ADR   	(EXIO_BASE+0x050000)	// EXP04-IN,  DOOR_IN
#define DEI06_ADR   	(EXIO_BASE+0x400000)	// EXP05-IN,  EMFA_IN_CS

#define DEO07_ADR   	(EXIO_BASE+0x100000)	// EXP06-OUT, UT_LCD_CTRL
#define DEO08_ADR   	(EXIO_BASE+0x110000)	// EXP07-OUT, UT_KEY_OUT
#define DEO09_ADR   	(EXIO_BASE+0x120000)	// EXP08-OUT, UT_LCD_DATA
#define DEO10_ADR   	(EXIO_BASE+0x300000)	// EXP09-OUT, BAR_OUT
#define DEO11_ADR   	(EXIO_BASE+0x310000)	// EXP10-OUT, TOP_FND_OUT
#define DEO12_ADR   	(EXIO_BASE+0x320000)	// EXP11-OUT, TOP_BOXLED_OUT
#define DEO13_ADR   	(EXIO_BASE+0x330000)	// EXP12-OUT, TOP_LAMP_OUT
#define DEO14_ADR   	(EXIO_BASE+0x340000)	// EXP13-OUT, APP_OUT
#define DEO15_ADR   	(EXIO_BASE+0x350000)	// EXP14-OUT, EN_FLAP_OUT
#define DEO16_ADR   	(EXIO_BASE+0x360000)	// EXP15-OUT, EX_FLAP_OUT
#define DEO17_ADR   	(EXIO_BASE+0x370000)	// EXP16-OUT, BUZZER_OUT

#define DEXP_FND    	(EXIO_BASE+0x460000) 	// OUT, FND_CS
#define DEXP_SIO    	(EXIO_BASE+0x200000) 	// I/O, SPARE_IOCS

// EIOB 및 FIOB 보드 I/O 메모리 맵(기타 EMI 단말기 사용 게이트 및 개방형 게이트)
#define EMTX_MAX		0
#define EEXP_MAX		2
#define EEXP_IN			1
#define EEXP_OUT		1

#define EEI01_ADR   	(EXIO_BASE+0x400000)	// EXP00-IN,  EMFA_IN_CS
#define EEO02_ADR   	(EXIO_BASE+0x450000)	// EXP01-OUT, BAR_OUT_CS

#define EEXP_FND    	(EXIO_BASE+0x460000) 	// OUT, FND_CS
#define EEXP_SIO    	(EXIO_BASE+0x200000) 	// I/O, SPARE_IOCS


/* 외부 UART 칩(TL16C554A) 제어를 위한 설정 값 */
#if 0
#define COMM_CLK		3686400		// UART X-tal: 3.6864MHz
#define COMM_CLK_16		230400
#else
#define COMM_CLK		14745600	// UART X-tal: 14.7456MHz
#define COMM_CLK_16		921600
#endif

/*
#define	COM1			0
#define COM2			1
#define COM3			2
#define	COM4			3
#define	COM5			4
#define COM6			5
#define COM7			6
#define COM8			7
#define COM9			8
#define COM10			9
#define COM_MAX			16
*/

#define FND_MAX		4

#define	BAUD2400		0
#define	BAUD4800		1
#define	BAUD9600		2
#define	BAUD19200		3
#define	BAUD38400		4
#define	BAUD57600		5
#define	BAUD115200		6

#define COMM_PARITY_NONE	0x00
#define COMM_PARITY_EVEN	0x01
#define COMM_PARITY_ODD		0x02

/* Stopbits */
#define COMM_STOP1 		0
#define COMM_STOP2 		1
#define COMM_STOP1_5 	1

/* word length */
#define COMM_CHR5 		0
#define COMM_CHR6 		1
#define COMM_CHR7 		2
#define COMM_CHR8 		3

/* fifo length */
#define COMM_FIFO1 		0
#define COMM_FIFO4 		1
#define COMM_FIFO8 		2
#define COMM_FIFO14 	3

 /* TL16C554A */
#define TL16C554A_RHR 	0	/* 0, Receive Holding Register */
#define TL16C554A_THR 	0	/* 0, Receive Holding Register */
#define TL16C554A_IER 	4 	/* 1, Interrupt Enable Register */
#define TL16C554A_FCR 	8 	/* 2, FIFO control Register */
#define TL16C554A_IIR 	8	/* 2, Interrupt Identification Register */
#define TL16C554A_LCR 	12	/* 3, Line control register */
#define TL16C554A_MCR 	16	/* 4, Modem Control Register */
#define TL16C554A_LSR 	20	/* 5, Line Status Register */
#define TL16C554A_MSR 	24 	/* 6, Modem Status Register */
#define TL16C554A_SPR 	28 	/* 7, Scratch pad Register */

#define DLL 			0	/* Divisor LSB latch address */
#define DLH 			4	/* Divisor MSB Latch address */


/* FCR */
#define	FIFO_EN			0x01
#define	RCV_FIFO_RESET	0x02
#define XMT_FIFO_RESET	0x04
#define DMA_MODE		0x08
#define	RCV_TRG			0x40
#define	XMT_TRG			0x80

/* Mode Switch Value */
#define MODE_SW_ENT		0x01
#define MODE_SW_EXT 	0x02
#define MODE_SW_ENX		0x04
#define MODE_SW_STP		0x08
#define MODE_SW_EMS		0x10
#define MODE_SW_O_C		0x20


#if 0

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

#endif

#endif /* INC_IOBCFG_H_ */
