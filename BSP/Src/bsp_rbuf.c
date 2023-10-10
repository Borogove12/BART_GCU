#include "bsp_rbuf.h"

// Macro
#define BUF_MEMSET                      memset
#define BUF_MEMCPY                      memcpy
#define BUF_IS_VALID(rb)                ((rb) != NULL && (rb)->buff != NULL && (rb)->size > 0)
#define BUF_MIN(x, y)                   ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y)                   ((x) > (y) ? (x) : (y))


uint8_t RBuf_Init(RBUF_T* pRBuf, void* buffer, size_t size)
{
    if (buffer == NULL || buffer == NULL || size == 0)
    {
        return 0;
    }

    BUF_MEMSET((void*)buffer, 0x00, sizeof(*buffer));

    pRBuf->size = size;
    pRBuf->buff = buffer;

    return 1;
}


uint8_t RBuf_Is_Ready(RBUF_T* pRBuf)
{
	return BUF_IS_VALID(pRBuf);
}


void RBuf_Free(RBUF_T* pRBuf)
{
    if (BUF_IS_VALID(pRBuf))
    {
    	pRBuf->buff = NULL;
    }
}


void RBuf_Reset(RBUF_T* pRBuf)
{
    if (BUF_IS_VALID(pRBuf))
    {
    	pRBuf->w = 0;
    	pRBuf->r = 0;
    }
}


/**
 * 	data를 링버퍼에 기록 한다.
 *
 *	data : 기록할 버퍼
 *  btw : 기록할 데이터 크기(Bytes of write)
 *
 *  return : 기록한 데이터 크기, btw 보다 작으면 링버퍼에 공간이 없음을 의미
 */
size_t RBuf_Write(RBUF_T* pRBuf, const void* data, size_t btw)
{
    size_t tocopy, free;
    volatile size_t buff_w_ptr;
    const uint8_t* d = data;

    if (!BUF_IS_VALID(pRBuf) || data == NULL || btw == 0)
    {
        return 0;
    }

    /* Calculate maximum number of bytes available to write */
    free = RBuf_Get_Free(pRBuf);
    btw = BUF_MIN(free, btw);
    if (btw == 0)
    {
        return 0;
    }

    /* Step 1: Write data to linear part of buffer */
    buff_w_ptr = pRBuf->w;
    tocopy = BUF_MIN(pRBuf->size - buff_w_ptr, btw);
    BUF_MEMCPY(&pRBuf->buff[buff_w_ptr], d, tocopy);
    buff_w_ptr += tocopy;
    btw -= tocopy;

    /* Step 2: Write data to beginning of buffer (overflow part) */
    if (btw > 0)
    {
        BUF_MEMCPY(pRBuf->buff, &d[tocopy], btw);
        buff_w_ptr = btw;
    }

    /* Step 3: Check end of buffer */
    if (buff_w_ptr >= pRBuf->size)
    {
        buff_w_ptr = 0;
    }

    /*
     * Write final value to the actual running variable.
     * This is to ensure no read operation can access intermediate data
     */
    pRBuf->w = buff_w_ptr;

    return tocopy + btw;
}


/**
 * 	링버퍼에 있는 데이터를 btr만큼 data에 읽어 온다
 *
 *	data : 읽은 데이터를 보관 할 버퍼
 *  btr	 : 읽을 데이터 크기(Bytes of read)
 *
 *  return : 실제 읽어온 데이터 크기
 */
