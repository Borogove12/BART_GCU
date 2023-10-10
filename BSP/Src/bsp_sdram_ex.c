/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_sdram_ex.h
 *  모듈 설명    : 8M SDRAM(IS42S16400J) 제어 루틴 확장 소스
 *                 STM32Cube Lib(STM32Cube FW H7) 참조
 *
 *  작성자       : 김정준
 *  작성일       : 2023/02/07
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */


#include "bsp.h"


SDRAM_HandleTypeDef hsdram[SDRAM_INSTANCES_NBR];

static FMC_SDRAM_CommandTypeDef Command;
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
static uint32_t IsMspCallbacksValid = 0;
#endif


static void SDRAM_MspInit(SDRAM_HandleTypeDef  *hSdram);
static void SDRAM_MspDeInit(SDRAM_HandleTypeDef  *hSdram);


static int32_t IS42S16400J_Delay(uint32_t Delay)
{
    DWORD dwStart = bsp_GetTickCount();
    while ((bsp_GetTickCount() - dwStart) < Delay)
    {
    }
    return IS42S16400J_OK;
}



int32_t IS42S16400J_Init(SDRAM_HandleTypeDef* Ctx, IS42S16400J_Context_t* pRegMode)
{
    int32_t ret = IS42S16400J_ERROR;

    /* Step 1: Configure a clock configuration enable command */
    if (IS42S16400J_ClockEnable(Ctx, pRegMode->TargetBank) == IS42S16400J_OK)
    {
        /* Step 2: Insert 100 us minimum delay */
        /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
        (void) IS42S16400J_Delay(1);

        /* Step 3: Configure a PALL (precharge all) command */
        if (IS42S16400J_Precharge(Ctx, pRegMode->TargetBank) == IS42S16400J_OK)
        {
            /* Step 4: Configure a Refresh command */
            if (IS42S16400J_RefreshMode(Ctx, pRegMode->TargetBank, pRegMode->RefreshMode) == IS42S16400J_OK)
            {
                /* Step 5: Program the external memory mode register */
                if (IS42S16400J_ModeRegConfig(Ctx, pRegMode) == IS42S16400J_OK)
                {
                    /* Step 6: Set the refresh rate counter */
                    if (IS42S16400J_RefreshRate(Ctx, pRegMode->RefreshRate) == IS42S16400J_OK)
                    {
                        ret = IS42S16400J_OK;
                    }
                }
            }
        }
    }
    return ret;
}


