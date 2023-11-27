/*******************************************************************************
*       COPYRIGHT �� 2023 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : test.c
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : 
*******************************************************************************/

#include <string.h>
#include "userlib.h"
#include "serial.h"
#include "test.h"
#include "repository.h"
#include "common_func.h"
#include "bsp.h"

volatile T_TEST_OPTION TestOption;

extern uint8_t Add_200_Value;

int nBuzzOpSeq = 0;
int nSDoorOpSeq = 0;
int nPDoorOpSeq = 0;
int nLampOpSeq = 0;
int nDirOpSeq = 0;
int nOverOpSeq = 0;

void TestProcess(void)
{
	BYTE testData;

	if (U0_Getch(&testData))
		U0_Putch(testData);
}

void TestDirection(void)
{
	BYTE bColor = DIR_OFF;

	if (nDirOpSeq)
	{
		if (!TestOption.b.fTestDir)
		{
			nDirOpSeq = 0;
			bColor = DIR_OFF;
		}
		else
		{
			switch (nDirOpSeq)
			{
			case 1:
				bColor = STATE_ALLOW;
				nDirOpSeq++;
				break;
			case 2:
				bColor = STATE_STOP;
				nDirOpSeq++;
				break;
			case 3:
				bColor = DIR_OFF;
				nDirOpSeq = 1;
				break;
			}
		}
		ControlStatusLED_Ex(bColor);
		ControlStatusLED_En(bColor);
	}
	else if (TestOption.b.fTestDir)
	{
		nDirOpSeq = 1;
	}
}

void TestOverHeadDirection(void)
{
	BYTE bColor = DIR_OFF;

	if (nOverOpSeq)
	{
		if (!TestOption.b.fTestOverHeadLamp)
		{
			nOverOpSeq = 0;
			bColor = DIR_OFF;
		}
		else
		{
			switch (nOverOpSeq)
			{
			case 1:
				bColor = STATE_ALLOW;
				nOverOpSeq++;
				break;
			case 2:
				bColor = STATE_STOP;
				nOverOpSeq++;
				break;
			case 3:
				bColor = DIR_OFF;
				nOverOpSeq = 1;
				break;
			}
		}
		ControlOverHeadDisplay_Ex(bColor);
		ControlOverHeadDisplay_En(bColor);
	}
	else if (TestOption.b.fTestOverHeadLamp)
	{
		nOverOpSeq = 1;
	}
}

void TestLamp(void)
{

	BYTE bColor = LAMP_OFF;
	if (nLampOpSeq)
	{
		if (!TestOption.b.fTestLamp)
		{
			nLampOpSeq = 0;
			bColor = LAMP_OFF;
		}
		else
		{
			switch (nLampOpSeq)
			{
			case 1:
				bColor = LAMP_RED_ON;
				nLampOpSeq++;
				break;
			case 2:
				bColor = LAMP_GREEN_ON;
				nLampOpSeq++;
				break;
			case 3:
				bColor = LAMP_BLUE_ON;
				nLampOpSeq++;
				break;
			case 4:
				bColor = LAMP_RED_ON + LAMP_GREEN_ON;
				nLampOpSeq++;
				break;
			case 5:
				bColor = LAMP_RED_ON + LAMP_BLUE_ON;
				nLampOpSeq++;
				break;
			case 6:
				bColor = LAMP_BLUE_ON + LAMP_GREEN_ON;
				nLampOpSeq++;
				break;
			case 7:
				bColor = LAMP_RED_ON + LAMP_BLUE_ON + LAMP_GREEN_ON;
				nLampOpSeq = 1;
				break;
			}
		}
		ControlIndicatorLight_En(bColor);
		ControlIndicatorLight_Ex(bColor);
	}
	else if (TestOption.b.fTestLamp)
	{
		nLampOpSeq = 1;
	}
}

