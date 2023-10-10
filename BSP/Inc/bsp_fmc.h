/*
 * bsp_fmc.h
 *
 *  Created on: 2023. 8. 9.
 *      Author: Moo
 */

#ifndef INC_BSP_FMC_H_
#define INC_BSP_FMC_H_


#define READ00_ADR      	(READ_BASE+0x00U) 	// nPSEN1_CS
#define READ01_ADR      	(READ_BASE+0x100U)	// nPSEN2_CS
#define READ02_ADR      	(READ_BASE+0x300U) 	// nDOOR_SW_CS
#define READ03_ADR      	(READ_BASE+0x500U)	// nSCADA_IN_CS
#define READ04_ADR      	(READ_BASE+0x600U) 	// nBRR_IN_CS


#define WRITE00_ADR      	(WRITE_BASE+0x00U) 	// n5V_LED_CS
#define WRITE01_ADR      	(WRITE_BASE+0x100U)	// n24V_LED_CS
#define WRITE02_ADR      	(WRITE_BASE+0x200U) // nBRR_OUT_CS
#define WRITE03_ADR      	(WRITE_BASE+0x400U)	// nSCADA_OUT_CS

/*
*******************************************************************************
*           INPUT 관련
*******************************************************************************
*/
// Flap Switch bit Value
#define FLAP1_SW1           0x01
#define FLAP1_SW2           0x02
#define FLAP1_SW3           0x04
#define FLAP2_SW1           0x08
#define FLAP2_SW2           0x10
#define FLAP2_SW3           0x20
#define FLAP3_SW1           0x01
#define FLAP3_SW2           0x02
#define FLAP3_SW3           0x04
#define FLAP4_SW1           0x08
#define FLAP4_SW2           0x10
#define FLAP4_SW3           0x20


// PUSH SW
#define PUSH_SW1            0x10
#define PUSH_SW2            0x20
#define PUSH_SW3            0x40
#define PUSH_SW4            0x80

// PWR_FAIL, EMC_IN, EMP_IN
#define PWR_FAIL            0x01
#define EMC_IN              0x02
#define EMP_IN              0x04


// Total 18 Passenger Sensor Bit Flag(IN/OUT)
#define PASS_SENSOR1        0x00000001
#define PASS_SENSOR2        0x00000002
#define PASS_SENSOR3        0x00000004
#define PASS_SENSOR4        0x00000008
#define PASS_SENSOR5        0x00000010
#define PASS_SENSOR6        0x00000020
#define PASS_SENSOR7        0x00000040
#define PASS_SENSOR8        0x00000080
#define PASS_SENSOR9        0x00000100
#define PASS_SENSOR10       0x00000200
#define PASS_SENSOR11       0x00000400
#define PASS_SENSOR12       0x00000800
#define PASS_SENSOR13       0x00001000
#define PASS_SENSOR14       0x00002000
#define PASS_SENSOR15       0x00004000
#define PASS_SENSOR16       0x00008000
#define PASS_SENSOR17       0x00010000
#define PASS_SENSOR18       0x00020000


// Remote Controller Switch 1 ~ 4
#define REMOCON_SW1         0x01
#define REMOCON_SW2         0x02
#define REMOCON_SW3         0x04
#define REMOCON_SW4         0x08


// Master, Extention Door Switch Value
#define DOOR_SW1            0x0001
#define DOOR_SW2            0x0002
#define DOOR_SW3            0x0004
#define DOOR_SW4            0x0008
#define DOOR_SW5            0x0010
#define DOOR_SW6            0x0020
#define DOOR_SW7            0x0040
#define DOOR_SW8            0x0080
#define DOOR_SW9            0x0100
#define DOOR_SW10           0x0200


// MODE Switch Bit Value
#define MODESW_ENTRY        0x01    // 개표
#define MODESW_EXIT         0x02    // 집표
#define MODESW_ENTEXIT      0x04    // 양방향(개집표)
#define MODESW_STOP         0x08    // 정지모드
#define MODESW_EMERGENCY    0x10    // 비상모드
#define MODESW_OC           0x20    // ??모드


// SW 값 반전을 위한 MASK
#define FLAP12SW_MASK       0x3F
#define FLAP34SW_MASK       0x3F
#define BOARDID_MASK        0x0F
#define PUSHSW_MASK			0xF0
#define DIPSW1_MASK         0x7F
#define DIPSW2_MASK         0xFF
#define PUSHSWONLY_MASK     0x0F
#define FAILSW_MASK         0xF0


