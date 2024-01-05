/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_rtc.c
 *  모듈 설명    : Real Time Clock 관련 루틴
 *                 (모든 시간은 UTC를 기준으로 LocalTime은 사용하지 않음)
 *
 *  작성자       : 김정준
 *  작성일       : 2022/05/02
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

/*
**  RTC Backup Register 0 :
**      RTC의 시간이 한번이라도 설정 되어서 시간이 유지되고 있을시 0x55AA32F4 값을 설정
**
**  RTC Backup Register 1 :
**      BOOT시 Program Update 시 프로그램 대상 미디어를 나타 냄.
**      0xAA55A151U - Xmodem Protocol로 Serial 통해서 프로그램 다운로드(현재 안쓰임)
**      0xAA55A252U - SPI Flash를 대상 미디어로 사용
**
** RTC Backup Register 2 : 업데이트 프로그램 크기를 기록
**
** * reg2, 3 는 프로그램 업데이트 완료 후 0로 클리어
*/


#include "bsp.h"
#include "bsp_rtc.h"
#include "global.h"
#include <time.h>
#include <stdio.h>


#define DEC2BCD(dec)    ((((BYTE)((dec) / 10)) << 4) + ((dec) % 10))
#define BCD2DEC(bcd)    (((((bcd) >> 4) & 0xf) * 10) + ((bcd) & 0xf))


extern RTC_HandleTypeDef hrtc;

// Porting 함수 쪽에서 정의 되어야 함
// BootLoader를 위한 임시 버전
/*__attribute__((weak)) time_t SystemInitTime(void)
{
    BYTE    bcdDTime[7] = { 0x20, 0x23, 0x10, 0x25, 0x12, 0x00, 0x00 };

    return (bsp_BcdDtimeToTime(bcdDTime));
}
*/

void bsp_SetDefaultTime(void)
{
    if ( HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != (uint32_t)RTCSET_MAGIC) // 설정된 적이 없으면
    {
        time_t sys = SystemInitTime();
        bsp_SetSystemTimeInEpocTime(sys);
    }
    else    // Timer 변수 설정.
    {
        time_t cur = bsp_GetSystemTimeInEpoc();
        bsp_SetTime(cur);
    }
}


// 부트로더에서만 호출
void bsp_InitRTC(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }

    /* Disable the write-protection */
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
    /* Wait until the shadow registers are synchronized */
    HAL_RTC_WaitForSynchro(&hrtc);
    /* Enable again the write-protection to prevent registers corruption */
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);

    bsp_SetDefaultTime();
}


// App에서 호출
void bsp_InitRTCforApp(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;

    bsp_SetDefaultTime();
}


