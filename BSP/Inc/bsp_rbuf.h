/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2019 - 2022 ITFun co., All right reserved.
 *
 *  시스템명     : 2022 STraffic GCU Module Firmware
 *  모듈명       : bsp_rbuf.h
 *  모듈 설명    : RING Buffer 헤더
 *
 *  작성자       : 김정준
 *  작성일       : 2022/08/15
 *  버  전       : 1.0
 * ---------------------------------------------------------------------------
 */

#ifndef _BSP_RB_H_
#define _BSP_RB_H_

#include "bsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct _rbuf
{
	uint8_t*	buff;
	size_t		size;	// buffer 크기. 실제 버퍼에 저장 할수 있는 크기는 1이 작음.
	size_t		r;		// Next read pointer. Buffer is considered empty when `r == w` and full when `w == r - 1`
	size_t		w;		// Next write pointer. Buffer is considered empty when `r == w` and full when `w == r - 1`
} RBUF_T;


uint8_t RBuf_Init(RBUF_T* pRBuf, void* buffer, size_t size);
uint8_t RBuf_Is_Ready(RBUF_T* pRBuf);
void    RBuf_Free(RBUF_T* pRBuf);
void    RBuf_Reset(RBUF_T* pRBuf);


/* Read/Write functions */
size_t  RBuf_Write(RBUF_T* pRBuf, const void* data, size_t btw);
size_t  RBuf_Read(RBUF_T* pRBuf, void* data, size_t btr);
size_t  RBuf_Peek(RBUF_T* pRBuf, size_t skip_count, void* data, size_t btp);

/* Buffer size information */
size_t  RBuf_Get_Free(RBUF_T* buff);	// for write to bufeer
size_t  RBuf_Get_Full(RBUF_T* buff);	// for read from bufeer


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _BSP_RB_H_

