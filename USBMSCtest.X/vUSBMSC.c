/*******************************************************************************
 * USB MSC HOST CONTROL
 *		
 * author  :hiroshi murakami
 * created :20190728
 * modified:-
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vUSBMSC_C

#include <string.h>		// for memset

#include "mcc_generated_files/system.h"
#include "xprintf.h"

#include "v.h"
#include "vUSBMSC.h"
#include "vTMR1.h"


BDT_ENTRY __attribute__ ((aligned(512)))    BDT[2];
//BDT_ENTRY   *pBDT;

UINT16	*BD;        // BD array access pointer
UINT8 UsbBufCMD64[64];	// Usb buffer for COMMAND
UINT8 UsbBufDAT512[512];	// Usb buffer for DATA

// USB condition
USB_CONDITION USBcondition;
enum eUSB_STATE eUSB_status;
UINT16	EP0_data01, EP1_data01, EP2_data01;

const UINT8 USBMSC_SetAdrsCommand[]   = {0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};	// SET_ADDRESS contents / address = 1
const UINT8 USBMSC_SetConfigCommand[] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};	// SET_CONFIG contents  / config  = 1

//******************************************************************************
//	USB handler (Handle SOF ATTACH DETACH interrupts)
//******************************************************************************
void __attribute__((__interrupt__,no_auto_psv)) _USB1Interrupt(void)
//void vUSB1Interrupt(void)
{
	USBcondition.U1IRsave = U1IR;	// Save USB status
	U1IR = 0xff;	// Clear all status bit
	IFS5bits.USB1IF = 0;	// Clear USB1 interrupt flag
	if(USBcondition.U1IRsave & 0x04)	// SOF interrupts?
		USBcondition.SOFCount++;	// Just increment SOF counter
	if(USBcondition.IsAttach == 0)	// Device is not attached yet?
	{
		if(USBcondition.U1IRsave & 0x40)	// Check attache flag
		{
			U1IEbits.ATTACHIE = 0;	// Disable ATTACH interrupts
			U1IEbits.DETACHIE = 1;	// Enable DETACH interrupts
			USBcondition.IsAttach = 1;	// Set attache flag
//			Delay(100);	// Wait untill USB bus becomes staible
			if(!U1CONbits.JSTATE)	// Connected device is low speed?
			{
				xprintf("LowSpeed!\r\n");
				USBcondition.IsLowSpeed = 1;	// Set low speed flag
			}
			U1ADDR = USBcondition.IsLowSpeed? 0x80: 0x00;	// Reset USB address (Set MSB if low speed device)
			U1EP0 = USBcondition.IsLowSpeed? 0xcd: 0x4d;	// Enable EP0 & Auto retry off (Set MSB if low speed device)
			xprintf("Attached1\r\n");	// Show attached message
//			USB_ResetUsbBus();	// Send BUS RESET condition
//			USB_ResetDataBits();	// Reset DATA0/1 bits
//			MscTotal = 0;	// Clear final sector number
		}
	}
	else	// Device is already attached
	{
		if(USBcondition.U1IRsave & 0x01)	// Detach interrupt?
		{
			//USB_Init();	// Reset USB registers to disable detach and enable attach
        	//USBMSC_init();	// Init USB registers
			U1IEbits.ATTACHIE = 1;	// Disable ATTACH interrupts
			U1IEbits.DETACHIE = 0;	// Enable DETACH interrupts
			USBcondition.IsAttach = 0;	// Set attache flag
			eUSB_status = eUSB_initRegister;
			xprintf("Detached1\r\n");	// Show detached message
		}
	}
}

//******************************************************************************
//	Function Name :vUSBMSC_init
//	Outline		:USB HOST CONTROL initializing registers
//	Input		:
//	Output		:
//******************************************************************************
void USBMSC_init(void)
{
	memset((void*)BDT, 0, 8);	// Clear BDT tables
	U1BDTP1 = (UINT16)BDT >> 8;	// Set BDT address ***********************************
    
	U1EIR = 0;      // Clear all USB error interrupts
	U1IR = 0xff;	// Clear all USB interrupts
	memset((void*)&U1EP0, 0, 32);	// Clear all UEP register

	U1EIE = 0;      // Disable error int
	U1IE = 0;       // Disable all interrupt
	U1PWRCbits.USBPWR = 1;	// Turn power on
	U1ADDR = 0;     // Clear USB address register
	U1SOF = 0x4A;	// Threth hold is 64 bytes
	U1OTGCON = 0x30; // Pull down D+ and D- & VBUS off
	U1CON = 0x08;	// Host mode enable

	U1IEbits.ATTACHIE = 1;	// Enable ATTACH interrupt
	U1IEbits.SOFIE = 1;	// Enable SOFIE interrupt
	U1EIE = 0xbf;	// Enable all error interrupts

	USBcondition.IsAttach = 0;	// Clear attach flag
	USBcondition.IsLowSpeed = 0;	// Reset USB bus speed
//	MscTotal = 0;	// Clear final sector number

	BDT_IN.ADR = (UINT16)UsbBufDAT512;	// Setup IN BD buffer address (max 64 bytes)
	BDT_OUT.ADR = (UINT16)UsbBufCMD64;	// Setup SETUP/OUT BD buffer address (max 64 bytes)

	eUSB_status = eUSB_initRegister;
	
	IEC5bits.USB1IE = 1;	// USB interrupt enable
	IPC21bits.USB1IP = 1;	// USB interrupt priority is one
	EP0_data01 = 0;		//set DATA0/1 bits, at first = data0 
	EP1_data01 = 0;		//set DATA0/1 bits, at first = data0
	EP2_data01 = 0;		//set DATA0/1 bits, at first = data0
}

//******************************************************************************
//	USB transaction 1msec wait for NAK
//******************************************************************************
void USBMSC_wait1msForNAK(void)
{
    if(USBcondition.SOFCountEx != USBcondition.SOFCount)	// 1mS past?
        {
            eUSB_status++;          //next step
        }

}

//******************************************************************************
//	USB transaction Transaction Return
//******************************************************************************
enum eUSB_STATE eUSBMSC_checkTransactionReturn(BDT_ENTRY *pBDT)
{
    if(USBcondition.SOFCountEx != USBcondition.SOFCount)	// 1mS past?
    {
        USBcondition.SOFCountEx = USBcondition.SOFCount;	// Update new tick value
    }
// Handle Stall
    if(U1IRbits.STALLIF)	// Check STALL bit
    {
//        ErrorCode = 0x40;	// Set STALL error code
        U1IRbits.STALLIF = 1;	// Clear STALL bit
		eUSB_status = eUSB_ErrSTALL;
        return eUSB_status;	// STALL return
    }
// Handle Errors
    if(U1IRbits.UERRIF)	// Check error bit
    {
		xprintf( "U1EIR:0x%x\n", U1EIR);
//        ErrorCode = U1EIR;	// Save error status
        U1EIR = 0xff;   // Clear all error status bit
        U1IRbits.UERRIF = 1;	// Clear error bit
		eUSB_status = eUSB_ErrERROR;	// ERROR Return
        return eUSB_status;
    }
// Handle data
    else if(U1IRbits.TRNIF)	// Data transfer bit set?
    {
        U1IRbits.TRNIF = 1;	// Clear data transfer bit
        USBcondition.BDpid = pBDT->STAT.PID;	// Get PID
        USBcondition.BDbyteCount = pBDT->count;	// Get PID
		DEBUG_USB1PRINTF( "PID:0x%02x\n", USBcondition.BDpid);
        if(USBcondition.BDpid == 0x02 || USBcondition.BDpid == 0x03 || USBcondition.BDpid == 0x0b)	// ACK/DATA0/DATA1 PID means success of transaction
		{
            uiTMR001 = 4;		//clear set 4ms time out trap timer for USB write 
            eUSB_status ++;		// Next Step
			return eUSB_status;	// return Next Steatus
		} 
        else if(USBcondition.BDpid == 0x0a)	// NAK PID means USB device is not ready
        {
    		eUSB_status = eUSB_status -2;		// retray but wait SOF change
            return eUSB_NAK;
        }
        else if(USBcondition.BDpid == 0x0e)	// STALL PID?
        {
			eUSB_status = eUSB_ErrSTALL;	// Return as STALL result
			return eUSB_status;
        }
        else	// Unknow pid has come
			xprintf( "Unknow PID:0x%02x\n", USBcondition.BDpid);
			eUSB_status = eUSB_ErrERROR;	// Return as ERROR result
			return eUSB_status;
    }
    
    if(IS_uiTMR001_FINISH)		//time out trap
    {
        eUSB_status = eUSB_ErrTIMEOUT;
        return eUSB_status;	// STALL return
    }
	
    return 0;	// Wait return
}

//******************************************************************************
//	USB transaction status control 
//******************************************************************************
void USBMSC_statusControl(void)
{
//    INT8 res;
    
	switch (eUSB_status) 
    {
	case  	eUSB_initRegister :
		USBMSC_init();
		eUSB_status++;		//next step
		break;
	case  	eUSB_init :
        DEBUG_USB1PUTS("eUSB_init\n");
		eUSB_status++;		//next step
		break;
	case  	eUSB_waitAttach :
		if (USBcondition.IsAttach) 
        {
	        uiTMR001 = 100;		// wait timer set
			eUSB_status++;		//next step
        }
 		break;
	case  	eUSB_waitAttachStable :
        if(IS_uiTMR001_FINISH)
        {
            DEBUG_USB1PUTS("Attached2\n");
			eUSB_status++;		//next step
		}
 		break;
	case  	eUSB_resetUsbBus:
		U1CONbits.SOFEN = 0;	// Turn off SOF's, SOF packet start
        U1CONbits.USBRST = 1;	// Invoke reset
        uiTMR001 = 50;          // wait timer set for 50ms
		eUSB_status++;          //next step
		break;
	case  	eUSB_resetUsbBusWait50ms:
        if(IS_uiTMR001_FINISH)
        {
            U1CONbits.USBRST = 0;	// Release reset
            U1CONbits.SOFEN = 1;	// Turn on SOF's, SOF packet start
            DEBUG_USB1PUTS("Reset USB Bus\n");
	        uiTMR001 = 100;          // wait timer set for 100ms
            eUSB_status++;          //next step
        }
		break;
	case  	eUSB_resetUsbBusWait100ms:
        if(IS_uiTMR001_FINISH)
        {
            eUSB_status++;          //next step
        }
		break;
		
    ////////////////////////////////////////////////////////////////////////////
	case  	eUSB_SETUP_setAddress_start:
        DEBUG_USB1PUTS("Set USB Address start\n");
        memcpy(UsbBufCMD64, USBMSC_SetAdrsCommand, 8);	// Copy SET_ADDRESS request packet to send buffer, with address 1.
        USBcondition.USBcommand = eUSB_SETUP_setAddress_start;
        eUSB_status = eUSB_SETUP_start;          //SETUP!
		break;
	case  	eUSB_SETUP_setAddress_End:
        U1ADDR = (USBcondition.IsLowSpeed? 0x80: 0x00) + USB_ADDRESS_1;	// Set new address
		xprintf( "Set USB Address. U1ADDR:0x%02x\n", U1ADDR);
        eUSB_status++;          //next step
		break;

	case  	eUSB_setConfig_start:
        DEBUG_USB1PUTS("Set USB Config start\n");
        memcpy(UsbBufCMD64, USBMSC_SetConfigCommand, 8);	// Copy SET_CONFIG request packet to send buffer.
        USBcondition.USBcommand = eUSB_setConfig_start;
        eUSB_status = eUSB_SETUP_start;          //SETUP!
		break;
	case  	eUSB_setConfig_End:
//        DEBUG_USB1PUTS("Set USB Config END\n");
        xputs("Set USB Config END\n");
        USBcondition.USBcommand = eUSB_waitDataTransmission;
        eUSB_status = eUSB_waitDataTransmission;          //Read & Write waiting
		break;
    
    ////////////////////////////////////////////////////////////////////////////
	case  	eUSB_SETUP_waitSOF:
        USBMSC_wait1msForNAK();
		break;
	case  	eUSB_SETUP_start:
        DEBUG_USB1PUTS("SETUP start\n");
        BDT_OUT.ADR = (UINT16)UsbBufCMD64;	// SET_CONFIG request packet address to BDT_OUT buffer address.
        BDT_OUT.UOWN_COUNT = 0x8008;	// Setup BD to send 8 byte
        BDT_OUT.STAT.DTS  = 0;		//DATA0/1 bits = 0 at first
        EP0_data01 = 1;	// Flip DATA0/1 bits for next
        U1TOK = U1TOK_PID_TOKEN_SETUP | U1TOK_EP_EP0;	//start transaction
        uiTMR001 = 10;     //time out trap
        eUSB_status++;          //next step
		break;
	case  	eUSB_SETUP_waitTransactionReturn:
		eUSBMSC_checkTransactionReturn(&BDT_OUT);
		break;
	case  	eUSB_SETUP_ACKandCheckBusy_waitSOF:
        USBMSC_wait1msForNAK();
		break;
	case  	eUSB_SETUP_ACKandCheckBusy:
        DEBUG_USB1PUTS("Check Busy\n");
        //next, check the device busy.
        BDT_IN.UOWN_COUNT = 0x80FF;	// Setup BD to get 255 byte
        BDT_IN.STAT.DTS  = 1;
        U1TOK = U1TOK_PID_TOKEN_IN | U1TOK_EP_EP0;	//start transaction
        uiTMR001 = 10;     //time out trap
        eUSB_status++;          //next step
		break;
	case  	eUSB_SETUP_waitBusy:
		eUSBMSC_checkTransactionReturn(&BDT_IN);
		break;
	case  	eUSB_SETUP_End:
        DEBUG_USB1PUTS("SETUP END\n");
        eUSB_status = ++USBcondition.USBcommand;    //return to command & next step
		break;

    ////////////////////////////////////////////////////////////////////////////
	case  	eUSB_waitDataTransmission:
		break;
		
	case  	eUSB_dataIN_waitSOF:
        USBMSC_wait1msForNAK();
		break;
	case  	eUSB_dataIN_start:
        DEBUG_USB1PRINTF("DATA IN START,SOF:%d\n",USBcondition.SOFCount);
//        memcpy(UsbBufDAT512, 0, 64);		//clear the buffer.
        BDT_IN.UOWN_COUNT = 0x8040;			//Setup BD to get 64 byte
        BDT_IN.STAT.DTS  = EP1_data01;		//set DATA0/1 bits 
        U1TOK = U1TOK_PID_TOKEN_IN | U1TOK_EP_EP1_IN;	//start transaction
        uiTMR001 = 10;		//time out trap
        eUSB_status++;		//next step
		break;
	case  	eUSB_dataIN_waitTransactionReturn:
		eUSBMSC_checkTransactionReturn(&BDT_IN);   //when it gets NACK, then retry.
		break;
	case  	eUSB_dataIN_End:
        DEBUG_USB1PUTS("DATA IN END \n");
		EP1_data01 ^= 1;	// Flip DATA0/1 bits for next
        eUSB_status = eUSB_waitDataTransmission;          //return to wait
		break;

		
	case  	eUSB_dataOUT_waitSOF:
//        USBMSC_wait1msForNAK();
        DEBUG_USB1PUTS("DATA OUT waitSOF\n");
        uiTMR001 = 3;		//time wait trap
        eUSB_status++;		//next step
        break;
	case  	eUSB_dataOUT_start:
        if(uiTMR001)		//time wait trap
        {
            break;
        }
        DEBUG_USB1PRINTF("DATA OUT START,SOF:%u\n",USBcondition.SOFCount);
//        memcpy(UsbBufDAT512, xxxComand, 64);		//Copy write data to send buffer.
//        BDT_OUT.UOWN_COUNT = 0x8040;		//Setup BD to send 64 byte
        BDT_OUT.STAT.DTS  = EP2_data01;		//set DATA0/1 bits 
        U1TOK = U1TOK_PID_TOKEN_OUT | U1TOK_EP_EP2_OUT;		//start transaction
        uiTMR001 = 10;		//time out trap
        eUSB_status++;		//next step
        break;
	case  	eUSB_dataOUT_waitTransactionReturn:
		eUSBMSC_checkTransactionReturn(&BDT_OUT);		//when it gets NACK, then retry.
		break;
	case  	eUSB_dataOUT_End:
        DEBUG_USB1PUTS("DATA OUT END \n");
		EP2_data01 ^= 1;		// Flip DATA0/1 bits for next
        eUSB_status = eUSB_waitDataTransmission;		//return to wait
		break;

    ////////////////////////////////////////////////////////////////////////////
	case  	eUSB_Err:
		xputs( "eUSB_Unknown_ERROR\n");
        eUSB_status = eUSB_END;
		break;
	case  	eUSB_ErrTIMEOUT:
		xputs( "eUSB_TIMEOUT_ERROR\n");
        eUSB_status = eUSB_END;
		break;
	case  	eUSB_ErrSTALL:
		xputs( "eSTALL\n");
        eUSB_status = eUSB_END;
		break;

	case	eUSB_ErrERROR:	
		xputs( "eUSB_ERROR\n");
		eUSB_status = eUSB_END;
		break;

	case	eUSB_NAK:	
		xputs( "eUSB_NAK\n");
        eUSB_status = eUSB_END;
		break;

	case	eUSB_END:
		break;

	default:
		eUSB_status = eUSB_initRegister;
	}

}

