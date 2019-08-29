/*************************************************
*  Switch_Menu処理ヘッダファイル
*    vPushSwitch_init()  ----- 初期化
*    vSW2_Check() ----- SW2_ チェック＆repeat処理
*    vSW2_UserHandling() ----- SW2_が押された時のユーザ処理を記述する
*    char cSW2_ON() ----- SW2_の状態を返信する。フラグをクリアする。
*    vSW1_Check() ----- SW1_ チェック＆repeat処理
*    vSW1_UserHandling() ----- SW2_が押された時のユーザ処理を記述する
*************************************************/
#ifndef _vSWITCH_H
#define _vSWITCH_H

#include "mcc_generated_files/mcc.h"
#include "integer.h"

//pushbuttons define
#define SW1		PORTBbits.RB2	//bring forward the LCD menu status.
#define SW2		PORTBbits.RB3	//for Number Addition. * RB3 shareing with ADC
#define SW3		PORTAbits.RA2	//for Number Subtraction.
#define SW4		PORTAbits.RA3	//for cancellation.
#define isSW1ON	(PORTBbits.RB2==0)	//Low is ON. bring forward the LCD menu status
#define isSW2ON	(PORTBbits.RB3==0)	//Low is ON. for Number Addition
#define isSW3ON	(PORTAbits.RA2==0)	//Low is ON. for Number Subtraction
#define isSW4ON	(PORTAbits.RA3==0)	//Low is ON. for cancellation
#define SWITCH_ON		1
#define SWITCH_OFF		0


/*****************************
 * VARIABLES
 *****************************/
#if defined _vSWITCH_C
	char cSW2_flag = 0;		
	char cSW3_flag = 0;		
	char cSW4_flag = 0;		

	unsigned short usSW1_OnTime = 0;
	unsigned short usSW2_OnTime = 0;
	unsigned short usSW3_OnTime = 0;
	unsigned short usSW4_OnTime = 0;
#else
#endif


/*****************************
 * PROTOTYPES
 *****************************/
void vSW1_Check(void);
void vSW1_UserHandling(void);
void vSW2_Check(void);
void vSW2_UserHandling(void);
char cSW2_ON(void);
void vSW3_Check(void);
void vSW3_UserHandling(void);
char cSW3_ON(void);
void vSW4_Check(void);
void vSW4_UserHandling(void);
char cSW4_ON(void);
void vSWs_clear(void);

#endif
