// 	@(#)gcuif.h		SSG100-0.01		2023/01/10		IOE

#ifndef __GCUIF_H
#define __GCUIF_H

/* IOB와 GCU 간의 통신 프레임(IOB와 원격 관리 프로그램간에도 동일 프레임 구조를 사용)
 * STX(1B:0x02)-DEV(1B:'0'/'1')-SEQ(1B)-CMD(1B)-LEN(2B)-MSG(Max 1029B)-ETX(1B:0x03)-BCC(1B)
 * 양방향 통신으로 구성되어 있으므로 이벤트가 발생한 장치에서 어제든지 전송이 가능하며
 * 전송하는 과정에서 반대쪽에서도 전송할 수 있으므로 현재 수신된 명령어를 잘 구분하여
 * 반대쪽 장비의 명령어인지 아니면 현재 전송한 명령어의 응답인지 구분을 하여야 한다.
 * 전송 명령어는 대문자로 구성되어 있고 그것의 소문자로 응답하도록 설계(IOB와 GCU간의 통신인 경우)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "iobcfg.h"

//#include "STM32H743xx.h"

#define	GCU_POLL	5000	// 폴링 간격(ms 단위)
#define GCU_PTMO	3500	// 응답 패킷 타임 아웃 시간(ms 단위)				//temp_pms_test 500ms ->3.5sec
#define GCU_CTMO	200		// 문자간 타임 아웃 시간(ms 단위)

#define REM_AMCNT	15		// 원격 관리 시스템 조작이 없으면 자동으로 소캣을 닫기 위한 횟수
							// Alive 전송 횟수가 연속해서 이 값 이상이면 소켓을 닫음..
#define	REM_ALIVE	20000	// Alive 패킷 전송 간격(ms 단위, GCU => 원격 관리 프로그램)
#define REM_PTMO	3000	// 응답 패킷 타임 아웃 시간(ms 단위)
#define REM_CTMO	1500	// 문자간 타임 아웃 시간(ms 단위)

#define	MAX_MSGSIZ	1029	// 데이터 메시지 최대 길이(업데이트 명령 기준)
#define MAX_PKTSIZ	1037	// 한 패킷의 최대 길이(MAX_MSGSIZ + 8 = 1037)
#define MIN_PKTSIZ	8
#define DEV_IOB		'0'		// 메시지 수신 장치가 IOB 보드인 경우
#define DEV_GCU		'1'		// 메시지 수신 장치가 GCU 보드인 경우

/* 송수신 패킷 상태 */
#define  PS_NON		0x00	// 패킷 초기 상태
#define  PS_RCV		0x01	// 패킷 수신 상태
#define  PS_SND		0x02	// 패킷 전송 상태
#define  PS_REQ		0x10	// 송신에서는 응답 대기 상태, 수신에서는 응답해야될 명령 수신
#define  PS_ERR		0x20	// 패킷 에러 상태
#define  PS_END		0x40	// 패킷 송수신 완료 상태

// 송수신 패킷 처리용 구조체
typedef struct {
	int		stat;	//
	dword	pms;	// 패킷 전송간 기준 clock
	dword	cms;	// 문자 전송간 기준 clock

	byte	dev;
	byte	seq;	
	byte	cmd;
	word	len;
	byte	data[MAX_PKTSIZ];	// socket send 에서는 전체 전송 패킷을 저장하는 버퍼로 사용
} pack_t;

// 패킷 상태값
enum {
	PK_STX = 0,
	PK_DEV, /* 1 */	
	PK_SEQ,	/* 2 */	
	PK_CMD, /* 3 */
	PK_LEL, /* 4 */
	PK_LEH, /* 5 */
	PK_DAT, /* 6 */
	PK_ETX  /* 7 */
};

// xFstat 상태
enum {
	XF_NONE = 0, // 프로그램 업데이트 없음
	XF_HEAD, 	 // 프로그램 헤더 정보 전송 상태
	XF_DATA, 	 // 프로그램 데이터 정보 전송 상태
	XF_TAIL, 	 // 프로그램 테일 정보 전송 상태
	XF_DONE,	 // 프로그램 업데이트 완료
	XF_UERR, 	 // 프로그램 업데이트 에러
};

