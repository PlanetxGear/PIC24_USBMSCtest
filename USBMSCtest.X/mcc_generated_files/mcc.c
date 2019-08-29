/**
  @Generated PIC24 / dsPIC33 / PIC32MM MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.c

  @Summary:
    This is the mcc.c file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.125
        Device            :  PIC24FJ64GB002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36B
        MPLAB             :  MPLAB X v5.20
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/


// Configuration bits: selected in the GUI

// CONFIG4
#pragma config DSWDTPS = DSWDTPS3    //DSWDT Postscale Select->1:128 (132 ms)
#pragma config DSWDTOSC = LPRC    //Deep Sleep Watchdog Timer Oscillator Select->DSWDT uses Low Power RC Oscillator (LPRC)
#pragma config RTCOSC = SOSC    //RTCC Reference Oscillator  Select->RTCC uses Secondary Oscillator (SOSC)
#pragma config DSBOREN = OFF    //Deep Sleep BOR Enable bit->BOR disabled in Deep Sleep
#pragma config DSWDTEN = OFF    //Deep Sleep Watchdog Timer->DSWDT disabled

// CONFIG3
#pragma config WPFP = WPFP0    //Write Protection Flash Page Segment Boundary->Page 0 (0x0)
#pragma config SOSCSEL = LPSOSC    //Secondary Oscillator Pin Mode Select->SOSC pins in Low-Power (low drive-strength) Oscillator Mode
#pragma config WUTSEL = LEG    //Voltage Regulator Wake-up Time Select->Default regulator start-up time used
#pragma config WPDIS = WPDIS    //Segment Write Protection Disable->Segmented code protection disabled
#pragma config WPCFG = WPCFGDIS    //Write Protect Configuration Page Select->Last page and Flash Configuration words are unprotected
#pragma config WPEND = WPENDMEM    //Segment Write Protection End Page Select->Write Protect from WPFP to the last page of memory

// CONFIG2
#pragma config POSCMOD = XT    //Primary Oscillator Select->XT Oscillator mode selected
#pragma config I2C1SEL = PRI    //I2C1 Pin Select bit->Use default SCL1/SDA1 pins for I2C1 
#pragma config IOL1WAY = OFF    //IOLOCK One-Way Set Enable->The IOLOCK bit can be set and cleared using the unlock sequence
#pragma config OSCIOFNC = OFF    //OSCO Pin Configuration->OSCO pin functions as clock output (CLKO)
#pragma config FCKSM = CSECMD    //Clock Switching and Fail-Safe Clock Monitor->Sw Enabled, Mon Disabled
#pragma config FNOSC = PRIPLL    //Initial Oscillator Select->Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL)
#pragma config PLL96MHZ = ON    //96MHz PLL Startup Select->96 MHz PLL Startup is enabled automatically on start-up
#pragma config PLLDIV = NODIV    //USB 96 MHz PLL Prescaler Select->Oscillator input used directly (4 MHz input)
#pragma config IESO = OFF    //Internal External Switchover->IESO mode (Two-Speed Start-up) disabled

// CONFIG1
#pragma config WDTPS = PS1    //Watchdog Timer Postscaler->1:1
#pragma config FWPSA = PR128    //WDT Prescaler->Prescaler ratio of 1:128
#pragma config WINDIS = OFF    //Windowed WDT->Standard Watchdog Timer enabled,(Windowed-mode is disabled)
#pragma config FWDTEN = OFF    //Watchdog Timer->Watchdog Timer is disabled
#pragma config ICS = PGx1    //Emulator Pin Placement Select bits->Emulator functions are shared with PGEC1/PGED1
#pragma config GWRP = OFF    //General Segment Write Protect->Writes to program memory are allowed
#pragma config GCP = OFF    //General Segment Code Protect->Code protection is disabled
#pragma config JTAGEN = OFF    //JTAG Port Enable->JTAG port is disabled

#include "mcc.h"
#include "clock.h"


void OSCILLATOR_Initialize(void)
{
    CLOCK_Initialize();
}

/**
 End of File
*/