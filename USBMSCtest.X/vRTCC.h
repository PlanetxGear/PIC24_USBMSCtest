/*******************************************************************************
 *	RTCC module
 *	author  :hiroshi murakami
 *	created :20161225
 *	modified:-
 ******************************************************************************/
#ifndef _vRTCC_H
#define _vRTCC_H

#include "mcc_generated_files/mcc.h"
#include "integer.h"
#include "v.h"

/*****************************
 * VARIABLES
 *****************************/


typedef enum{
	eAlarmT11_idle,
	eAlarmT11_open,
	eAlarmT11_repeat,
	eAlarmT11_busy,
	eAlarmT11_end
} ALARM_TASK11_STATUS;

typedef struct{
	int iAlarmRepeat;
	int pre_iAlarmRepeat;
	int iAlarmRepeatCountdown;
	ALARM_TASK11_STATUS status;
} ALARM_TASK11_PROPERTY;

typedef struct{
	char cFlagON;			//0:off 1:on
	char cLCDoutPutChar;	//LCD output character for Flag
} RTCC_FLAGS;

#if defined _vRTCC_C
	char cFlag1Minute;

//	char cFlagAlarmON;			//0:off 1:on
//	char cFlagChimeON;			//0:off 1:on
	RTCC_FLAGS alarm;
	RTCC_FLAGS chime;

	struct tm currentTime;
	struct tm alarmTime;
	struct tm setupTime;
	struct tm pre_setupTime;

	ALARM_TASK11_PROPERTY alarmTask11;

	BYTE bPreviousHHMM[5];
	BYTE bCurrentHHMM[5];
	BYTE bNextHHMM[5];
	BYTE bChimeFile[] = "chime.txt";
	BYTE bAlarmHHMM[5];
	BYTE bAlarmPath[] = "/Alarm";

#else
	extern char cFlag1Minute;

//	extern char cFlagAlarmON;		//0:off 1:on
//	extern char cFlagChimeON;		//0:off 1:on
	extern RTCC_FLAGS alarm;
	extern RTCC_FLAGS chime;
	extern BYTE bPreviousHHMM[5];
	extern BYTE bCurrentHHMM[5];
	extern BYTE bNextHHMM[5];

	extern struct tm currentTime;
	extern struct tm alarmTime;
	extern struct tm setupTime;
	extern struct tm pre_setupTime;
	
//	extern BYTE bAlarmTime[5];
	extern ALARM_TASK11_PROPERTY alarmTask11;
	extern BYTE bAlarmHHMM[5];

#endif

/*****************************
 * PROTOTYPES
 *****************************/
void vRTCC_init(void);
void vRTCC_timeSet(struct tm setupTime);
void vRTCC_dateSet(struct tm setupTime);


#endif

