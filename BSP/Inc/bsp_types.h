/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_types.h
 *  모듈 설명    : 각종 타입 정의, 매크로및 전처리 상수
 *
 *  작성자       : 김정준
 *  작성일       : 2022/03/08
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef __BSP_TYPES_H__
#define __BSP_TYPES_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <time.h>
//#include "time.h"
#include "main.h"
#include "integer.h"    // FatFs
#include "bsp_config.h"
#include "bsp_macro.h"



/*
**  FAT FS 사용시 INT, UINT, ... 충돌 방지위해 조건부 정의
**
*/
//#if defined(_FATFS)

#ifndef VOID
#define VOID            void
typedef char            CHAR, CCHAR, INT8, SINT8, *PCHAR, *PSTR, *LPSTR;
typedef const char      *PCSTR, *LPCSTR;
//typedef unsigned char   UCHAR, BYTE, UINT8, *PUCHAR, *PBYTE, *LPBYTE, BOOLEAN, *PBOOLEAN;
typedef unsigned char   UCHAR, UINT8, *PUCHAR, *PBYTE, *LPBYTE, BOOLEAN, *PBOOLEAN;
typedef short           SHORT, *PSHORT, INT16, SINT16;
//typedef unsigned short  USHORT, *PUSHORT, WORD, *PWORD, *LPWORD, WCHAR, *PWCHAR, *PWSTR, *LPWSTR, UINT16;
typedef unsigned short  USHORT, *PUSHORT, *PWORD, *LPWORD, *PWCHAR, *PWSTR, *LPWSTR, UINT16;
//typedef int             INT;
//typedef unsigned int    UINT, DWORD, *PDWORD;
typedef unsigned int    *PDWORD;
//typedef long            LONG;
typedef unsigned long   ULONG;

typedef char            SINT8;
typedef unsigned char   UINT8;
typedef short           INT16;
typedef short           SINT16;
typedef int             INT32;
typedef int             SINT32;
typedef unsigned int    UINT32;
typedef long            LONG;
typedef unsigned long   ULONG;

typedef int64_t         LONGLONG, *PLONGLONG, INT64, *PINT64;
typedef uint64_t        ULONGLONG, PULONGLONG, UINT64, *PUINT64;

typedef int             BOOL;

#endif  // VOID


typedef unsigned char       byte;       /* 8-bit data */
typedef unsigned short int  word;       /* 16-bit data */
typedef unsigned int        dword;      /* 32-bit data */
typedef unsigned long       lword;      /* 32-bit data */

typedef unsigned char       U08;        /* 8-bit data */
typedef unsigned short int  U16;        /* 16-bit data */
typedef unsigned int        U32;        /* 32-bit data */

typedef unsigned char       uchar;      /* 8-bit data */
typedef unsigned int        uint;       /* 32-bit data */
typedef unsigned long       ulong;      /* 32-bit data */

typedef char                S08;        /* 8-bit data */
typedef short int           S16;        /* 16-bit data */
typedef int                 S32;        /* 32-bit data */

#define true	1
#define false	0

//#endif  // defined(_FATFS)

typedef union _LARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        LONG  HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        LONG  HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;


typedef union _ULARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    LONGLONG QuadPart;
} ULARGE_INTEGER;


typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;


#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif


/* Serial Commincation */
#define tochar(ch)  ((ch)+0x20)
#define unchar(ch)  ((ch)-0x20)

/* ASCII */
#ifndef NULL
#define NULL    0x00    /* NULL (^@)                */
#endif

#define SOH     0x01    /*  (^A)                    */
//#define STX     0x02    /*  (^B)                    */
#define ETX     0x03    /*  (^C)                    */
#define BREAK   0x03    /* break (^C)               */
#define EOT     0x04    /*  (^D)                    */
#define ENQ     0x05    /*  (^E)                    */
#define ACK     0x06    /*  (^F)                    */
#define BELL    0x07    /* bell (^G)                */
#define BS      0x08    /* back space (^H)          */
#define HT      0x09    /* horizontally tab (^I)    */
#define LF      0x0a    /* line feed (^J)           */
#define VT      0x0b    /* vertically tab (^K)      */
#define FF      0x0c    /* form feed (^L)           */
//#define CR        0x0d    /* carriage return (^M)     */
#define SO      0x0e    /* shift out (^N)           */
#define SI      0x0f    /* shift in (^O)            */
#define DLE     0x10    /*  (^P)                    */
#define DC1     0x11    /* device control 1 (^Q)    */
#define DC2     0x12    /* device control 2 (^R)    */
#define DC3     0x13    /* device control 3 (^S)    */
#define DC4     0x14    /* device control 4 (^T)    */
#define NAK     0x15    /*  (^U)                    */
#define SYN     0x16    /*  (^V)                    */
#define ETB     0x17    /*  (^W)                    */
#define CA      0x18    /* cancel line (^X)         */
#define EM      0x19    /* end of medium (^Y)       */
#define SUB     0x1a    /*  (^Z)                    */
#define ESC     0x1b    /* escape (^[)              */
#define FS      0x1c    /*  (^\)                    */
#define GS      0x1d    /*  (^])                    */
#define RS      0x1e    /*  (^^)                    */
#define US      0x1f    /*  (^_)                    */
#define SP      0x20    /* space                    */

#define DLE_OP		0x40
#define DLE_STX		0x42
#define DLE_ETX		0x43
#define DLE_DLE		0x50

#define XON     DC1
#define XOFF    DC3

#define NONE    0
#define EXIST   1
#define OFF     0
#define ON      1
//#define OPEN    2
#define SHORT1  3
#define OK      0
#define FAIL    (-1)
//#define ERROR (-1)
#define EQ      0


typedef unsigned long   u32;
typedef unsigned short  u16;
typedef unsigned char   u8;

typedef signed long   s32;
typedef signed short  s16;
typedef signed char   s8;

typedef volatile unsigned long   vu32;
typedef volatile unsigned short  vu16;
typedef volatile unsigned char   vu8;

typedef volatile signed long   vs32;
typedef volatile signed short  vs16;
typedef volatile signed char   vs8;

#endif  // _BSP_TYPES_H_