#define XF_HEADSIZ	5
#define XF_DATASIZ  1024

// 업데이트 명령어(gcuio.c & wiznet.c & pgmload.c)
struct __packed cU {
	byte	ftype;		// 프레임 구분자
	word	fseq;		// 프레임 seqno
	word	fsiz;		// 프레임 데이터 사이즈
	byte	data[XF_DATASIZ];	
};

// 업데이트 응답(gcuio.c & wiznet.c)
struct __packed rU {
	word	fseq;		// 프레임 seqno
	byte	fack;		// 프레임 응답 코드
};

//////////////////////////////////////
//  IO/GCU 보드간의 송수신 구조체   //
//////////////////////////////////////

// IO/GCU 폴링 및 입출력 전송 명령어(gcuio.c)
struct __packed  cP {
	time_t	time;		// 보드 현재 시간
	word	dipsw;
	S16		temp;		// 보드 온도
	byte	extio[EXP_MAX];	// 입출력 상태값
};


// IO/GCU 폴링 및 입출력 응답 명령어(gcuio.c)
struct __packed  rP {
	byte	vern[4];	// 응답 보드 응용 프로그램 버전
	word	dipsw;
	S16		temp;		// 응답 보드 온도
	byte	extio[EXP_MAX];	// 응답 보드 입출력 상태값
};



#define CAN_MAXBUF		1024 // 8 x 128
// CAN 통신 패킷 처리용 구조체(gcuio.c)
typedef struct {
	int		stat;	//
	dword	sms;	// 송신 패킷 clock
	dword	rms;	// 수신 패킷 clock
	int		sseq;
	int		rseq;
	byte	scmd;
	byte	rcmd;
	byte	data[CAN_MAXBUF];
} cpack_t;

// CAN 통신 보드 정보 명령어(gcuio.c)
struct __packed  cI {
	byte	cmd;
	byte	vern[3];	// 보드 응용 프로그램 버전
	word	dipsw;
	S16		temp;		// 응답 보드 온도
};

//////////////////////////////////////
//  원격관리 PGM 간의 송수신 구조체 //
//////////////////////////////////////

// 장비 상태 응답 메시지(원격 관리 "장비상태" 탭의 명령에서 응답)
struct __packed rStat {
	byte	ack;		// 0 : 응답 상태
	byte	gateno;		// 1 : 장비 번호(SsgCfg.gateno)
	word	statno;		// 2.3 : 역사 코드(SsgCfg.statno)
	byte 	agmtype;	// 4 : 장비 타입(AgmType)
	byte 	agmmode;	// 5 : 장비 모드(AgmMode)
	byte 	swtmode;	// 6 : 스위치 장비 모드(SwtMode)
#if 1
	byte 	smsmode[2];	// 7.8 : RF단말기 장비 모드(SmsMode[ENX])
	byte 	remmode;	// 9 : 원격 장비 모드(RemMode)
	byte	remctrl;	// 10 : 원격 제어 상태값(RemCtrl)
	byte 	lstmode;	// 11 : 최종 모드 적용 조건
	time_t 	lsttime;	// 12.15 : 최종 모드 변경 시간
#else
	byte	smsmode;	// 7 : RF단말기 장비 모드(SmsMode)
	byte	remmode;	// 8 : 원격 장비 모드(RemMode)
	byte	remctrl;	// 9 : 원격 제어 상태값(RemCtrl)
	word	remport;	// 10.11 : 원격 TCP 통신 포트 번호(SsgCfg.port)
	byte	remip[4];	// 12.15 : 원격 IP 정보(SsgCfg.ip[4])
#endif
	byte	iobvern[4];	// 16.19 : IOB 버전(Version[8]~Version[11])
	byte	iobtemp[2];	// 20.21 : IOB 온도(TempData)
	byte	gcuvern[4];  // 22.25 : GCU 버전(GcuVern[])
	byte	gcutemp[2];  // 26.27 : GCU 온도(GcuTemp)
	byte	gcuonln;    // 28 : GCU RS232 통신 상태(GcuOnLine)
	byte	canonln;	// 29 : GCU CAN 통신 상태(CanOnLine)
	byte	tagserv[2];	// 30.31 : 태그 상태(TagServ)
	word	battime;	// 32.33 : 배터리 설정값(SsgCfg.batime)
	DWORD	mchstat;	// 34.37 : 장비 알람(MchStat)
	word	dooralm;	// 38.39 : 도어 알람(DoorAlarm)
	word	sensalm;	// 40.41 : 센서 알람(SensAlarm)
	word	flapalm;   	// 42.43 : 플랩 알람(FlapAlarm)
	word	doormsk;	// 44.45 : 도어 마스크 설정값(DoorMask)
	word	sensmsk;	// 46.47 : 센서 마스크 설정값(SensMask)
	word	haltmsk;	// 48.49 : 중지 마스크 설정값(HaltMask)
	word	hubstat[5];	// 50.59 : 허브 상태
	DWORD	flapcnt[4]; // 60.75 : 플랩 카운터(FlapCnt[MAX_FLAP])
	DWORD	nopicnt[2]; // 76.83 : 무단진입 카운터(NopICnt[ENX])
	DWORD	nopocnt[2]; // 84.91 : 무단통과 카운터(NopOCnt[ENX])
	word	doorsw;		// 92.93 : 도어 상태(DoorSw)
	byte	flapsts[4];	// 94.97 : 플랩 상태(FlapStat[MAX_FLAP])
	byte	sensor[2];	// 98.99 : 센서 상태(Sensor[ENX])
	