static inline void PrintTmStruct(char *pTitle, struct tm *pTm)
{
    printf("%s : %04d-%02d-%02d %02d:%02d:%02d\n", pTitle, pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
            pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
}



static inline void RTCDTimeToTmStruct(RTC_DateTypeDef* pDate, RTC_TimeTypeDef *pTime, struct tm *ptm)
{
    ptm->tm_hour = pTime->Hours;        // 00 ~ 23
    ptm->tm_min = pTime->Minutes;       // 00 ~ 59
    ptm->tm_sec = pTime->Seconds;       // 00 ~ 59

    ptm->tm_mday = pDate->Date;         // 01 ~ 31
    ptm->tm_mon = pDate->Month - 1;     // 00 ~ 11
    ptm->tm_year = pDate->Year + 100;   // 2000 - 1900
}



static inline void TimeToRTCDTime(time_t t, RTC_DateTypeDef* prDate, RTC_TimeTypeDef *prTime)
{
    struct tm   *ptm;

    ptm = GmTime(&t);

    memset(prDate, 0x00, sizeof(RTC_DateTypeDef));
    memset(prTime, 0x00, sizeof(RTC_TimeTypeDef));

    prDate->Year = ptm->tm_year -100;
    prDate->Month = ptm->tm_mon + 1;
    prDate->Date = ptm->tm_mday;
    prDate->WeekDay = ptm->tm_wday;

    prTime->Hours = ptm->tm_hour;
    prTime->Minutes = ptm->tm_min;
    prTime->Seconds = ptm->tm_sec;
}

/*
** BCD 형식의 DateTime(yyyymmddhhmmss:7Byte)을 time_t로 변환
*/
time_t bsp_BcdDtimeToTime(PBYTE pbcdDTime)
{
    struct tm   tm;

    tm.tm_year = BCD2DEC(pbcdDTime[0])*100 + BCD2DEC(pbcdDTime[1]) - 1900;
    tm.tm_mon = BCD2DEC(pbcdDTime[2])-1;
    tm.tm_mday = BCD2DEC(pbcdDTime[3]);
    tm.tm_hour = BCD2DEC(pbcdDTime[4]);
    tm.tm_min = BCD2DEC(pbcdDTime[5]);
    tm.tm_sec = BCD2DEC(pbcdDTime[6]);

    return(mktime(&tm));
}



time_t bsp_SystemTimeToTime(PSYSTEMTIME pSysTime)
{
    struct tm tm;

    tm.tm_hour = pSysTime->wHour;   // 00 ~ 23
    tm.tm_min = pSysTime->wMinute;  // 00 ~ 59
    tm.tm_sec = pSysTime->wSecond;  // 00 ~ 59

    tm.tm_mday = pSysTime->wDay;    // 01 ~ 31
    tm.tm_mon = pSysTime->wMonth - 1;    // 00 ~ 11
    tm.tm_year = pSysTime->wYear - 1900;  // 1900-base

    return(mktime(&tm));
}


time_t bsp_RTCDTimeToTime(RTC_DateTypeDef* pDate, RTC_TimeTypeDef *pTime)
{
    struct tm tm;

    tm.tm_hour = pTime->Hours;      // 00 ~ 23
    tm.tm_min = pTime->Minutes;     // 00 ~ 59
    tm.tm_sec = pTime->Seconds;     // 00 ~ 59

    tm.tm_mday = pDate->Date;           // 01 ~ 31
    tm.tm_mon = pDate->Month - 1;       // 00 ~ 11
    tm.tm_year = pDate->Year + 100;     // 1900부터 시작

    return(mktime(&tm));
}



void bsp_SetSystemTimeInEpocTime(time_t cur)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    struct tm   *ptm;

    ptm = GmTime(&cur);

    memset(&sDate, 0x00, sizeof(RTC_DateTypeDef));
    memset(&sTime, 0x00, sizeof(RTC_TimeTypeDef));

    sDate.Year = ptm->tm_year - 100;
    sDate.Month = ptm->tm_mon + 1;
    sDate.Date = ptm->tm_mday;
    sDate.WeekDay = ptm->tm_wday;

    sTime.Hours = ptm->tm_hour;
    sTime.Minutes = ptm->tm_min;
    sTime.Seconds = ptm->tm_sec;

    bsp_SetTime(cur);
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0,  RTCSET_MAGIC);
}


void bsp_SetSystemTimeInBcdDTime(PBYTE pbcdDTime)
{
    time_t t;

    t = bsp_BcdDtimeToTime(pbcdDTime);
    bsp_SetSystemTimeInEpocTime(t);
}

/*
void bsp_SetSystemTimeInTm(struct tm* tm)
{
    time_t t;

    t = mktime(tm);
    bsp_SetSystemTimeInEpocTime(t);
}*/


time_t bsp_GetSystemTimeInEpoc(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    time_t  t;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    t = bsp_RTCDTimeToTime(&sDate, &sTime);
    return t;
}


void bsp_GetSystemTimeInBcdDtime(PBYTE pBcdDTime)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    pBcdDTime[0] = 0x20;
    pBcdDTime[1] = DEC2BCD(sDate.Year);
    pBcdDTime[2] = DEC2BCD(sDate.Month);
    pBcdDTime[3] = DEC2BCD(sDate.Date);
    pBcdDTime[4] = DEC2BCD(sTime.Hours);
    pBcdDTime[5] = DEC2BCD(sTime.Minutes);
    pBcdDTime[6] = DEC2BCD(sTime.Seconds);
}

/*
void bsp_GetSystemTimeInTm(struct tm* pRetTm)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    RTCDTimeToTmStruct(&sDate, &sTime, pRetTm);
}
*/

uint32_t bsp_ReadBackupReg(uint32_t BackupRegister)
{
	return (HAL_RTCEx_BKUPRead(&hrtc, BackupRegister));
}

void bsp_WriteBackupReg(uint32_t BackupRegister, uint32_t Data)
{
	HAL_RTCEx_BKUPWrite(&hrtc, BackupRegister, Data);
}



