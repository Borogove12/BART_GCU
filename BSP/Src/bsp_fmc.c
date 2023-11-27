/*
 * bsp_fmc.c
 *
 *  Created on: 2023. 8. 9.
 *      Author: Moo
 */


#include "bsp.h"
#include "integer.h"
#include "global.h"
#include "gcu_def.h"


//uint8_t BrrValue_Temp = 0;
uint8_t Add_000_Value = 0;
uint8_t Add_100_Value = 0;
uint8_t Add_200_Value = 0;
uint8_t Add_400_Value = 0;

void BuzzerOn(void)
{
	HAL_GPIO_WritePin(BUZZ_CON_GPIO_Port,BUZZ_CON_Pin,GPIO_PIN_SET);
}

void BuzzerOff(void)
{
	HAL_GPIO_WritePin(BUZZ_CON_GPIO_Port,BUZZ_CON_Pin,GPIO_PIN_RESET);
}

void ControlStatusLED_En(UINT8 nValue)
{
	Add_000_Value &= 0xFC;			//1111 1100

	wnP_LED_CS_ADDR = Add_000_Value;

	wnP_LED_CS_ADDR |= (UINT8)(nValue & 0x03);

	Add_000_Value = wnP_LED_CS_ADDR;
}

void ControlStatusLED_Ex(UINT8 nValue)
{
	Add_000_Value &= 0xCF;				//1100 1111

	wnP_LED_CS_ADDR = Add_000_Value;

	wnP_LED_CS_ADDR |= (UINT8)((nValue << 4) & 0x30);

	Add_000_Value = wnP_LED_CS_ADDR;
}

void ControlOverHeadDisplay_En(UINT8 nValue)
{
	Add_000_Value &= 0xF3;	// 1111 0011

	wnP_LED_CS_ADDR = Add_000_Value;

	wnP_LED_CS_ADDR |= (UINT8)((nValue << 2) & 0x0C);

	Add_000_Value = wnP_LED_CS_ADDR;
}

void ControlOverHeadDisplay_Ex(UINT8 nValue)
{
	Add_000_Value &= 0x3F; // 0011 1111

	wnP_LED_CS_ADDR = Add_000_Value;

	wnP_LED_CS_ADDR |= (UINT8)((nValue << 6) & 0xC0);

	Add_000_Value = wnP_LED_CS_ADDR;
}


void ControlIndicatorLight_En(UINT8 nValue)
{
	Add_100_Value &= 0xF8; // 1111 1000

	wnS_LED_CS_ADDR = Add_100_Value;

	wnS_LED_CS_ADDR |= (UINT8)(nValue & 0x07);

	Add_100_Value = wnS_LED_CS_ADDR;
}

void ControlIndicatorLight_Ex(UINT8 nValue)
{
	Add_100_Value &= 0x8F;

	wnS_LED_CS_ADDR = Add_100_Value;

	wnS_LED_CS_ADDR |= (UINT8)((nValue << 4) & 0x70);

	Add_100_Value = wnS_LED_CS_ADDR;
}

void Brr_SetEmergency_P(bool isActive)
{
	Add_200_Value &= 0xFF;

	switch(isActive)
	{
	case TRUE:
		Add_200_Value |= ((UINT8) ON << 1);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 1) & 0xFF));
		break;
	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}


void Brr_SetEmergency_S(bool isActive)
{
	Add_200_Value &= 0xFF;
	switch(isActive)
	{
	case TRUE:
		Add_200_Value  |= ((UINT8) ON << 5);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 5) & 0xFF));
		break;
	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}

void Brr_SetBreak_P(bool isActive)
{
	Add_200_Value &= 0xFF;

	switch(isActive)
	{
	case TRUE:
		Add_200_Value |= ((UINT8) ON << 3);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 3) & 0xFF));
		break;
	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}

void Brr_SetBreak_S(bool isActive)
{
	Add_200_Value &= 0xFF;
	switch(isActive)
	{
	case TRUE:
		Add_200_Value  |= ((UINT8) ON << 7);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 7) & 0xFF));
		break;
	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}