/*
*******************************************************************************
*           CONTROL 관련
*******************************************************************************
*/

// FLAP 1, 3 Control Signal
#define FLAP13_OC           0x01
#define FLAP13_STP          0x02
#define FLAP13_REV          0x04
#define FLAP24_OC           0x08
#define FLAP24_STP          0x10
#define FLAP24_REV          0x20

#define FLAP13_CLOSE        0x0F//0xFF   //0x00
#define FLAP13_OPEN         0x0E//0xFE   //0x01
#define FLAP13_STOP         0x0D//0xFD   //0x02
#define FLAP13_RESET        0x0B//0xFB   //0x04
#define FLAP24_CLOSE        0xFF//0xFF   //0x00
#define FLAP24_OPEN         0xF7//0xF7   //0x08
#define FLAP24_STOP         0xE0//0xEF   //0x10
#define FLAP24_RESET        0xD0//0xDF   //0x20

#define		FLAP_OPEN	0 // flap open control
#define		FLAP_CLOSE	1 // flap close control
#define		FLAP_STOP	2 // flap stop control
#define 	FLAP_ACTMO	1600	// flap action timeout(1600ms)


/*
** LED, LAMP Control - Bit Value Oring 가능
** Master, Slave  같이 사용
** TODO: DipSW 설정에 따라 하니의 함수에서 Master, Slave 알아서 Control 하게
*/
#define LED_DIR_ARROW       0x0001
#define LED_DIR_STOP        0x0002
#define LED_DIR_RED         0x0004
#define LED_DIR_GREEN       0x0008
#define LED_DIR_BLUE        0x0010
#define LED_TOP_RED         0x0020
#define LED_TOP_GREEN       0x0040
#define LED_TOP_BLUE        0x0080
#define LED_LINE_RED        0x0100
#define LED_LINE_GREEN      0x0200
#define LED_LINE_BLUE       0x0400

#define LED_DIR_WHITE       (LED_DIR_RED | LED_DIR_GREEN | LED_DIR_BLUE)
#define LED_DIR_YELLOW      (LED_DIR_RED | LED_DIR_GREEN)
#define LED_DIR_PURPLE      (LED_DIR_RED | LED_DIR_BLUE)
#define LED_DIR_CYAN        (LED_DIR_GREEN | LED_DIR_BLUE)
#define LED_LINE_WHITE      (LED_LINE_RED | LED_LINE_GREEN | LED_LINE_BLUE)
#define LED_LINE_YELLOW     (LED_LINE_RED | LED_LINE_GREEN)
#define LED_LINE_PURPLE     (LED_LINE_RED | LED_LINE_BLUE)
#define LED_LINE_CYAN       (LED_LINE_GREEN | LED_LINE_BLUE)
#define LED_TOP_WHITE       (LED_TOP_RED | LED_TOP_GREEN | LED_TOP_BLUE)
#define LED_TOP_YELLOW      (LED_TOP_RED | LED_TOP_GREEN)
#define LED_TOP_PURPLE      (LED_TOP_RED | LED_TOP_BLUE)
#define LED_TOP_CYAN        (LED_TOP_GREEN | LED_TOP_BLUE)

void BuzzerOff(void);
void BuzzerOn(void);
void ControlStatusLED_En(UINT8 nValue);
void ControlStatusLED_Ex(UINT8 nValue);
void ControlOverHeadDisplay_En(UINT8 nValue);
void ControlOverHeadDisplay_Ex(UINT8 nValue);
void ControlIndicatorLight_En(UINT8 nValue);
void ControlIndicatorLight_Ex(UINT8 nValue);
void Brr_SetEmergency_P(bool isActive);
void Brr_SetEmergency_S(bool isActive);
void Brr_SetBreak_P(bool isActive);
void Brr_SetBreak_S(bool isActive);
void Brr_ControlBarrier_P(UINT8 nValue);
void Brr_ControlBarrier_S(UINT8 nValue);
void Brr_ControlBarrier_All_OPnC_Dir(UINT8 nValue);
void Brr_ControlBarrier_All_Dir(UINT8 nValue);
void Brr_SetBreak_A(bool isActive);
void Brr_SetEmergency_A(bool isActive);
void Barrier_Val(void);

#endif /* INC_BSP_FMC_H_ */