/*
**  Enable SDRAM clock
**
**  Interface Could be FMC_SDRAM_CMD_TARGET_BANK1 or FMC_SDRAM_CMD_TARGET_BANK2
*/
int32_t IS42S16400J_ClockEnable(SDRAM_HandleTypeDef* Ctx, uint32_t Interface)
{
    Command.CommandMode = IS42S16400J_CLK_ENABLE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Precharge all sdram banks
**
**  Interface Could be FMC_SDRAM_CMD_TARGET_BANK1 or FMC_SDRAM_CMD_TARGET_BANK2
*/
int32_t IS42S16400J_Precharge(SDRAM_HandleTypeDef* Ctx, uint32_t Interface)
{
    Command.CommandMode = IS42S16400J_PALL_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Program the external memory mode register
**
**  pRegMode: Pointer to Register Mode structure
*/
int32_t IS42S16400J_ModeRegConfig(SDRAM_HandleTypeDef *Ctx, IS42S16400J_Context_t *pRegMode)
{
    uint32_t tmpmrd;

    /* Program the external memory mode register */
    tmpmrd = (uint32_t)pRegMode->BurstLength   |\
                       pRegMode->BurstType     |\
                       pRegMode->CASLatency    |\
                       pRegMode->OperationMode |\
                       pRegMode->WriteBurstMode;

    Command.CommandMode = IS42S16400J_LOAD_MODE_CMD;
    Command.CommandTarget = pRegMode->TargetBank;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Program the SDRAM timing
**
**  pTiming Pointer to SDRAM timing configuration structure
*/
int32_t IS42S16400J_TimingConfig(SDRAM_HandleTypeDef* Ctx, FMC_SDRAM_TimingTypeDef* pTiming)
{
    /* Program the SDRAM timing */
    if (HAL_SDRAM_Init(Ctx, pTiming) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Configure Refresh mode
**
**  Interface Could be FMC_SDRAM_CMD_TARGET_BANK1 or FMC_SDRAM_CMD_TARGET_BANK2
**  RefreshMode Could be IS42S16400J_CMD_AUTOREFRESH_MODE or IS42S16400J_CMD_SELFREFRESH_MODE
*/
int32_t IS42S16400J_RefreshMode(SDRAM_HandleTypeDef* Ctx, uint32_t Interface, uint32_t RefreshMode)
{
    Command.CommandMode = RefreshMode;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Set the device refresh rate
**
**  RefreshCount The refresh rate to be programmed
*/
int32_t IS42S16400J_RefreshRate(SDRAM_HandleTypeDef* Ctx, uint32_t RefreshCount)
{
    /* Set the device refresh rate */
    if (HAL_SDRAM_ProgramRefreshRate(Ctx, RefreshCount) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Enter Power mode
**
**  Interface Could be FMC_SDRAM_CMD_TARGET_BANK1 or FMC_SDRAM_CMD_TARGET_BANK2
*/
int32_t IS42S16400J_EnterPowerMode(SDRAM_HandleTypeDef* Ctx, uint32_t Interface)
{
    Command.CommandMode = IS42S16400J_POWERDOWN_MODE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}



/*
**  Exit Power mode
**
**  Interface Could be FMC_SDRAM_CMD_TARGET_BANK1 or FMC_SDRAM_CMD_TARGET_BANK2
*/

int32_t IS42S16400J_ExitPowerMode(SDRAM_HandleTypeDef* Ctx, uint32_t Interface)
{
    Command.CommandMode = IS42S16400J_NORMAL_MODE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}


/*
**  Sends command to the SDRAM bank.
**
**  SdramCmd: Pointer to SDRAM command structure
*/

int32_t IS42S16400J_Sendcmd(SDRAM_HandleTypeDef* Ctx, FMC_SDRAM_CommandTypeDef* SdramCmd)
{
    if (HAL_SDRAM_SendCommand(Ctx, SdramCmd, IS42S16400J_TIMEOUT) != HAL_OK)
    {
        return IS42S16400J_ERROR;
    }
    else
    {
        return IS42S16400J_OK;
    }
}



/*****************************************************************************/


/*
**  Initializes the SDRAM device.
**
**  Instance  SDRAM Instance(always 0)
*/
int32_t BSP_SDRAM_Init(uint32_t Instance)
{
    int32_t ret;
    static IS42S16400J_Context_t pRegMode;

    if (Instance >= SDRAM_INSTANCES_NBR)
    {
        ret =  BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        ret = BSP_ERROR_NONE;
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
        /* Register the SDRAM MSP Callbacks */
        if (IsMspCallbacksValid == 0)
        {
            if (BSP_SDRAM_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
            {
                ret = BSP_ERROR_PERIPH_FAILURE;
            }
        }
#else
        /* Msp SDRAM initialization */
        SDRAM_MspInit(&hsdram[Instance]);
#endif /* USE_HAL_SDRAM_REGISTER_CALLBACKS */

        if (ret == BSP_ERROR_NONE)
        {
            if (MX_SDRAM_BANK2_Init(&hsdram[Instance]) != HAL_OK)
            {
                ret = BSP_ERROR_NO_INIT;
            }
            else
            {
                /* External memory mode register configuration */
                pRegMode.TargetBank      = FMC_SDRAM_CMD_TARGET_BANK2;
                pRegMode.RefreshMode     = IS42S16400J_AUTOREFRESH_MODE_CMD;
                pRegMode.RefreshRate     = REFRESH_COUNT;
                pRegMode.BurstLength     = IS42S16400J_BURST_LENGTH_1;
                pRegMode.BurstType       = IS42S16400J_BURST_TYPE_SEQUENTIAL;
                pRegMode.CASLatency      = IS42S16400J_CAS_LATENCY_3;
                pRegMode.OperationMode   = IS42S16400J_OPERATING_MODE_STANDARD;
                pRegMode.WriteBurstMode  = IS42S16400J_WRITEBURST_MODE_SINGLE;

                /* SDRAM initialization sequence */
                if (IS42S16400J_Init(&hsdram[Instance], &pRegMode) != IS42S16400J_OK)
                {
                    ret =  BSP_ERROR_COMPONENT_FAILURE;
                }
            }
        }
    }

    return ret;
}



/**
  * @brief  DeInitializes the SDRAM device.
  * @param  Instance  SDRAM Instance
  * @retval BSP status
  */
int32_t BSP_SDRAM_DeInit(uint32_t Instance)
{
    int32_t ret = BSP_ERROR_NONE;;

    if(Instance >= SDRAM_INSTANCES_NBR)
    {
        ret =  BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        /* SDRAM device de-initialization */
        hsdram[Instance].Instance = FMC_SDRAM_DEVICE;

        (void)HAL_SDRAM_DeInit(&hsdram[Instance]);
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 0)
        /* SDRAM controller de-initialization */
        SDRAM_MspDeInit(&hsdram[Instance]);
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 0) */
    }

    return ret;
}


/**
  * @brief  Initializes the SDRAM periperal.
  * @param  hSdram SDRAM handle
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SDRAM_BANK2_Init(SDRAM_HandleTypeDef *hSdram)
{
    FMC_SDRAM_TimingTypeDef sdram_timing;

    /* SDRAM device configuration */
    hSdram->Instance = FMC_SDRAM_DEVICE;

    /* SDRAM handle configuration */
    hSdram->Init.SDBank             = FMC_SDRAM_BANK2;
    hSdram->Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
    hSdram->Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
    hSdram->Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_16;
    hSdram->Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hSdram->Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_2;
    hSdram->Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hSdram->Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
    hSdram->Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
    hSdram->Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_1;

    /* Timing configuration for as SDRAM */
    sdram_timing.LoadToActiveDelay    = 3;
    sdram_timing.ExitSelfRefreshDelay = 10;
    sdram_timing.SelfRefreshTime      = 7;
    sdram_timing.RowCycleDelay        = 8;
    sdram_timing.WriteRecoveryTime    = 16;
    sdram_timing.RPDelay              = 3;
    sdram_timing.RCDDelay             = 3;

    /* SDRAM controller initialization */
    if(HAL_SDRAM_Init(hSdram, &sdram_timing) != HAL_OK)
    {
        return  HAL_ERROR;
    }

    return HAL_OK;
}

#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP SDRAM Msp Callbacks
  * @param Instance      SDRAM Instance
  * @retval BSP status
  */
int32_t BSP_SDRAM_RegisterDefaultMspCallbacks (uint32_t Instance)
{
    int32_t ret = BSP_ERROR_NONE;

    /* Check if the instance is supported */
    if(Instance >= SDRAM_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        /* Register MspInit/MspDeInit Callbacks */
        if (HAL_SDRAM_RegisterCallback(&hsdram[Instance],  HAL_SDRAM_MSP_INIT_CB_ID, SDRAM_MspInit) != HAL_OK)
        {
            ret = BSP_ERROR_PERIPH_FAILURE;
        }
        if (HAL_SDRAM_RegisterCallback(&hsdram[Instance], HAL_SDRAM_MSP_DEINIT_CB_ID, SDRAM_MspDeInit) != HAL_OK)
        {
            ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else
        {
            IsMspCallbacksValid = 1;
        }
    }
    /* Return BSP status */
    return ret;
}

/**
  * @brief BSP SDRAM Msp Callback registering
  * @param Instance     SDRAM Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_SDRAM_RegisterMspCallbacks (uint32_t Instance, BSP_SDRAM_Cb_t *CallBacks)
{
    int32_t ret = BSP_ERROR_NONE;

    /* Check if the instance is supported */
    if(Instance >= SDRAM_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        /* Register MspInit/MspDeInit Callbacks */
        if (HAL_SDRAM_RegisterCallback(&hsdram[Instance], HAL_SDRAM_MSP_INIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
        {
            ret = BSP_ERROR_PERIPH_FAILURE;
        }
        if (HAL_SDRAM_RegisterCallback(&hsdram[Instance], HAL_SDRAM_MSP_DEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
        {
            ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else
        {
            IsMspCallbacksValid = 1;
        }
    }
    /* Return BSP status */
    return ret;
}
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1) */


/**
  * @brief  Sends command to the SDRAM bank.
  * @param  Instance  SDRAM Instance
  * @param  SdramCmd  Pointer to SDRAM command structure
  * @retval BSP status
  */
int32_t BSP_SDRAM_SendCmd(uint32_t Instance, FMC_SDRAM_CommandTypeDef *SdramCmd)
{
    int32_t ret;

    if (Instance >= SDRAM_INSTANCES_NBR)
    {
        ret =  BSP_ERROR_WRONG_PARAM;
    }
    else if (IS42S16400J_Sendcmd(&hsdram[Instance], SdramCmd) != IS42S16400J_OK)
    {
        ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}


/**
  * @brief  This function handles SDRAM MDMA interrupt request.
  * @param  Instance SDRAM instance
  * @retval None
  */
void BSP_SDRAM_IRQHandler(uint32_t Instance)
{
    HAL_MDMA_IRQHandler(hsdram[Instance].hmdma);
}


/**
  * @brief  Initializes SDRAM MSP.
  * @param  hSdram SDRAM handle
  * @retval None
  */
static void SDRAM_MspInit(SDRAM_HandleTypeDef  *hSdram)
{
    static MDMA_HandleTypeDef mdma_handle;
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable FMC clock */
    __HAL_RCC_FMC_CLK_ENABLE();

    /* Enable chosen MDMAx clock */
    SDRAM_MDMAx_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* Common GPIO configuration */
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_PULLUP;
    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;

    /* GPIOC configuration */   //    NWE
    gpio_init_structure.Pin   = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    /* GPIOD configuration */   //    D2          D3          D13         D14          D15
                                //    D0          D1
    gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1 | GPIO_PIN_8| GPIO_PIN_9 | GPIO_PIN_10 |\
                                GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    /* GPIOE configuration */   //    NBL0        NBL1            D4          D5              D6
                                //    D7          D8              D9          D10             D11
                                //    D12
    gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  |\
                                GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                                GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* GPIOF configuration */   //    A0          A1              A2          A3          A4
                                //    A5          RAS             A6          A7          A8
                                //    A9
    gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  |\
                                GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                                GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);

    /* GPIOG configuration */   //    A10         A11             A14         A15
                                //    CLK         CAS
    gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_4 | GPIO_PIN_5 |\
                                GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* GPIOH configuration */   //    NE1         CKE1
    gpio_init_structure.Pin   = GPIO_PIN_6  | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOH, &gpio_init_structure);

    /* Configure common MDMA parameters */
    mdma_handle.Init.Request                  = MDMA_REQUEST_SW;
    mdma_handle.Init.TransferTriggerMode      = MDMA_BLOCK_TRANSFER;
    mdma_handle.Init.Priority                 = MDMA_PRIORITY_HIGH;
    mdma_handle.Init.Endianness               = MDMA_LITTLE_ENDIANNESS_PRESERVE;
    mdma_handle.Init.SourceInc                = MDMA_SRC_INC_WORD;
    mdma_handle.Init.DestinationInc           = MDMA_DEST_INC_WORD;
    mdma_handle.Init.SourceDataSize           = MDMA_SRC_DATASIZE_WORD;
    mdma_handle.Init.DestDataSize             = MDMA_DEST_DATASIZE_WORD;
    mdma_handle.Init.DataAlignment            = MDMA_DATAALIGN_PACKENABLE;
    mdma_handle.Init.SourceBurst              = MDMA_SOURCE_BURST_SINGLE;
    mdma_handle.Init.DestBurst                = MDMA_DEST_BURST_SINGLE;
    mdma_handle.Init.BufferTransferLength     = 128;
    mdma_handle.Init.SourceBlockAddressOffset = 0;
    mdma_handle.Init.DestBlockAddressOffset   = 0;
    mdma_handle.Instance                      = SDRAM_MDMAx_CHANNEL;

    /* Associate the MDMA handle */
    __HAL_LINKDMA(hSdram, hmdma, mdma_handle);

    /* De-initialize the Stream for new transfer */
    HAL_MDMA_DeInit(&mdma_handle);

    /* Configure the MDMA stream */
    (void)HAL_MDMA_Init(&mdma_handle);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SDRAM_MDMAx_IRQn, BSP_SDRAM_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(SDRAM_MDMAx_IRQn);
}


/*
**  DeInitializes SDRAM MSP.
**
**  hSdram SDRAM handle
*/
static void SDRAM_MspDeInit(SDRAM_HandleTypeDef  *hSdram)
{
    static MDMA_HandleTypeDef mdma_handle;

    /* Prevent unused argument(s) compilation warning */
    UNUSED(hSdram);
    /* Disable NVIC configuration for MDMA interrupt */
    HAL_NVIC_DisableIRQ(SDRAM_MDMAx_IRQn);

    /* Deinitialize the stream for new transfer */
    mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;
    (void)HAL_MDMA_DeInit(&mdma_handle);
}