void Test_SetSystemTime(void)
{
    BYTE    bcdDTime[7] = { 0x20, 0x22, 0x09, 0x18, 0x16, 0x40, 0x00 };

    printf("Set SystemTime to '%02X%02X-%02X-%02X %02X:%02X:%02X'\n",
            bcdDTime[0], bcdDTime[1], bcdDTime[2], bcdDTime[3], bcdDTime[4], bcdDTime[5], bcdDTime[6]);

    bsp_SetSystemTimeInBcdDTime(bcdDTime);

}




/*
**  printf()가 64bit int를 출력 못함 -> %lld format 안 먹음
**  time_t, int64_t ... 등의 64비트 숫자 출력 위해서 ULARGE_INTEGER, LARGE_INTEGER 사용
*/
void Dump_AllTimeValue(void)
{
    ULARGE_INTEGER  ulExTime;
    ULARGE_INTEGER  ulCurTime;
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    struct tm *ptm;
    time_t  t;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    printf("RTC DTIME : %02d-%02d-%02d %02d:%02d:%02d\n", sDate.Year, sDate.Month, sDate.Date,
            sTime.Hours, sTime.Minutes, sTime.Seconds);

    t = bsp_GetTime();
    ptm = GmTime(&t);
    PrintTmStruct("Timer Int Time", ptm);

    ulCurTime.QuadPart = bsp_GetTime();
    ulExTime.QuadPart = bsp_GetTimeEx();

    printf("SYS Ticks : TickCount[%ld] Time[High : %lu, Low : %lu] ExTime[High : %lu, Low : %lu]\n",
            bsp_GetTickCount(), ulCurTime.u.HighPart, ulCurTime.u.LowPart, ulExTime.u.HighPart, ulExTime.u.LowPart);
}


/*
** RTC Backup Register(0-31) 값을 출력한다.
**
**
*/
void Dump_RtcBackupRegs(void)
{
    for (int i = RTC_BKP_DR0; i <= RTC_BKP_DR31; i++)
    {
        printf("RTC Backup Reg%02d : %08lX\n", i, bsp_ReadBackupReg(i));
    }
}


BOOL IsValidRtcDate(RTC_DateTypeDef* pDate)
{
    if (pDate->Year > 99)
    {
        return (FALSE);
    }

    if (pDate->Month == 0 && pDate->Month > 12)
    {
        return (FALSE);
    }

    if (pDate->Date == 0 || pDate->Date > 31)
    {
        return (FALSE);
    }

    return (TRUE);
}


BOOL IsValidTime(RTC_TimeTypeDef* pTime)
{
    if (pTime->Hours > 23)
        return (FALSE);

    if (pTime->Minutes > 59)
        return (FALSE);

    if (pTime->Seconds > 59)
        return (FALSE);

    return (TRUE);
}


BOOL ConvStrToDate(PCHAR szDate, int* pnYear, int* pnMonth, int* pnDay)
{
    int nYear, nMonth, nDay;
    char szValue[5];

    memcpy(szValue, &szDate[0], 4); szValue[4] = 0x00;
    nYear = atoi(szValue);
    if (nYear < 2000 || nYear > 2099)
        return (FALSE);

    szValue[0] = szDate[5]; szValue[1] = szDate[6]; szValue[2] = 0x00;
    nMonth = atoi(szValue);
    if (nMonth == 0 || nMonth > 12)
        return (FALSE);

    szValue[0] = szDate[8]; szValue[1] = szDate[9]; szValue[2] = 0x00;
    nDay = atoi(szValue);
    if (nDay == 0 || nDay > 31)
        return (FALSE);

    *pnYear = nYear;
    *pnMonth = nMonth;
    *pnDay = nDay;

    return (TRUE);
}


BOOL ConvStrToTime(PCHAR szTime, int* pnHour, int* pnMin, int* pnSec)
{
    int nHour, nMin, nSec;
    char szValue[5];

    szValue[0] = szTime[0]; szValue[1] = szTime[1]; szValue[2] = '\0';
    nHour = atoi(szValue);
    if (nHour < 0 || nHour > 23)
        return (FALSE);

    szValue[0] = szTime[3]; szValue[1] = szTime[4]; szValue[2] = '\0';
    nMin = atoi(szValue);
    if (nMin == 0 || nMin > 59)
        return (FALSE);

    szValue[0] = szTime[6]; szValue[1] = szTime[7]; szValue[2] = '\0';
    nSec = atoi(szValue);
    if (nSec < 0 || nSec > 59)
        return (FALSE);

    *pnHour = nHour;
    *pnMin = nMin;
    *pnSec = nSec;

    return (TRUE);
}