	byte	flapctl[4];	// 100.103 : 플랩 제어값(FlapCntl[MAX_FLAP])
	byte	apprctl[2];	// 104.105 : 방향 표시기 제어값(Appr[ENX])
	byte	abarctl[2]; // 106.107 : 할인바 제어값(APBar[ENX])
	byte	toplctl[2]; // 108.109 : 상단램프 제어값(TLamp[ENX])
	byte	ledlctl[2]; // 110.111 : LED-Bar 제어값(LLamp[ENX])
	byte	buzzctl;	// 112 : 부저 제어값(Buzzer)
	byte	firetyp;	// 113 : 화재발생 타입(FireType)
	byte	rfconn[2];	// 114.115 : RF단말기 사용여부(RfConn[ENX])
	byte	rfstat[2];	// 116.117 : RF단말기 운영 상태값(RfStat[ENX])
#if 1
	byte	rfu[10];	// 118.127 : 예비 영역
#else
	byte	remsn[4];	// 118.121 : 원격 Subnet Mask
	byte	remgw[4];	// 122.125 : 원격 Gateway IP
	byte	rfu[2];		// 126.127 : 예비 영역
#endif
};

// 장비 제어 응답 메시지(원격 관리 "테스트 및 설정" 탭의 명령에서 응답)
struct __packed rCtrl {
	byte	ack;		// 0 : 응답 상태
	byte 	agmmode;	// 1 : 장비 모드(AgmMode)
	byte	flapctl[4];	// 2.5 : 플랩 제어값(FlapCntl[MAX_FLAP])
	byte	flapsts[4];	// 6.9 : 플랩 상태(FlapStat[MAX_FLAP])
	DWORD	flapcnt[4]; // 10.25 : 플랩 카운터(FlapCnt[MAX_FLAP])
	byte	apprctl[2];	// 26.27 : 방향 표시기 제어값(Appr[ENX])
	byte	abarctl[2]; // 28.29 : 할인바 제어값(APBar[ENX])
	byte	toplctl[2]; // 30.31 : 상단램프 제어값(TLamp[ENX])
	byte	ledlctl[2]; // 32.33 : LED-Bar 제어값(LLamp[ENX])
	byte	buzzctl;	// 34 : 부저 제어값(Buzzer)
	byte	remctrl;	// 35 : 원격 제어 상태값(RemCtrl)
	word	battime;	// 36.37 : 배터리 설정값(BatTime)
	word	doormsk;	// 38.39 : 도어 마스크 설정값(DoorMask)
	word	sensmsk;	// 40.41 : 센서 마스크 설정값(SensMask)
	word	haltmsk;	// 42.43 : 중지 마스크 설정값(HaltMask)
	DWORD	rfu;		// 44.47 : 예비 영역
};
#endif

// EOF
