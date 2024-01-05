/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : time_comp.c
 *  모듈 설명    : 구 소스 time 관련 호환성 처리 루틴
 *
 *  작성자       : 김정준
 *  작성일       : 2022/11/21 -
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#include "time_comp.h"
#include "struct.h"
#include "global.h"

#define ASCBUFSIZE        		(26 + 2)

static char dtBuf[ASCBUFSIZE];


time_t time(time_t *_timer)
{
	if(_timer != NULL)
		*_timer = bsp_GetTime();

	return bsp_GetTime();
}


// return sec.
dword clocktime(void)
{
	return (bsp_GetTime());
}


// return ms.
dword mclocktime(void)
{
	return (bsp_GetTimeEx());
}


char *dformat(const struct tm *tp)
{
	char *pdtBuf = dtBuf;

	if (tp == NULL)
		strcpy(pdtBuf, "0000-00-00 00:00:00");
	else
		sprintf(pdtBuf, "%04d-%02d-%02d %02d:%02d:%02d",
				(tp->tm_year + 1900), (tp->tm_mon + 1), tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	return pdtBuf;
}


char *kformat(const struct tm *tp)
{
	char *pdtBuf = dtBuf;

	if (tp == NULL)
		strcpy(pdtBuf, "000000000000"); // YYMMDDHHNNSS
	else
		sprintf(pdtBuf, "%02d%02d%02d%02d%02d%02d",
				(tp->tm_year >= 100) ? (tp->tm_year - 100) : tp->tm_year, (tp->tm_mon + 1),
				tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

	return pdtBuf;
}


char *dtime(const time_t *tp)
{
	return dformat(*tp == 0 ? NULL : GmTime(tp));
}


char *ktime(const time_t *tp)
{
	return kformat(*tp == 0 ? NULL : GmTime(tp));
}


char *strmtime()
{
	time_t	now;
	char	*s;

	time(&now);
	s = dtime(&now);
	sprintf(&s[strlen(s)], ".%03u", (unsigned)(mclocktime()%1000));
	return s;
}


clock_t clock(void)
{
	return bsp_GetTime();
}

// n sec delay
void sleep(int t)
{
	time_t sec;

	timeout(&sec, 0);
	while (!timeout(&sec, t))
		;
}

// ticktime(1ms unit) delay
void msleep(int t)
{
	clock_t ms;

	mstimeout(&ms,0);
	while (!mstimeout(&ms, t))
		;
}


clock_t ticktime(void)
{
	return (bsp_GetTime());
}


dword difftick(clock_t st, clock_t et)
{
	DWORD diff, m = 0xffffffff;

	if(et >= st)
		diff = et - st;
	else {
		diff = (m - st) + 1;
		diff += et;
	}
	return diff;
}


clock_t mstime(clock_t *tp)
{
	time_t cur = bsp_GetTickCount();

	if(tp != NULL)
		*tp = cur;

	return cur;
}


int timeout(time_t *tp, int sec)
{
	time_t tim;
	DWORD  diff;
	int    tout=0;

	tim = time(0);
	diff = tim - *tp;
	if (sec == 0 || diff >= sec)
	{
		*tp = tim;
		tout = 1;
	}
	return(tout);
}


int mstimeout(clock_t *cp, int msec)
{
	clock_t tim;
	DWORD   diff;

	tim = mstime(0);
	if(msec == 0)
	{
		*cp = tim;
		return(1);
	}

	if(tim >= *cp)
		diff = tim - *cp;
	else
	{
		diff = (0xffffffffUL - *cp) + 1;
		diff += tim;
	}

	if(diff >= msec)
	{
		*cp  = tim;
		return(1);
	}

	return (0);
}


int setdate(struct tm *t, char *str)
{
    char    *av[6];
    char    dstr[20];
    int n, i;

    // string conversion "yyyymmddhhmmss" => "yyyy\0mm\0dd\0hh\0mm\0ss\0"
    memset(dstr, 0x00, sizeof(dstr));
    av[0] = dstr;
    av[1] = &dstr[5];
    av[2] = &dstr[8];
    av[3] = &dstr[11];
    av[4] = &dstr[14];
    av[5] = &dstr[17];

    n = strlen(str);
    if(n < 4 || n > 14 || n % 2)
        return(-1);

    if(n == 4){
        memcpy(av[0], str, 4);
        n = 1;
    }
    else {
        n /= 2;
        n --;
        memcpy(av[0], str, 4);
        for(i = 1 ; i < n ; i++)
            memcpy(av[i], &str[i*2+2], 2);
    }

    if(n > 0) {
        i = atoi(av[0]);
        if(i >= 2000 && i <= 2099)
            t->tm_year = i - 1900;
        else
            return(-1);
    }

    if(n > 1) {
        i = atoi(av[1]);
        if(i >= 1 && i <= 12)
            t->tm_mon = i - 1;
        else
            return(-1);
    }

    if(n > 2) {
        i = atoi(av[2]);
        if (i >= 1 && i <= 31)
            t->tm_mday = i;
        else
            return(-1);
    }

    if(n > 3) {
        i = atoi(av[3]);
        if(i >= 0 && i <= 23 )
            t->tm_hour = i;
        else
            return(-1);
    }

    if(n > 4) {
        i = atoi(av[4]);
        if(i >= 0 && i <= 59 )
            t->tm_min = i;
        else
            return(-1);
    }

    if(n > 5) {
        i = atoi(av[5]);
        if(i >= 0 && i <= 59)
            t->tm_sec = i;
        else
            return(-1);
    }

    return 0;
}


byte *asc2bcd(byte *des, byte *asc, int n)
{
    byte *bcd = des;
    int i, c, x;

    n /= 2;
    for (i = 0; i < n; i++) {
        x = toupper(asc[0]);
        c  = x-((!isxdigit(x)||isdigit(x))?'0':('A'-10)) << 4;
        x = toupper(asc[1]);
        c |= x-((!isxdigit(x)||isdigit(x))?'0':('A'-10)) & 0xf;
        *bcd++ = c;
        asc += 2;
    }
    return(des);
}

#define frombcd(v)  ((v >> 4) * 10 + (v & 0x0f))

// YYYYMMDDHHmmss BCD 臾몄옄�뿴 �룷留� -> time_t �떆媛꾩쑝濡� 蹂�寃�
time_t bcd2time(byte *bcd)
{
    struct tm st;
    time_t    tt;
    int   dt[6], i;

    for(i = 0 ; i < 6 ; i++)
        dt[i] = frombcd(bcd[i+1]);

    if(dt[0] >= 70)
        st.tm_year = dt[0];
    else
        st.tm_year = 100 + dt[0]; /* 1900�뀈�쓣 湲곗��  */

    st.tm_mon = dt[1] - 1;
    st.tm_mday = dt[2];
    st.tm_hour = dt[3];
    st.tm_min = dt[4];
    st.tm_sec = dt[5];
    st.tm_isdst = 0;

    if ((tt = mktime(&st)) == (time_t)(-1)) {
        printf("time string format error\n");
        return (-1);
    }
    return (tt);
}


// "YYYYMMDDHHmmss" -> time_t
time_t asc2time(byte *asc)
{
    byte bcd[7];

    //byte *asc2bcd(byte *des, byte *asc, int n);
    //time_t bcd2time(byte *bcd);

    asc2bcd(bcd, asc, 14);
    return(bcd2time(bcd));
}


time_t rtc2time(byte *rtc)
{
    struct tm   tm;

    tm.tm_year = BCD2DEC(rtc[0])*100 + BCD2DEC(rtc[1]) - YEAR0;
    tm.tm_mon = BCD2DEC(rtc[2])-1;
    tm.tm_mday = BCD2DEC(rtc[3]);
    tm.tm_hour = BCD2DEC(rtc[4]);
    tm.tm_min = BCD2DEC(rtc[5]);
    tm.tm_sec = BCD2DEC(rtc[6]);
    return(mktime(&tm));
}

byte *time2rtc(const time_t t)
{
    struct  tm *tp = GmTime(&t);
    int     y0, y1;
    static  byte    rtc[7];

    y0 = (tp->tm_year+YEAR0) / 100;
    y1 = (tp->tm_year+YEAR0) % 100;
    rtc[0] = DEC2BCD(y0);
    rtc[1] = DEC2BCD(y1);
    rtc[2] = DEC2BCD(tp->tm_mon+1);
    rtc[3] = DEC2BCD(tp->tm_mday);
    rtc[4] = DEC2BCD(tp->tm_hour);
    rtc[5] = DEC2BCD(tp->tm_min);
    rtc[6] = DEC2BCD(tp->tm_sec);
    return(rtc);
}



void TimeOfDay(void)
{
    // Every 0.1 sec period
    config.RunTime++;
    if(!((++config.hund)%2))
        RetriggerWdt();

    if(config.hund < 10 )
        return;

    // Every 1 sec period
    config.hund = 0;
    config.sec++;
}


//
// Interval Timer Implementation
//
#define NCALL   16  // maximum simultaneous callouts

typedef struct {
    dword   c_initm;        // initial time
    dword   c_time;         // target time
    int     c_tmid;         // timer id
    int     c_arg;          // argument to routine
    void    (*c_func)();    // routine
} clo_t;

static clo_t    callOut[NCALL+1];
static clo_t    curCall;



void TimerService(void)
{
    static dword    srvClkTime;
    register dword  clktime;
    register clo_t  *cp1, *cp2;

    clktime = mclocktime();
    if (clktime != srvClkTime) {
        cp1 = cp2 = &callOut[0];
        while (cp1->c_func && (int)(cp1->c_time - clktime) <= 0) {
            curCall = *cp1;
            do *cp2 = *++cp1; while (cp2++->c_func);
            (*curCall.c_func)(curCall.c_arg);
            cp1 = cp2 = &callOut[0];
        }
        srvClkTime = clktime;
    }
}


// set timer function
int timer(void (*func)(), int arg, int sec)
{
    register dword  ctime;
    register int    tmid;
    register clo_t  *cp1, *cp2;
    static int  timerid;

    ctime = MS(sec);
    cp1 = &callOut[0];
    if (callOut[NCALL-1].c_func) {
        printf("timer table overflow (%d)", NCALL);
        return(0);
    }
    while (cp1->c_func && (int)(cp1->c_time-(mclocktime()+ctime)) <= 0)
        cp1++;
    cp2 = cp1;
    while (cp2->c_func)
        cp2++;
    while (cp2 >= cp1) {
        (cp2+1)->c_initm = cp2->c_initm;
        (cp2+1)->c_time = cp2->c_time;
        (cp2+1)->c_func = cp2->c_func;
        (cp2+1)->c_arg = cp2->c_arg;
        (cp2+1)->c_tmid = cp2->c_tmid;
        cp2--;
    }
    cp1->c_initm = ctime;
    cp1->c_time = mclocktime() + ctime;
    cp1->c_func = func;
    cp1->c_arg = arg;
    tmid = ++timerid;
    cp1->c_tmid = tmid;
    return(tmid);
}

// remove timer-set
int untimer(int tmid)
{
    register clo_t  *cp1, *cp2;
    register int    found = 0;

    for (cp1 = callOut; cp1->c_func; cp1++)
        if (cp1->c_tmid == tmid) {
            found++;
            cp2 = cp1 + 1;
            do *cp1++ = *cp2++; while (cp1->c_func);
            break;
        }
    return(found);
}

// get current timer id
int timer_id(void)
{
    return(curCall.c_tmid);
}

// timer is in set?
int timer_in(int tmid)
{
    register clo_t  *p;

    for (p = callOut; p->c_func; p++)
        if (p->c_tmid == tmid)
            return(TRUE);
    return(FALSE);
}

// call timer immediately
void timer_out(int tmid)
{
    register clo_t  *p;

    for (p = callOut; p->c_func; p++)
        if (p->c_tmid == tmid) {
            p->c_time = mclocktime();
            break;
        }
}



void Test_TimeCompatible(void)
{
	  time_t curT = time(NULL);
	  struct tm curTm;
	  bsp_GetSystemTimeInTm(&curTm);

	  printf("struct tm : %04d-%02d-%02d %02d:%02d:%02d\n", curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday,
			  curTm.tm_hour, curTm.tm_min, curTm.tm_sec);
	  printf("dformat(cur) : %s\r\n", dformat(&curTm));
	  printf("kformat(cur) : %s\r\n", kformat(&curTm));
	  printf("dtime(cur) : %s\r\n", dtime(&curT));
	  printf("ktime(cur) : %s\r\n", ktime(&curT));

	  printf("strmtime() : %s\r\n", strmtime());

	  printf("sizeof(clock_t) : %d, clock() : %ld\r\n", sizeof(clock_t), clock());


	  printf("\r\n");


}


#define tobcd(v)    (((v / 10) << 4) | (v % 10))
#define frombcd(v)  ((v >> 4) * 10 + (v & 0x0f))

const char pattern[16] = {
    0x11, 0x22, 0x44, 0x88, 0xee, 0xdd, 0xbb, 0x77, 0xaa, 0x55,
    'B', 'A', 'C', 'K', 'U', 'P'
};


time_t SystemInitTime(void)
{
    struct  tm ts;

    ts.tm_year = frombcd(GCUDate[1]);
    ts.tm_year += 100;
    ts.tm_mon = frombcd(GCUDate[2]);
    ts.tm_mon--;
    ts.tm_mday = frombcd(GCUDate[3]);
    ts.tm_hour = frombcd(GCUDate[4]);
    ts.tm_min = frombcd(GCUDate[5]);
    ts.tm_sec = 0;

    //time_t initTime = mktime(&ts);
    SysInitTime = mktime(&ts);
    return (SysInitTime);
}


// bsp_RTCInit() 로 대체
void SetRtc(int fTimeReset)
{
    if (fTimeReset)
        config.sec = SystemInitTime();

    bsp_SetSystemTimeInEpocTime(config.sec);
}


// RTC 레지스터 값 및 백업 패턴을 리턴하는 함수
time_t GetRtc(void)
{
    time_t rtc_time;

    if (bsp_ReadBackupReg(RTC_BKP_DR0) != RTCSET_MAGIC)   // RTC가 설정된적이 없음.
    {
        rtc_time = SystemInitTime();
        bsp_SetSystemTimeInEpocTime(rtc_time);
        config.rBackup = 0;
        return (rtc_time);
    }

    rtc_time = bsp_GetSystemTimeInEpoc();

    config.rBackup = 1;
    return (rtc_time);
}