size_t RBuf_Read(RBUF_T* pRBuf, void* data, size_t btr)
{
    size_t tocopy, full;
    volatile size_t buff_r_ptr;
    uint8_t* d = data;

    if (!BUF_IS_VALID(pRBuf) || data == NULL || btr == 0)
    {
        return 0;
    }

    /* Calculate maximum number of bytes available to read */
    full = RBuf_Get_Full(pRBuf);
    btr = BUF_MIN(full, btr);
    if (btr == 0)
    {
        return 0;
    }

    /* Step 1: Read data from linear part of buffer */
    buff_r_ptr = pRBuf->r;
    tocopy = BUF_MIN(pRBuf->size - buff_r_ptr, btr);
    BUF_MEMCPY(d, &pRBuf->buff[buff_r_ptr], tocopy);
    buff_r_ptr += tocopy;
    btr -= tocopy;

    /* Step 2: Read data from beginning of buffer (overflow part) */
    if (btr > 0)
    {
        BUF_MEMCPY(&d[tocopy], pRBuf->buff, btr);
        buff_r_ptr = btr;
    }

    /* Step 3: Check end of buffer */
    if (buff_r_ptr >= pRBuf->size)
    {
        buff_r_ptr = 0;
    }

    /*
     * Write final value to the actual running variable.
     * This is to ensure no write operation can access intermediate data
     */
    pRBuf->r = buff_r_ptr;

    return tocopy + btr;
}


/**
 * 	링버퍼에 있는 데이터를 btr만큼 data에 읽어 온다. 이때 내부 포인터는 조정하지 않는다.
 *
 * 	skip_count : 일기 전에 skip 할 바이트 수
 * 	data : 읽은 데이터를 보관 할 버퍼
 *  btr	 : 읽을 데이터 크기(Bytes of read)
 */
size_t RBuf_Peek(RBUF_T* pRBuf, size_t skip_count, void* data, size_t btp)
{
    size_t full, tocopy;
    volatile size_t r;
    uint8_t* d = data;

    if (!BUF_IS_VALID(pRBuf) || data == NULL || btp == 0)
    {
        return 0;
    }

    r = pRBuf->r;

    /* Calculate maximum number of bytes available to read */
    full = RBuf_Get_Full(pRBuf);

    /* Skip beginning of buffer */
    if (skip_count >= full)
    {
        return 0;
    }
    r += skip_count;
    full -= skip_count;
    if (r >= pRBuf->size)
    {
        r -= pRBuf->size;
    }

    /* Check maximum number of bytes available to read after skip */
    btp = BUF_MIN(full, btp);
    if (btp == 0)
    {
        return 0;
    }

    /* Step 1: Read data from linear part of buffer */
    tocopy = BUF_MIN(pRBuf->size - r, btp);
    BUF_MEMCPY(d, &pRBuf->buff[r], tocopy);
    btp -= tocopy;

    /* Step 2: Read data from beginning of buffer (overflow part) */
    if (btp > 0)
    {
        BUF_MEMCPY(&d[tocopy], pRBuf->buff, btp);
    }

    return tocopy + btp;
}



/**
 * 	링버퍼의 기록 가용 크기를 리턴한다.
 */
size_t RBuf_Get_Free(RBUF_T* pRBuf)
{
    size_t size;
    volatile size_t w, r;

    if (!BUF_IS_VALID(pRBuf))
    {
        return 0;
    }

    /* Use temporary values in case they are changed during operations */
    w = pRBuf->w;
    r = pRBuf->r;
    if (w == r)		// empty
    {
        size = pRBuf->size;
    }
    else if (r > w)
    {
        size = r - w;
    }
    else
    {
        size = pRBuf->size - (w - r);
    }

    /* Buffer free size is always 1 less than actual size */
    return size - 1;
}


/**
 * 	링버퍼에 저장되어 있는 데이터 크기를 리턴(읽기용)
 */
size_t RBuf_Get_Full(RBUF_T* pRBuf)
{
    size_t size;
    volatile size_t w, r;

    if (!BUF_IS_VALID(pRBuf))
    {
        return 0;
    }

    /* Use temporary values in case they are changed during operations */
    w = pRBuf->w;
    r = pRBuf->r;
    if (w == r)		// buffer is empty
    {
        size = 0;
    }
    else if (w > r)
    {
        size = w - r;
    }
    else
    {
        size = pRBuf->size - (r - w);
    }
    return size;
}


