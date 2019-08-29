/*******************************************************************************
 *	  RTCC module
 *	  author  :hiroshi murakami
 *	  created :20161225
 *	  modified:-
 ******************************************************************************/
#define _vRTCC_C

#include "mcc_generated_files/mcc.h"
#include <string.h>

#include "xprintf.h"
#include "vRTCC.h"

/*****************************
 * VARIABLES
 *****************************/


//******************************************************************************
//	initialize RTCC property
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_init(void)
{
	//check the currentTime
	RTCC_TimeGet(&currentTime);
	if(currentTime.tm_hour > 23 || currentTime.tm_min > 59 || currentTime.tm_mon > 12 || currentTime.tm_mday > 31 || currentTime.tm_year > 99)
	{
		// set RTCC time 2019-08-11 SUN 12-00-00
		RTCC_Initialize();
//		RTCDATE = 0x19081100;
//		RTCTIME = 0x12000000;
//		RTCCONbits.CAL = 0x3FF;

	} else {
		RTCC_TimeReset(1);
	}
}


//******************************************************************************
//	RTCC time setting
//
//	Description	: -
//	Input		: -
//	Output		:
//******************************************************************************
void vRTCC_timeSet(
	struct tm setupTime
)
{
	short	cal;
	int		cTime, sTime;
	RTCC_TimeGet(&currentTime);
	cTime = ((currentTime.tm_hour * 0x100) + currentTime.tm_min) * 0x100 + currentTime.tm_sec;
	sTime = ((setupTime.tm_hour * 0x100) + setupTime.tm_min) * 0x100 + setupTime.tm_sec;
	cal = RTCCONbits.CAL << 6;
	if(cTime < sTime){
		cal = cal + (1 << 6);
	} else if(cTime > sTime){
		cal = cal - (1 << 6);
	}
	RTCCONbits.CAL = cal >> 6;
	currentTime.tm_hour = setupTime.tm_hour;
	currentTime.tm_min  = setupTime.tm_min;
	currentTime.tm_sec  = setupTime.tm_sec;
	RTCC_TimeSet(&currentTime);			
}

void vRTCC_dateSet(
	struct tm setupTime
)
{
		RTCC_TimeGet(&currentTime);
		currentTime.tm_year = setupTime.tm_year;
		currentTime.tm_mon  = setupTime.tm_mon;
		currentTime.tm_mday = setupTime.tm_mday;
		RTCC_TimeSet(&currentTime);			//setting date & time to RTCC
}

