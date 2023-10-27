/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_util.c
 *  모듈 설명    : bsp 루틴 전체에서 사용되는 유틸리티 함수들
 *
 *  작성자       : 김정준
 *  작성일       : 2022/04/27
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#include "bsp.h"
#include "bsp_util.h"
#include <stdlib.h>
#include <ctype.h>
#include "const.h"
#include "global.h"

static char HexTbl[17] = "0123456789ABCDEF";


static void DelayMs(UINT32 ms)
{
    uint32_t tickstart = bsp_GetTickCount();

    if (tickstart != 0)
    {
        while ((bsp_GetTickCount() - tickstart) < ms)
        {
        }
    }
    else
    {
        for (tickstart = 0; tickstart < (20000 * ms); tickstart++)
        {
        }
    }
}

/*
**  ms 단위의 지연 함수
**  FreeRTOS 상에서는 Context Switch가 발생하여 Task간 교환이 일어남.
*/
void bsp_Delay(UINT32 nTicks)
{
#if USE_FreeRTOS == 1
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        osDelay(nTicks);
    }
    else
    {
        DelayMs(nTicks);
    }
#else
    DelayMs(nTicks);
#endif
}



/*
**  CPU 의 고유ID(UID)를 읽어 온다.
**  UID는 CPU 마다 고유하다.(장치마다)
**  MAC Address에 응용등....
*/
uint8_t* bsp_GetCpuUid(void)
{
    static uint32_t cpuId[3];

    cpuId[0] = *(__IO uint32_t*)(0x1FF1E800);
    cpuId[1] = *(__IO uint32_t*)(0x1FF1E800 + 4);
    cpuId[2] = *(__IO uint32_t*)(0x1FF1E800 + 8);

    return (uint8_t*)cpuId;
}


uint16_t bsp_GetFlashSize(void)
{
    uint16_t size;

    size = *(__IO uint16_t*)(0x1FF1E880);

    return size;
}


void test_Util(void)
{
    uint8_t CpuUID[CPU_UID_SIZE];
    uint8_t* pId = bsp_GetCpuUid();
    memcpy(CpuUID, pId, CPU_UID_SIZE);

    printf("CPUID : %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\r\n",
            CpuUID[0], CpuUID[1], CpuUID[2], CpuUID[3], CpuUID[4], CpuUID[5],
            CpuUID[6], CpuUID[7], CpuUID[8], CpuUID[9], CpuUID[10], CpuUID[11]);

    printf("FLASH SIZE : %dK\r\n", bsp_GetFlashSize());
}


/*
** ============================================================================
**  구 소스 호환 유틸리티 함수
** ============================================================================
*/

/* hexdecimal unsigned int to ascii */
void htoa(word h, char *s)
{
	int	cnt;

	for ( cnt = 12; cnt >= 0; cnt -= 4 )
		*s++ = HexTbl[(h >> cnt) & 0x0f];
	*s = '\0';
}

/* Converts all characters in s to lowercase */
char *strlwr(char *s)
{
	char	*ss;

	ss = s;
	while ( *s != '\0' ) {
		*s = tolower(*s);
		s++;
	}
	return	ss;
}

/* Converts all characters in s to uppercase */
char *strupr(char *s)
{
	char	*ss;

	ss = s;
	while ( *s != '\0' ) {
		*s = toupper(*s);
		s++;
	}
	return	ss;
}

int strcmpi(const char *a1, const char *a2)
{
	char    c1, c2;
	/* Want both assignments to happen but a 0 in both to quit, so it's | not || */
	while((c1=*a1) | (c2=*a2)) {
		if (!c1 || !c2 || /* Unneccesary? */
				(islower(c1) ? toupper(c1) : c1) != (islower(c2) ? toupper(c2) : c2))
			return (c1 - c2);
		a1++;
		a2++;
	}
	return 0;
}

dword atoh(char *str)
{
	dword i = 0, dig;
	char c;

	while((c = *str++) != '\0') {
		if(c >= '0' && c <= '9')
			dig = (dword)(c - '0');
		else if(c >= 'a' && c <= 'f')
			dig = (dword)(c - 'a') + 10;
		else if(c >= 'A' && c <= 'F')
			dig = (dword)(c - 'A') + 10;
		else
			break;

		i = (i << 4) + dig;
	}
	return i;
}

long aton(char *str)
{
	if(memcmp(str, "0x", 2) == EQ || memcmp(str, "0X", 2) == EQ)
		return(atoh(&str[2]));
	return(atoi(str));
}

static	void dump(long adr, byte *m, char n)
{
#define isdump(c)	(isprint(c) ? (c) : ('.'))
	byte	cnt, i, j;

	printf("%06lX:", adr & 0xfffff0L);
	cnt = (byte)adr & 0x0f;
	for ( i = 0, j = 0; i < 16; i++ ) {
		if ( i == 8 )
			putchar('-');
		else
			putchar(' ');
		if ( i < cnt || j >= n )
			putstr("  ");
		else {
			HexPrint(m[j], 2);
			j++;
		}
	}
	putstr("  ");
	for ( i = 0; i < cnt; i++ )
		putchar(' ');
	for ( i = 0; i < n; i++ )
		putchar(isdump(m[i]));
	while ( (cnt + i++) < 16 )
		putchar(' ');

	putchar('\n');
}

void hexdump(long adr, byte *pbuf, int siz)
{
	int 	i, n, l, d;

	if(siz == 0)
		return;

	for(i = 0 ; i < siz ;  ) {
		n = adr % 16;
		if(n == 0) {
			d = siz - i;
			n = min(16, d);
			dump(adr, &pbuf[i], n);
		}
		else {
			l = 16 - n;
			d = siz - i;
			n = min(l, d);
			dump(adr, &pbuf[i], n);
		}
		i += n;
		adr += n;
	}
}

#define MSGDUMP_LEN 	30
void MsgDump(byte enb, char *msg, byte *data, int size)
{
	int 	i, slen;
	char    sbuf[MSGDUMP_LEN + 1];

	if(!enb)
		return;

	slen = strlen(msg);
	if(slen > MSGDUMP_LEN)
		slen = MSGDUMP_LEN;
	memset(sbuf, 0x20, slen);	//ascii-code space setting
	sbuf[slen] = '\0';
	printf("%s", msg);
	for(i = 0 ; i < size ; i++) {
		if(i && !(i%15))
			printf("\n%s", sbuf);
		printf("%02x ", data[i]);
	}
	putchar('\n');
}

int allis(byte *p, byte c, int n)
{
	while (n-- > 0)
		if (*p++ != c)
			return(0);
	return(1);
}

void swapn(void *d, void *s, int n)
{
	int i;

	i = 0;
	while(n--)
		*((byte *)d + i++) = *((byte *)s + n);
}