void Brr_ControlBarrier_P(UINT8 nValue)
{
	wnBRR_OUT_CS_ADDR = Add_200_Value;

	switch(nValue)
	{
	case ENTRY_OPEN :
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 2) & 0xFF));
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 0) & 0xFF));
		break;
	case CLOSE :
		wnBRR_OUT_CS_ADDR |= (UINT8)(1 << 0);
		break;
	case EXIT_OPEN :
		wnBRR_OUT_CS_ADDR |= (UINT8)(((1 << 2) & 0xFF));
		wnBRR_OUT_CS_ADDR  &= (UINT8)((~(1 << 0) & 0xFF));
		break;
	}

	Add_200_Value = wnBRR_OUT_CS_ADDR;
}

void Brr_ControlBarrier_S(UINT8 nValue)
{
    wnBRR_OUT_CS_ADDR = Add_200_Value;

	switch(nValue)
	{
	case ENTRY_OPEN :
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 6) & 0xFF));
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 4) & 0xFF));
		break;
	case CLOSE :
		wnBRR_OUT_CS_ADDR |= (UINT8)(1 << 4);
		break;
	case EXIT_OPEN :
		wnBRR_OUT_CS_ADDR |= (UINT8)(((1 << 6) & 0xFF));
		wnBRR_OUT_CS_ADDR  &= (UINT8)((~(1 << 4) & 0xFF));
		break;
	}

	Add_200_Value = wnBRR_OUT_CS_ADDR;
}

void Brr_ControlBarrier_All_OPnC_Dir(UINT8 nValue)		//open/close
{

    wnBRR_OUT_CS_ADDR = Add_200_Value;

	switch(nValue)
	{
	case OPEN :
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 4) & 0xFF));
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 0) & 0xFF));
		break;
	case CLOSE :
		wnBRR_OUT_CS_ADDR |= (UINT8)(1 << 4);
		wnBRR_OUT_CS_ADDR |= (UINT8)(1 << 0);
		break;
	}

	Add_200_Value = wnBRR_OUT_CS_ADDR;
}


void Brr_ControlBarrier_All_Dir(UINT8 nValue)		//direction
{

    wnBRR_OUT_CS_ADDR = Add_200_Value;

	switch(nValue)
	{
	case ENTRY :
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 2) & 0xFF));
		wnBRR_OUT_CS_ADDR &= (UINT8)((~(1 << 6) & 0xFF));
		break;
	case EXIT :
		wnBRR_OUT_CS_ADDR |= (UINT8)(((1 << 2) & 0xFF));
		wnBRR_OUT_CS_ADDR |= (UINT8)(((1 << 6) & 0xFF));
		break;
	}
	Add_200_Value = wnBRR_OUT_CS_ADDR;
}


void Brr_SetBreak_A(bool isActive)
{
	Add_200_Value &= 0xFF;

	switch(isActive)
	{
	case TRUE:
		Add_200_Value |= ((UINT8) ON << 3);
		Add_200_Value |= ((UINT8) ON << 7);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 3) & 0xFF));
		Add_200_Value &= (UINT8)((~(ON << 7) & 0xFF));
		break;

	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}


void Brr_SetEmergency_A(bool isActive)
{
	Add_200_Value &= 0xFF;

	switch(isActive)
	{
	case TRUE:
		Add_200_Value  |= ((UINT8) ON << 1);
		Add_200_Value  |= ((UINT8) ON << 5);
		break;
	case FALSE:
		Add_200_Value &= (UINT8)((~(ON << 1) & 0xFF));
		Add_200_Value &= (UINT8)((~(ON << 5) & 0xFF));
		break;

	}

	wnBRR_OUT_CS_ADDR = Add_200_Value;
}


void Barrier_Val(void)
{
	printf("Address_Barrier_Write = [0x%02X]\n", Add_200_Value);
}

