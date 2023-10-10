
#include "bsp.h"
#include "global.h"
#include "main.h"
/*
**  시스템 전체 시간 관리를 위한 변수를 인터럽트에서 관리
**
**  g_TickCount : 시스템 시작 후 0 부터 1ms 증가
**  g_Epoch : 1970-01-01 00:00:00 이후 현재 시간 초 단위 값을 가짐.(RTC와 연계됨)
**  g_exTimeTick : 1970-01-01 00:00:00 이후 현재 시간 milliSecond 단위 값을 가짐.
**
**  세 변수 모두 시스템 시작 혹은 리셋 후 0에서 부터 시작하나
**  현재 시간 관련 변수는 통신에서 받는 현재 시간을 설정하여 사용 한다.
*/

__IO UINT32 g_TickCount;        // 시스템 시작 후 1ms 증가 값 저장
__IO time_t g_Epoch;
__IO UINT64 g_exTimeTick;
__IO uint8_t g_lcdBackLightBrightness = 0;  // LCD BackLight 밝기 조정 값(0 - 10)

// 각 시간 단위 마다 불린 CallBack
static void (*s_Per1msCallBack)(void);
static void (*s_Per10msCallBack)(void);
static void (*s_Per100msCallBack)(void);
static void (*s_Per1SecCallBack)(void);


extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim1;
extern BOOL bCan_Recovery_flag;
void lv_tick_inc(uint32_t tick_period);

unsigned int unBuzzer_timer = 0;
extern bool bDip_SW_Flag;

extern dword exiAddr[5];

void bsp_InitTimer(void)
{
    HAL_TIM_Base_Start_IT(&htim7);
    //HAL_TIM_Base_Start_IT(&htim1);
}

DWORD bsp_GetTickCount(void)
{
    return g_TickCount;
}


time_t bsp_GetTime(void)
{
    return g_Epoch;
}


UINT64 bsp_GetTimeEx(void)
{
    return g_exTimeTick;
}



void bsp_SetTime(time_t uEpoch)
{
    DISABLE_INT();
    g_exTimeTick = uEpoch * 1000;
    g_Epoch = uEpoch;
    g_exTimeTick = uEpoch * 1000;
    ENABLE_INT();
}


void bsp_Set1msCallback(void (*Callback)(void))
{
    s_Per1msCallBack = Callback;
}


void bsp_Set10msCallback(void (*Callback)(void))
{
    s_Per10msCallBack = Callback;
}


void bsp_Set100msCallback(void (*Callback)(void))
{
    s_Per100msCallBack = Callback;
}


void bsp_Set1SecCallback(void (*Callback)(void))
{
    s_Per1SecCallBack = Callback;
}


// 밝기값 : 0 ~ 10
void bsp_SetLcdBrightness(uint8_t nSet)
{
    if (nSet > 10)
        nSet = 10;

    g_lcdBackLightBrightness = nSet;
}


uint8_t bsp_GetLcdBrightness(void)
{
    return g_lcdBackLightBrightness;
}


/*
**  TIM7 Interrupy Handler에서 매 1ms 마다 불리는 Callback 함수
**
**  시스템 전체의 Epoch 변수와 1ms 단위 Tick을 관리, fnd Out 함수 호출
**
*/
unsigned int scount = 0;

int test_input_data = 0;
int test_temp_add = 0;
uint8_t bDataOld[5] = {0,};
uint8_t bDataOld1[5] = {0,};

//BYTE gbDipSW, gbOldDipSW;

