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
#ifndef _BSP_UTIL_H_
#define _BSP_UTIL_H_

#define CPU_UID_SIZE 12

void bsp_Delay(UINT32 nTicks);
uint8_t* bsp_GetCpuUid(void);
uint16_t bsp_GetFlashSize(void);


/*
** ============================================================================
**  구 소스 호환 유틸리티 함수
** ============================================================================
*/

void    htoa(WORD h, char *s);
char    *strlwr(char *s);
char    *strupr(char *s);
int     strcmpi(const char *a1, const char *a2);
dword   atoh(char *str);
long    aton(char *str);
void    hexdump(long adr, BYTE *pbuf, int siz);
void    MsgDump(BYTE enb, char *msg, BYTE *data, int size);
int     allis(BYTE *p, BYTE c, int n);
#define allzero(p,n)    allis(p, 0, n)
#define allone(p,n) allis(p, 0xff, n)
void    swapn(void *d, void *s, int n);

#endif  // _BSP_UTIL_H_