void TestDoor_P(void)
{
	BYTE bDir = ENTRY_OPEN;

	if (nPDoorOpSeq)
	{
		if (!TestOption.b.fTestBarrier)
		{
			nPDoorOpSeq = 0;
			Brr_SetEmergency_P(TRUE);
			bDir = ENTRY_OPEN;
		}
		else
		{
			switch (nPDoorOpSeq)
			{
			case 1:
				bDir = ENTRY_OPEN;
				nPDoorOpSeq++;
				break;
			case 2:
				bDir = CLOSE;
				nPDoorOpSeq++;
				break;
			case 3:
				bDir = EXIT_OPEN;
				nPDoorOpSeq++;
				break;
			case 4:
				bDir = CLOSE;
				nPDoorOpSeq = 1;
				break;
			}
		}
		Brr_ControlBarrier_P(bDir);
	}
	else if (TestOption.b.fTestBarrier)
	{
		nPDoorOpSeq = 1;
		Brr_SetEmergency_P(FALSE);
	}
}

void TestDoor_S(void)
{
	BYTE bDir = ENTRY_OPEN;

	if (nSDoorOpSeq)
	{
		if (!TestOption.b.fTestBarrier)
		{
			nSDoorOpSeq = 0;
			Brr_SetEmergency_S(TRUE);
			bDir = ENTRY_OPEN;
		}
		else
		{
			switch (nSDoorOpSeq)
			{
			case 1:
				bDir = ENTRY_OPEN;
				nSDoorOpSeq++;
				break;
			case 2:
				bDir = CLOSE;
				nSDoorOpSeq++;
				break;
			case 3:
				bDir = EXIT_OPEN;
				nSDoorOpSeq++;
				break;
			case 4:
				bDir = CLOSE;
				nSDoorOpSeq = 1;
				break;
			}
		}
		Brr_ControlBarrier_S(bDir);
	}
	else if (TestOption.b.fTestBarrier)
	{
		nSDoorOpSeq = 1;
		Brr_SetEmergency_S(FALSE);
	}
}

void TestBuzzer(void)
{

	if (nBuzzOpSeq)
	{
		if (!TestOption.b.fTestBuzzer)
		{
			nBuzzOpSeq = 0;
		}
		else
		{
			switch (nBuzzOpSeq)
			{
			case 1:
				nBuzzOpSeq++;
				break;
			case 2:
				nBuzzOpSeq--;
				break;
			}
		}

		if (nBuzzOpSeq == 1)
		{
			BuzzerOn();
		}
		else
		{
			BuzzerOff();
		}
	}
	else if (TestOption.b.fTestBuzzer)
	{
		nBuzzOpSeq = 1;
	}
}

void Write_IO_init(void)
{

	switch (TestOption.bVal)
	{
	case 0x00:
		break;
	case 0x01:
		break;
	case 0x02:
		ControlStatusLED_Ex(DIR_OFF);
		ControlStatusLED_En(DIR_OFF);
		break;
	case 0x04:
		ControlIndicatorLight_En(LAMP_OFF);
		ControlIndicatorLight_Ex(LAMP_OFF);
		break;
	case 0x08:
		ControlOverHeadDisplay_Ex(DIR_OFF);
		ControlOverHeadDisplay_En(DIR_OFF);
		break;
	case 0x10:
		Brr_SetEmergency_P(TRUE);
		break;
	case 0x20:
		Brr_SetEmergency_S(TRUE);
		break;
	case 0x40:
		BuzzerOff();
		break;
	case 0x80:
		break;

	default:

		ControlStatusLED_Ex(DIR_OFF);
		ControlStatusLED_En(DIR_OFF);

		ControlIndicatorLight_En(LAMP_OFF);
		ControlIndicatorLight_Ex(LAMP_OFF);

		ControlOverHeadDisplay_Ex(DIR_OFF);
		ControlOverHeadDisplay_En(DIR_OFF);

		Brr_SetEmergency_P(TRUE);

		Brr_SetEmergency_S(TRUE);

		BuzzerOff();
		break;
	}

	nBuzzOpSeq 	= 0;
	nSDoorOpSeq = 0;
	nPDoorOpSeq = 0;
	nLampOpSeq 	= 0;
	nDirOpSeq 	= 0;
	nOverOpSeq 	= 0;
}

void TestAging(void)
{
	static DWORD loop = 0;	// 1 loop = 10ms

	// Every 2 seconds, the test function will run.
	if (!loop)
	{
		TestOption.bVal = ReadDipSwitch();
		TestDirection();
		TestOverHeadDirection();
		TestLamp();
		TestDoor_P();
		TestDoor_S();
		TestBuzzer();
	}

	loop++;
	if (loop >= 3000)
		loop = 0;
}

