/*******************************************************************************
 *	  Switch
 *	  author  :hiroshi murakami
 *	  created :20161225
 *	  modified:-
 ******************************************************************************/
#define _vSWITCH_C

#include "string.h"
#include "vLCD_MENU.h"
#include "vSWITCH.h"
#include "vTMR1.h"


//******************************************************************************
//  Switch SW1 (for LCD menu12 status)
//
//	Description	:
//  SW1 go forward the LCD menu status.
//  this section should be callded every 10msec.
//  it doesn't use repeat.
//******************************************************************************
void vSW1_Check(void)
{

	if(isSW1ON){	//switch ON when an Input port is low.
		switch(usSW1_OnTime)
		{
			case 0:
				//user handling here
				vSW1_UserHandling();
				usSW1_OnTime++;
				break;
/*
			case 100:					//10msec*200 wait for the first push
				//user handling here
				vSW1_UserHandling();
				usSW1_OnTime = 100 - 30; 	//10msec*30 wait for repeat
				break;
*/
			default:
				break;
		}
//		usSW1_OnTime++;
	}else{
		usSW1_OnTime = 0;
	}
}

/*****************************
* SW1 User Handling
*****************************/

void vSW1_UserHandling(void)
{
	//bring forward the Switch menu status
	LcdMenu12.status++;
	if(LcdMenu12.status > eLcdM12_end){
		LcdMenu12.status = eLcdM12_idle;
	}
	
	//clear the Switch menu waiting time.
	uiTMR007 = 0; //msec period

}

//******************************************************************************
//   Switch SW2
//******************************************************************************
/*****************************
* push SW2 on/off check section
* this section is set at 10msec section.
* it has repeat.
*****************************/
void vSW2_Check(void)
{

	if(isSW2ON){	//switch ON when an Input port is low.
		switch(usSW2_OnTime)
		{
			case 0:
				//user handling here
				vSW2_UserHandling();
				break;
			case 100:					//10msec*100 wait after the first push
				//user handling here
				vSW2_UserHandling();
				usSW2_OnTime = 100 - 30; 	//10msec*30 wait for Next repeat
				break;
			default:
				break;
		}
		usSW2_OnTime++;
	}else{
		usSW2_OnTime = 0;
	}
}

/*****************************
* SW2 User Handling
*****************************/
void vSW2_UserHandling(void)
{
	cSW2_flag = SWITCH_ON;
}

/*****************************
* SW2 test
*****************************/
char cSW2_ON(void)
{
	if(cSW2_flag == SWITCH_ON){
		cSW2_flag = SWITCH_OFF;
		return 1;
	}
	return 0;
}

//******************************************************************************
//   Switch SW3
//******************************************************************************
//****************************
// push Switch SW3 on/off check section
// this section is set at 10msec section.
// it has repeat.
//****************************
void vSW3_Check(void)
{

	if(isSW3ON){	//switch ON when an Input port is low.
		switch(usSW3_OnTime)
		{
			case 0:
				//user handling here
				vSW3_UserHandling();
				break;
			case 100:					//10msec*100 wait after the first push
				//user handling here
				vSW3_UserHandling();
				usSW3_OnTime = 100 - 30; 	//10msec*30 wait for Next repeat
				break;
			default:
				break;
		}
		usSW3_OnTime++;
	}else{
		usSW3_OnTime = 0;
	}
}

//****************************
// SW3 User Handling
//****************************
void vSW3_UserHandling(void)
{
	cSW3_flag = SWITCH_ON;
}

//****************************
// SW3 test
//****************************
char cSW3_ON(void)
{
	if(cSW3_flag == SWITCH_ON){
		cSW3_flag = SWITCH_OFF;
		return 1;
	}
	return 0;
}

//******************************************************************************
//   Switch SW4
//******************************************************************************
// CANCEL SWITCH
//****************************
// push Switch SW4 on/off check section
// this section is set at 10msec section.
// it has repeat.
//****************************
void vSW4_Check(void)
{

	if(isSW4ON){	//switch ON when an Input port is low.
		switch(usSW4_OnTime)
		{
			case 0:
				//user handling here
				vSW4_UserHandling();
				break;
			case 100:					//10msec*100 wait after the first push
				//user handling here
				vSW4_UserHandling();
				usSW4_OnTime = 100 - 1; 	//10msec*1 wait for Next repeat. *SW4 use for cancel
				break;
			default:
				break;
		}
		usSW4_OnTime++;
	}else{
		usSW4_OnTime = 0;
	}
}

//****************************
// SW4 User Handling
//****************************
void vSW4_UserHandling(void)
{
	cSW4_flag = SWITCH_ON;
}

//****************************
// SW4 test
//****************************
char cSW4_ON(void)
{
	if(cSW4_flag == SWITCH_ON){
		cSW4_flag = SWITCH_OFF;
		return 1;
	}
	return 0;
}

//****************************
// switchs clear
//****************************
void vSWs_clear(void)
{
	cSW2_flag = SWITCH_OFF;
	cSW3_flag = SWITCH_OFF;
	cSW4_flag = SWITCH_OFF;
	usSW1_OnTime = 0;
	usSW2_OnTime = 0;
	usSW3_OnTime = 0;
	usSW4_OnTime = 0;
}

