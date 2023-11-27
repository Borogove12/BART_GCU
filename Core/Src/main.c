/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "global.h"
#include "repository.h"
#include "bsp.h"
#include "Inithw.h"
#include "common_func.h"
#include "interface.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void GCUInitialize(void);

bool bDip_SW_Flag = FALSE;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const char GCUDate[] __attribute__((section (".gcu_date"))) = {0x20, 0x23, 0x10, 0x26, 0x12, 0x00, 0x00};
/* USER CODE END PV */
time_t  SysInitTime;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_FMC_Init(void);
void MX_SPI1_Init(void);     
void MX_TIM7_Init(void);
word dip_sw(void);
extern void CMD_Test(byte bVal);
/* USER CODE BEGIN PFP */
dword exiAddr[5] = {
		READ00_ADR,READ01_ADR,READ02_ADR,READ03_ADR,READ04_ADR};
dword extAddr[4] = {
		WRITE00_ADR,WRITE01_ADR,WRITE02_ADR,WRITE03_ADR};

uint8_t bData[5] = {0,};

CONFIG  config;    //configuration

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  MPU_Config();

  SCB_EnableICache();
  SCB_EnableDCache();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  SystemClock_Config();

  /* USER CODE BEGIN Init */
  InitPeripherals();
  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  bsp_InitTimer();
  bsp_InitUart();
  bsp_InitSpiFlash();
  bDip_SW_Flag = TRUE;
  GCUInitialize();
  SerialProcess();
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}



word dip_sw(void)
{
	word	idata;

	idata = (GPIOB->IDR)^0xff;

	return(idata & 0xff);
}


/*******************************************************************************
* Name:     GCUInitialize
* Function: Initialize GCU modules
* In:       None
* Out:      None
* Return:   void
* comments:
*******************************************************************************/

void GCUInitialize(void)
{
	//EcuEmergencyOff();
	BuzzerOff();

	//ControlStatusLED_En(DIR_RED);
	//ControlStatusLED_Ex(DIR_RED);
	ControlIndicatorLight_En(LAMP_OFF);
	ControlIndicatorLight_Ex(LAMP_OFF);
	ControlUPS_CheckOn();
	ControlUPS_ShutDownOff();
	SetDefaultOpMode();
	SetDefaultParameter();
	SetDefaultStatus();
	// Brr_SetSerialComm(TRUE);
	Brr_OpenBarrier(BARRIER_OPEN_FOR_EX);
}


inline void SerialProcess(void)
{
	for (;;)
	{
		ProcessComm_Barrier();
		ProcessComm_ECU();
	}
}


byte extinb(byte Add)			//test Read Address
{
	// in-port : 0 ~ 6
	if (Add >= 5)
		return (0);

	bData[Add] = inb(exiAddr[Add]);
	return (bData[Add]);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
