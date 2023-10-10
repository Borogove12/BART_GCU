
#ifndef INC_STRUCT_H_
#define INC_STRUCT_H_

#include "bsp_types.h"

//configuration table
typedef struct {
	short	size;		// sizeof(struct config)
	word	HwVersion;	// H/W version
	word	OsVersion;	// OS version
	word	DipStatus;	// status of DIP switch
	byte	rBackup;	// Rtc Backup
	byte	mBackup;	// Memory Backup
	byte	ResetType;	//
	byte	RtcType;
	time_t  sec;        // Every 1sec increment
	dword   hund;       // Every 100msec increment to 10
	clock_t tick;       // Every 10msec increment
	dword   RunTime;    // Every 100msec increment
	byte	backup[16]; // backup pattern
}	__attribute__((packed)) CONFIG;


extern 	time_t  SysInitTime;

#endif /* INC_STRUCT_H_ */