void bsp_TimCallback(void)
{
    static UINT16   sCount = 0;
    static UINT32   s5Count = 0;
	char cha;
	char cTest;
	char cVal_dsw5,cVal_dsw10 = 0;
	char cVal_UPS_FAIL,cVal_UPS_BAT,cVal_UPS_CONN = 0;

    g_TickCount++;
    g_exTimeTick++;
    if (s_Per1msCallBack != NULL)
        s_Per1msCallBack();

    sCount++;
    s5Count++;

    if (sCount % 10 == 0)   // per 10ms
    {
        if (s_Per10msCallBack != NULL)
            s_Per10msCallBack();
    }
    if (sCount % 100 == 0)   // per 100ms
    {
    }
    if (sCount % 500 == 0)  // per 0.5 sec : WDT & STS Led toggle
    {
 		HAL_GPIO_TogglePin(WDT_GPIO_Port, WDT_Pin);
 		HAL_GPIO_TogglePin(STSLED_GPIO_Port, STS_LED1_Pin);
    }
    if (sCount >= 1000)     // per 1sec
    {
        sCount = 0;
        g_Epoch++;
        scount++;
        //printf("%s\n",strmtime());
      //  printf("in [0x%x]\n",inb(exiAddr[0]));
      printf("in [0x%x][0x%x][0x%x][0x%x][0x%x]\n",inb(exiAddr[0]),inb(exiAddr[1]),inb(exiAddr[2]),inb(exiAddr[3]),inb(exiAddr[4]));

       // cVal_EMG   = HAL_GPIO_ReadPin(EMG_GPIO_Port,EMG_Pin);					BART_Emgergency pin
        //cVal_sen9  = HAL_GPIO_ReadPin(PSEN_GPIO_Port,pSEN9_Pin);
        //cVal_sen19 = HAL_GPIO_ReadPin(PSEN_GPIO_Port,pSEN19_Pin);
      // cVal_dsw5  = HAL_GPIO_ReadPin(DipSW5_GPIO_Port,DipSW5_Pin);
        //cVal_dsw10 = HAL_GPIO_ReadPin(D_SW_GPIO_Port,D_SW10_Pin);

       // cVal_UPS_FAIL = HAL_GPIO_ReadPin(nUPS_LINE_FAIL_GPIO_Port,nUPS_LINE_FAIL_Pin);
      //  cVal_UPS_BAT  = HAL_GPIO_ReadPin(nUPS_LOW_BAT_GPIO_Port,nUPS_LOW_BAT_Pin);
       // cVal_UPS_CONN = HAL_GPIO_ReadPin(nUPS_CONN_GPIO_Port,nUPS_CONN_Pin);

       // printf("EMG in [0x%x]sen9[0x%x]sen19[0x%x]dsw5[0x%x]dsw10[0x%x] \n",cVal_EMG,cVal_sen9,cVal_sen19,cVal_dsw5,cVal_dsw10);
       // printf("UPS in FAIL[0x%x]BAT[0x%x]CONN[0x%x] \n",cVal_UPS_FAIL,cVal_UPS_BAT,cVal_UPS_CONN);
        //outb(WRITE00_ADR, 0x9C);
       HAL_GPIO_TogglePin(STSLED_GPIO_Port, STS_LED1_Pin);
       HAL_GPIO_TogglePin(STSLED_GPIO_Port, STS_LED2_Pin);
       HAL_GPIO_TogglePin(STSLED_GPIO_Port, STS_LED3_Pin);
       HAL_GPIO_TogglePin(STSLED_GPIO_Port, STS_LED4_Pin);



       //outb(WRITE00_ADR,test2);

        if (s_Per1SecCallBack != NULL)
            s_Per1SecCallBack();

    }

    if (s5Count >= 5000)     // per 5sec
    {
    	s5Count = 0;

    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{

    if (htim->Instance == htim7.Instance)
    {
        bsp_TimCallback();
    }
}


// microSecond delay 함수 - TIM1 이용 하여 debug/Release mode 상관없이
// 정확한 microseconds 만큼 기다린다.
// 16bit Counter Register를 이용 하므로 delay gksrPsms 65535 us 이다.
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}


BOOL IsTimeoutMs(DWORD *base, DWORD toMs)
{
    DWORD   tim;
    DWORD   diff;

    if (toMs == TIMEOUT_INFINITE)
        return (FALSE);

    tim = bsp_GetTickCount();
    if (toMs == 0)
    {
        *base = tim;
        return (TRUE);
    }

    if (tim >= *base)
        diff = tim - *base;
    else
    {
        diff = (0xffffffffUL - *base) + 1;
        diff += tim;
    }

    if(diff >= toMs)
    {
        *base  = tim;
        return (TRUE);
    }

    return (FALSE);
}