void TEST_LAMP_P_Cmd(void)
{
	static int nLampOpSeq_cmd = 0;
	BYTE bColor = DIR_OFF;
	switch (nLampOpSeq_cmd)
	{
	case 0:
		bColor = LAMP_OFF;
		nLampOpSeq_cmd++;
		break;
	case 1:
		bColor = LAMP_RED_ON;
		nLampOpSeq_cmd++;
		break;
	case 2:
		bColor = LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 3:
		bColor = LAMP_BLUE_ON;
		nLampOpSeq_cmd++;
		break;
	case 4:
		bColor = LAMP_RED_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 5:
		bColor = LAMP_RED_ON + LAMP_BLUE_ON;
		nLampOpSeq_cmd++;
		break;
	case 6:
		bColor = LAMP_BLUE_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 7:
		bColor = LAMP_RED_ON + LAMP_BLUE_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd = 0;
		break;
	}

	ControlIndicatorLight_En(bColor);
	//	test1 = (wnBRR_OUT_CS_ADDR & 0xFF);
	// ControlIndicatorLight_Ex(bColor);
}

void TEST_LAMP_S_Cmd(void)
{
	static int nLampOpSeq_cmd = 0;
	BYTE bColor = DIR_OFF;
	switch (nLampOpSeq_cmd)
	{
	case 0:
		bColor = LAMP_OFF;
		nLampOpSeq_cmd++;
		break;
	case 1:
		bColor = LAMP_RED_ON;
		nLampOpSeq_cmd++;
		break;
	case 2:
		bColor = LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 3:
		bColor = LAMP_BLUE_ON;
		nLampOpSeq_cmd++;
		break;
	case 4:
		bColor = LAMP_RED_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 5:
		bColor = LAMP_RED_ON + LAMP_BLUE_ON;
		nLampOpSeq_cmd++;
		break;
	case 6:
		bColor = LAMP_BLUE_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd++;
		break;
	case 7:
		bColor = LAMP_RED_ON + LAMP_BLUE_ON + LAMP_GREEN_ON;
		nLampOpSeq_cmd = 0;
		break;
	}

	// ControlIndicatorLight_En(bColor);
	//	test1 = (wnBRR_OUT_CS_ADDR & 0xFF);
	ControlIndicatorLight_Ex(bColor);
}

void TEST_Barrier_P_Cmd(void)
{
	static int nPDoorOpSeq_cmd = 0;
	BYTE bDir = DIR_OFF;

	switch (nPDoorOpSeq_cmd)
	{
	case 0:
		bDir = CLOSE;
		nPDoorOpSeq_cmd++;
		break;
	case 1:
		bDir = EXIT_OPEN;
		nPDoorOpSeq_cmd++;
		break;
	case 2:
		bDir = CLOSE;
		nPDoorOpSeq_cmd++;
		break;
	case 3:
		bDir = ENTRY_OPEN;
		nPDoorOpSeq_cmd = 0;
		break;
	}

	Brr_ControlBarrier_P(bDir);
}

void TEST_Barrier_S_Cmd(void)
{
	static int nPDoorOpSeq_cmd = 0;
	BYTE bDir = DIR_OFF;

	switch (nPDoorOpSeq_cmd)
	{
	case 0:
		bDir = CLOSE;
		nPDoorOpSeq_cmd++;
		break;
	case 1:
		bDir = EXIT_OPEN;
		nPDoorOpSeq_cmd++;
		break;
	case 2:
		bDir = CLOSE;
		nPDoorOpSeq_cmd++;
		break;
	case 3:
		bDir = ENTRY_OPEN;
		nPDoorOpSeq_cmd = 0;
		break;
	}

	Brr_ControlBarrier_S(bDir);
}

void TEST_OverHeadDirection_Cmd(void)
{
	static int nDirOpSeq_cmd = 0;
	BYTE bColor = DIR_OFF;

	switch (nDirOpSeq_cmd)
	{
	case 0:
		bColor = STATE_ALLOW;
		nDirOpSeq_cmd++;
		break;
	case 1:
		bColor = STATE_STOP;
		nDirOpSeq_cmd++;
		break;
	case 2:
		bColor = STATE_ALL;
		nDirOpSeq_cmd++;
		break;
	case 3:
		bColor = DIR_OFF;
		nDirOpSeq_cmd = 0;
		break;
	}

	ControlOverHeadDisplay_Ex(bColor);
	ControlOverHeadDisplay_En(bColor);
}

