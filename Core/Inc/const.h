// 	@(#)const.h		SSG100-0.01		2023/01/10		IOE

#ifndef __CONST_H
#define __CONST_H

#define	ENT		0	// Entry
#define	EXT		1	// Exit
#define	ENX		2	// Both
#define	SYS		2	// System

/*
#define ERR_SYNC		1		// 동기 신호 에러
#define ERR_TIMOUT		2 		// 데이터 문자간 타임 에러
#define ERR_CHKSUM   	3		// CRC 오류
#define ERR_RBUFSZ		4    	// 수신 버퍼 사이즈 에러
#define ERR_MODUID		5		// Module ID 값 이상
#define ERR_NOTCOM		6		// 지원되지 않는 명령어
#define ERR_LENGTH		7		// 명령어의 길이 에러
#define ERR_PARAMT   	8		// 명령어 파라미터 에러
#define ERR_NOTMOD  	9		// 명령어가 해당 프로그램 모드에서 지원이 되지 않음.
#define ERR_CFAULT		10 		// 명령어의 잘못된 수행
#define ERR_VERIFY		11
*/

//#define OS_VER      	0x0100  /* OS Version 1.00 */
//#define HW_VER      	0x0100  /* HW Version 1.00 */

#define RESET_CMD		0x01    // Reset cuase by "RESET" cmd

/* Serial Commincation */
#define tochar(ch)	((ch)+0x20)
#define unchar(ch)  ((ch)-0x20)

/* ASCII */
#ifndef NULL
#define NULL	0x00	/* NULL (^@)				*/
#endif

#define SOH		0x01	/*  (^A)					*/
#define STX	    0x02	/*  (^B)					*/
#define ETX	    0x03	/*  (^C)					*/
#define BREAK	0x03	/* break (^C)				*/
#define EOT		0x04	/*  (^D)					*/
#define ENQ		0x05	/*  (^E)					*/
#define ACK		0x06	/*  (^F)					*/
#define BELL	0x07	/* bell (^G)				*/
#define BS		0x08	/* back space (^H)			*/
#define HT		0x09	/* horizontally tab (^I)	*/
#define LF		0x0a	/* line feed (^J)			*/
#define VT		0x0b	/* vertically tab (^K)		*/
#define FF		0x0c	/* form feed (^L)			*/
//#define CR		0x0d	/* carriage return (^M) 	*/
#define SO		0x0e	/* shift out (^N)			*/
#define SI		0x0f	/* shift in (^O)			*/
#define DLE		0x10	/*  (^P)					*/
#define DC1		0x11	/* device control 1 (^Q)	*/
#define DC2		0x12	/* device control 2 (^R)	*/
#define DC3		0x13	/* device control 3 (^S)	*/
#define DC4		0x14	/* device control 4 (^T)	*/
#define NAK		0x15	/*  (^U)					*/
#define SYN		0x16	/*  (^V)					*/
#define ETB		0x17	/*  (^W)					*/
#define CA		0x18	/* cancel line (^X)			*/
#define EM		0x19	/* end of medium (^Y)		*/
#define SUB		0x1a	/*  (^Z)					*/
#define ESC		0x1b	/* escape (^[)				*/
#define FS		0x1c	/*  (^\)					*/
#define GS		0x1d	/*  (^])					*/
#define RS		0x1e	/*  (^^)					*/
#define US		0x1f	/*  (^_)					*/
#define SP      0x20    /* space               		*/

#define XON		DC1
#define XOFF	DC3

//#define RESET	0
//#define SET		1
//#define DISABLE   0
//#define ENABLE	1
#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define NONE	0
#define EXIST	1
#define OFF		0
#define ON		1
#define OPEN	2
#define SHORT1	3
#define OK		0
#define FAIL    (-1)
//#define ERROR	(-1)
#define EQ		0

#endif
/* const.h */
