/*
 * crc16.c
 *
 *  Created on: 2022. 4. 19.
 *      Author: ITFUN - SUN07
 */

// 	@(#)crc16.c	AIO100-0.2a	2017/03/25	(C)2017.SOLIDTEK

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

typedef unsigned char	byte;
typedef unsigned short	word;

#define	POLYNOMIAL	0x8005

word *
mk_crc16_table(word polynomial)
{
	register word	*crc16_table, c;
        register int	i, j;

	if (polynomial == 0)
		polynomial = POLYNOMIAL;
	crc16_table = (word *)malloc(256 * sizeof(word));
        for (i = 0; i < 256; i++) {
                c = i << 8;
                for (j = 0; j < 8; j++) {
                        if (c & 0x8000)
                                c = (c << 1) ^ polynomial;
                        else
                                c = (c << 1);
                }
                crc16_table[i] = c;
        }
	return(crc16_table);
}

static word	*crc16_table;

word
crc16c(word *tbl, byte *ptr, unsigned len, word crc)
{
	register word	idx;
	byte		b;

	if (tbl == (word *)0) {
		if (crc16_table == (word *)0)
			crc16_table = mk_crc16_table(0);
		tbl = crc16_table;
	}
	if (len == 0)
		len = strlen((char *)ptr);
	else if (len == 1) {
		b = (int)ptr;
		ptr = &b;
	}
        while (len-- > 0) {
                idx = (crc >> 8) ^ *ptr++;
                crc = (crc << 8) ^ tbl[idx];
        }
        return(crc);
}

// EOF