void TEST_Direction_Cmd(void)
{
	static int nDirOpSeq_cmd = 0;
	BYTE bColor = DIR_OFF;

	switch (nDirOpSeq_cmd)
	{
	case 0:
		bColor = STATE_ALLOW;
		nDirOpSeq_cmd++;
		break;
	case 1:
		bColor = STATE_STOP;
		nDirOpSeq_cmd++;
		break;
	case 2:
		bColor = STATE_ALL;
		nDirOpSeq_cmd++;
		break;
	case 3:
		bColor = DIR_OFF;
		nDirOpSeq_cmd = 0;
		break;
	}

	ControlStatusLED_Ex(bColor);
	ControlStatusLED_En(bColor);
}

void TEST_Buzzer_Cmd(void)
{
	static int nBuzzOpSeq_cmd = 0;

	switch (nBuzzOpSeq_cmd)
	{
	case 0:
		nBuzzOpSeq_cmd++;
		break;

	case 1:
		nBuzzOpSeq_cmd--;
		break;
	}

	if (nBuzzOpSeq_cmd == 1)
	{
		BuzzerOn();
	}
	else
	{
		BuzzerOff();
	}
}

void TEST_Barrier_P_E_Cmd(void)
{
	static int nPDoorOpSeq_cmd = 0;

	switch (nPDoorOpSeq_cmd)
	{
	case 0:
		nPDoorOpSeq_cmd++;
		break;

	case 1:
		nPDoorOpSeq_cmd--;
		break;
	}

	if (nPDoorOpSeq_cmd == 1)
	{
		Brr_SetEmergency_P(TRUE);
	}
	else
	{
		Brr_SetEmergency_P(FALSE);
	}
}

void TEST_Barrier_S_E_Cmd(void)
{
	static int nPDoorOpSeq_cmd = 0;

	switch (nPDoorOpSeq_cmd)
	{
	case 0:
		nPDoorOpSeq_cmd++;
		break;

	case 1:
		nPDoorOpSeq_cmd--;
		break;
	}

	if (nPDoorOpSeq_cmd == 1)
	{
		Brr_SetEmergency_S(TRUE);
	}
	else
	{
		Brr_SetEmergency_S(FALSE);
	}
}

void TEST_Barrier_P_B_Cmd(void)
{
	static int nPDoorOpSeqP_cmd = 0;

	switch (nPDoorOpSeqP_cmd)
	{
	case 0:
		nPDoorOpSeqP_cmd++;
		break;

	case 1:
		nPDoorOpSeqP_cmd--;
		break;
	}

	if (nPDoorOpSeqP_cmd == 1)
	{
		Brr_SetBreak_P(TRUE);
	}
	else
	{
		Brr_SetBreak_P(FALSE);
	}
}

void TEST_Barrier_S_B_Cmd(void)
{
	static int nPDoorOpSeqS_cmd = 0;

	switch (nPDoorOpSeqS_cmd)
	{
	case 0:
		nPDoorOpSeqS_cmd++;
		break;

	case 1:
		nPDoorOpSeqS_cmd--;
		break;
	}

	if (nPDoorOpSeqS_cmd == 1)
	{
		Brr_SetBreak_S(TRUE);
	}
	else
	{
		Brr_SetBreak_S(FALSE);
	}
}

void TEST_Barrier_A_B_Cmd(void)
{
	static int nPDoorOpSeqA_cmd = 0;

	switch (nPDoorOpSeqA_cmd)
	{
	case 0:
		nPDoorOpSeqA_cmd++;
		break;

	case 1:
		nPDoorOpSeqA_cmd--;
		break;
	}

	if (nPDoorOpSeqA_cmd == 1)
	{
		Brr_SetBreak_A(TRUE);
	}
	else
	{
		Brr_SetBreak_A(FALSE);
	}
}

void TEST_Barrier_A_E_Cmd(void)
{
	static int nPDoorOpSeqA_cmd = 0;

	switch (nPDoorOpSeqA_cmd)
	{
	case 0:
		nPDoorOpSeqA_cmd++;
		break;

	case 1:
		nPDoorOpSeqA_cmd--;
		break;
	}

	if (nPDoorOpSeqA_cmd == 1)
	{
		Brr_SetEmergency_A(TRUE);
	}
	else
	{
		Brr_SetEmergency_A(FALSE);
	}
}

