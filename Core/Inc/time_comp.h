/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : time_comp.h
 *  모듈 설명    : 구 소스 time 관련 호환성 처리 루틴 헤더
 *
 *  작성자       : 김정준
 *  작성일       : 2022/11/21 -
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _TIME_COMP_H_
#define _TIME_COMP_H_

#include "bsp_types.h"
#include "bsp_timer.h"
#include "bsp_rtc.h"


#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define ASCBUFSIZE              (26 + 2)
#define MS(t)                   ((t) < 0 ? ~(t) : ((t)*1000))

dword mclocktime(void);
dword clocktime(void);

char *dformat(const struct tm *tp);
char *kformat(const struct tm *tp);
char *dtime(const time_t *tp);
char *ktime(const time_t *tp);
char *strmtime();

void sleep(int t);
void msleep(int t);
clock_t ticktime(void);
dword   difftick(clock_t st, clock_t et);
clock_t mstime(clock_t *tp);
int timeout(time_t *tp, int sec);
int mstimeout(clock_t *cp, int msec);
int setdate(struct tm *t, char *str);
byte *asc2bcd(byte *des, byte *asc, int n);
time_t bcd2time(byte *bcd);
time_t asc2time(byte *asc);

time_t rtc2time(byte *rtc);
byte *time2rtc(const time_t t);


void TimerService(void);
int timer(void (*func)(), int arg, int sec);
int untimer(int tmid);
int timer_id(void);
int timer_in(int tmid);
void timer_out(int tmid);

time_t SystemInitTime(void);
void SetRtc(int fTimeReset);
time_t GetRtc(void);


void Test_TimeCompatible(void);

#endif 	// _TIME_COMP_H_
