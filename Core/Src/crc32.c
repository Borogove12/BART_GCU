// 	@(#)crc32.c		SSG100-0.01		2023/01/10		IOE

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

typedef unsigned char	byte;
typedef unsigned long	dword;

#define	POLYNOMIAL	0x04c11db7L

static dword *
mk_crc32_table(dword polynomial)
{
        register dword	*crc32_table, c;
	register int	i, j;

	if (polynomial == 0)
		polynomial = POLYNOMIAL;
	crc32_table = (dword *)malloc(256 * sizeof(dword));
        for (i = 0; i < 256; i++) {
                c = (dword)i << 24;
                for (j = 0; j < 8; j++) {
                        if (c & 0x80000000L)
                                c = (c << 1) ^ polynomial;
                        else
                                c = (c << 1);
                }
                crc32_table[i] = c;
        }
	return(crc32_table);
}

static dword	*crc32_table;

dword
crc32c(dword *tbl, byte *ptr, unsigned len, dword crc)
{
	register dword	idx;

	if (tbl == (dword *)0) {
		if (crc32_table == (dword *)0)
			crc32_table = mk_crc32_table(0);
		tbl = crc32_table;
	}
	if (len == 0)
		len = strlen((char *)ptr);
        while (len-- > 0) {
                idx = (crc >> 24) ^ *ptr++;
                crc = (crc << 8) ^ tbl[idx];
        }
        return(crc);
}

/*	EOF	*/