void TEST_Barrier_A_Dir_Cmd(void) // direction
{
	static int nPDoorOpSeqA_Dir_cmd = 0;

	switch (nPDoorOpSeqA_Dir_cmd)
	{
	case 0:
		nPDoorOpSeqA_Dir_cmd++;
		break;

	case 1:
		nPDoorOpSeqA_Dir_cmd--;
		break;
	}

	if (nPDoorOpSeqA_Dir_cmd == 1)
	{
		Brr_ControlBarrier_All_Dir(ENTRY);
		printf("Setting ENTRY Direction \n");
	}
	else
	{
		Brr_ControlBarrier_All_Dir(EXIT);
		printf("Setting EXIT Direction \n");
	}
}

void TEST_Barrier_A_OPnC_Cmd(void) // open/close
{
	static int nPDoorOpSeqA_OPnC_cmd = 0;

	switch (nPDoorOpSeqA_OPnC_cmd)
	{
	case 0:
		nPDoorOpSeqA_OPnC_cmd++;
		break;

	case 1:
		nPDoorOpSeqA_OPnC_cmd--;
		break;
	}

	if (nPDoorOpSeqA_OPnC_cmd == 1)
	{
		Brr_ControlBarrier_All_OPnC_Dir(OPEN);
	}
	else
	{
		Brr_ControlBarrier_All_OPnC_Dir(CLOSE);
	}
}

void CMD_Test(byte bVal)
{

	switch (bVal)
    {
	    	case 0x31:			//1
	    		Brr_Inq_Status();
	    		break;
	case 0x32: // 2
	    		TEST_LAMP_P_Cmd();
	    		TEST_LAMP_S_Cmd();
	    		break;
	case 0x33: // 3
	    		TEST_Direction_Cmd();
	    		break;
	case 0x34: // 4
	    		TEST_OverHeadDirection_Cmd();
	    		break;
	case 0x35: // 5
	    		TEST_Barrier_P_Cmd();
	    		break;
	case 0x36: // 6
	    		TEST_Barrier_S_Cmd();
	    		break;
	case 0x37: // 7
	    		TEST_Buzzer_Cmd();
	    		break;
	case 0x42: // B break s
	case 0x62: // b break s
	    		TEST_Barrier_S_B_Cmd();
	    		TEST_Barrier_P_B_Cmd();
	    		break;
	case 0x45: // E	Emergency s
	case 0x65: // e	Emergency s
	    		TEST_Barrier_S_E_Cmd();
	    		TEST_Barrier_P_E_Cmd();
	    		break;
	case 0x43: // C	close
	case 0x63: // c	close
		if (Add_200_Value & 0x88)
		    		{
		         		printf("Setting break \n");
		    		}
		else if (Add_200_Value & 0x22)
		    		{
			    		printf("Setting Emergency \n");
		    		}
		    		else
		    		{
		    			Brr_ControlBarrier_All_OPnC_Dir(CLOSE);
		    		}
	    		break;
	case 0x4E: // N	entry
	case 0x6E: // n	entry
		if (Add_200_Value & 0x88)
	    		{
	         		printf("Setting break \n");
	    		}
		else if (Add_200_Value & 0x22)
	    		{
		    		printf("Setting Emergency \n");
	    		}
	    		else
	    		{
					Brr_ControlBarrier_All_OPnC_Dir(OPEN);
					Brr_ControlBarrier_All_Dir(ENTRY);
					printf("Setting ENTRY Direction Open\n");
	    		}
	    		break;
	case 0x58: // X	exit
	case 0x78: // x	exit
		if (Add_200_Value & 0x88)
	    		{
	         		printf("Setting break \n");
	    		}
		else if (Add_200_Value & 0x22)
	    		{
		    		printf("Setting Emergency \n");
	    		}
	    		else
	    		{
					Brr_ControlBarrier_All_OPnC_Dir(OPEN);
					Brr_ControlBarrier_All_Dir(EXIT);
					printf("Setting EXIT Direction Open\n");
	    		}
	    		break;
	case 0x30: // Barrier_Value
	    		Barrier_Val();
	    		break;
    }
}

/*****END OF FILE****/
