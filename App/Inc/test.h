/*******************************************************************************
*       COPYRIGHT �� 2020 STraffic Co., Ltd. All rights reserved
********************************************************************************
* File Name    : test.c
* Author       : Joseph Yosep Choi
* Date Created : 01/17/2020
* Description  : 
*******************************************************************************/

#ifndef __TEST_H_
#define __TEST_H_

typedef union {
	volatile BYTE bVal;
	struct {
		volatile int fRFU:			 		 1;
		volatile int fTestDir:		 		 1;
		volatile int fTestLamp:		 		 1;
		volatile int fRFUforEMGSiganl:		 1;
		volatile int fTestOverHeadLamp:		 1;
		volatile int fTestBarrier:		 	 1;
		volatile int fTestBuzzer:	 		 1;
		volatile int fTestSol:		 		 1;
	} b;
} T_TEST_OPTION;

/* Functions prototypes ------------------------------------------------------*/
void TestProcess(void);
void TestAging(void);
void Write_IO_init(void);

#endif  // __TEST_H_

/*****END OF FILE****/