void Cmd_SetDateTime(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    int ret;
    char inputStr[80];
    int nYear, nMonth, nDay;
    int nHour, nMin, nSec;
    time_t setTime;

    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    printf("Press Enter to Skip...\n");
    printf("Current Date : 20%02d-%02d-%02d\n", sDate.Year, sDate.Month, sDate.Date);
    while (TRUE)
    {
        putstr("Input Date : ");
        ret = GetInputStr(inputStr, 80);
        if (ret == 0)
            break;
        if (ret != 10)
        {
            putstr("\n");
            continue;
        }
        if (ConvStrToDate(inputStr, &nYear, &nMonth, &nDay))
            break;
        putstr("Retry Input Again\n");
    }
    if (ret == 10)
    {
        sDate.Year = (uint8_t)(nYear - 2000);
        sDate.Month = (uint8_t)nMonth;
        sDate.Date = (uint8_t)nDay;
    }
    putstr("\n");

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    printf("Current Time : %02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
    memset(inputStr, 0x00, 80);
    while (TRUE)
    {
        putstr("Input Time : ");
        ret = GetInputStr(inputStr, 80);
        if (ret == 0)
            break;
        if (ret != 8)
        {
            putstr("\n");
            continue;
        }
        if (ConvStrToTime(inputStr, &nHour, &nMin, &nSec))
            break;
        putstr("Retry Input Again\n");
    }
    if (ret == 8)
    {
        sTime.Hours = (uint8_t)nHour;
        sTime.Minutes = (uint8_t)nMin;
        sTime.Seconds = (uint8_t)nSec;
    }
    putstr("\n");

    setTime = bsp_RTCDTimeToTime(&sDate, &sTime);

    bsp_SetSystemTimeInEpocTime(setTime);
}


/*
** ============================================================================
**  _USE_LONG_TIME_T : 32 bit yime_t 사용 flag를 정의시 glibc 의 gmtime_r, gmtime 함수가
**  제대로 동작하지 않아 호환 함수로 대체
**  이시스템에서는 UTC와 LocalTime을 구분하지않고 UTC를 Local Time처럼 사용한다.
** ============================================================================
*/

#define TIME_MAX        4294967295UL

#define difftime(time2, time1) ((double)((time2) - (time1)))

#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

//static char *_days[] =
//{
//	"Sunday", "Monday", "Tuesday", "Wednesday",
//	"Thursday", "Friday", "Saturday"
//};
//
//static char *_days_abbrev[] =
//{
//	"Sun", "Mon", "Tue", "Wed",
//	"Thu", "Fri", "Sat"
//};
//
//static char *_months[] =
//{
//	"January", "February", "March",
//	"April", "May", "June",
//	"July", "August", "September",
//	"October", "November", "December"
//};
//
//static char *_months_abbrev[] =
//{
//	"Jan", "Feb", "Mar",
//	"Apr", "May", "Jun",
//	"Jul", "Aug", "Sep",
//	"Oct", "Nov", "Dec"
//};

static int _ytab[2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static struct tm tmb;


struct tm *GmTime_r(const time_t *timer, struct tm *tmbuf)
{
	time_t 	time = *timer;
	unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long) time % SECS_DAY;
	dayno = (unsigned long) time / SECS_DAY;

	tmbuf->tm_sec = dayclock % 60;
	tmbuf->tm_min = (dayclock % 3600) / 60;
	tmbuf->tm_hour = dayclock / 3600;
	tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
	while (dayno >= (unsigned long) YEARSIZE(year))
	{
		dayno -= YEARSIZE(year);
		year++;
	}
	tmbuf->tm_year = year - YEAR0;
	tmbuf->tm_yday = dayno;
	tmbuf->tm_mon = 0;
	while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon])
	{
		dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday = dayno + 1;
	tmbuf->tm_isdst = 0;

	return tmbuf;
}

struct tm *GmTime(const time_t *timer)
{
	return GmTime_r(timer, &tmb);
}

