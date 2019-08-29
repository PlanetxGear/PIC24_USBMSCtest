/*******************************************************************************
 * USB MSC HOST CONTROL
 *		
 * author  :hiroshi murakami
 * created :20190728
 * modified:-
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vUSBMSC_H
#define _vUSBMSC_H

#include "integer.h"

/*****************************
 * STATE
 *****************************/
enum eUSB_STATE {
	eUSB_initRegister = 0,
	eUSB_init,
	eUSB_waitAttach,
	eUSB_waitAttachStable,
	eUSB_resetUsbBus,
    	eUSB_resetUsbBusWait50ms,
    	eUSB_resetUsbBusWait100ms,		// for USB stable
//	eUSB_setAddress,
    eUSB_SETUP_setAddress_start,
    eUSB_SETUP_setAddress_End,
//	eUSB_setConfig,
   	eUSB_setConfig_start,
   	eUSB_setConfig_End,
        
	eUSB_SETUP_waitSOF,
    	eUSB_SETUP_start,
        eUSB_SETUP_waitTransactionReturn,
        eUSB_SETUP_ACKandCheckBusy_waitSOF,
        eUSB_SETUP_ACKandCheckBusy,
    	eUSB_SETUP_waitBusy,
    	eUSB_SETUP_End,

	eUSB_waitDataTransmission,
	
	eUSB_dataIN_waitSOF,
		eUSB_dataIN_start,
		eUSB_dataIN_waitTransactionReturn,
		eUSB_dataIN_End,
	eUSB_dataOUT_waitSOF,
		eUSB_dataOUT_start,
		eUSB_dataOUT_waitTransactionReturn,
		eUSB_dataOUT_End,

	eUSB_Detach,
	eUSB_NAK,
	eUSB_Err,
    	eUSB_ErrTIMEOUT,
		eUSB_ErrSTALL,
		eUSB_ErrERROR,
	eUSB_END
};

/*****************************
 * DEFINES
 *****************************/
#define USB_ADDRESS_1               1           // ADDRESS = 1
#define U1TOK_PID_TOKEN_SETUP		((UINT16)0b11010000)
#define U1TOK_PID_TOKEN_IN			((UINT16)0b10010000)
#define U1TOK_PID_TOKEN_OUT			((UINT16)0b00010000)
#define U1TOK_EP_EP0				((UINT16)0x00)
#define U1TOK_EP_EP1_IN				((UINT16)0x01)
#define U1TOK_EP_EP2_OUT			((UINT16)0x02)

#define USB_INITIALIZE()		(eUSB_status = eUSB_initRegister)
#define USB_DATAIN()  			(eUSB_status = eUSB_dataIN_start)
#define USB_DATAOUT() 			(eUSB_status = eUSB_dataOUT_start)
//#define USB_ADDRESS_GetACKandCheckBusy			(eUSB_status = eUSB_setAddress_GetACKandCheckBusy)

#define BDT_IN						BDT[0]           // EP0 IN Buffer Descriptor
#define BDT_OUT						BDT[1]           // EP0 OUT Buffer Descriptor

// Buffer Descriptor Status Register layout.
typedef union _BD_STAT
{
    struct{
        unsigned            :2;     //Byte count
        unsigned    BSTALL  :1;     //Buffer Stall Enable
        unsigned    DTSEN   :1;     //Data Toggle Synch Enable
        unsigned            :2;     //Reserved - write as 00
        unsigned    DTS     :1;     //Data Toggle Synch Value
        unsigned    UOWN    :1;     //USB Ownership
    };
    struct{
        unsigned            :2;
        unsigned    PID0    :1;
        unsigned    PID1    :1;
        unsigned    PID2    :1;
        unsigned    PID3    :1;
    };
    struct{
        unsigned            :2;
        unsigned    PID     :4;     // Packet Identifier
    };
    UINT8            Val;
} BD_STAT;                      //Buffer Descriptor Status Register


// BDT Entry Layout
typedef union __BDT
{
    union
    {
        struct
        {
            UINT8       CNT;
            BD_STAT     STAT __attribute__ ((packed));
        };
        struct
        {
            UINT16        count:10;   //test
            UINT8        :6;
            UINT16        ADR; //Buffer Address
        };
        struct
        {
            UINT16        UOWN_COUNT;   //test
            UINT16        ADR_; //Buffer Address
        };
    };
    UINT32              Val;
    UINT16              v[2];
} BDT_ENTRY;

typedef struct __USB_CONDITION
{
 	enum eUSB_STATE		USBcommand;
	UINT16	IsLowSpeed;     //Low speed device flag. 1:Low Speed 0:Full Speed,  
	UINT16	IsAttach;       //Attache flag. 1:attach 0:detach
	UINT16	SOFCount;       // SOF freame counter
	UINT16	SOFCountEx;     // SOF freame counter
	UINT16	U1IRsave;       // U1IR register save
	UINT16	BDpid;          // U1IR register save
	UINT16	BDbyteCount;    // U1IR register save
#ifdef _V_DEBUG_SCSI2
	UINT16	SOFCountSt;     // SOF freame counter save
#endif
} USB_CONDITION;

/*****************************
 * VARIABLES
 *****************************/
//extern UINT8 UsbBuf64[64];	// Usb send buffer
extern UINT8 UsbBufDAT512[512];	// Usb buffer for DATA
extern UINT8 UsbBufCMD64[64];	// Usb buffer for COMMAND

extern enum eUSB_STATE	eUSB_status;
extern BDT_ENTRY		BDT[2];
extern USB_CONDITION	USBcondition;


/*****************************
 * PROTOTYPES
 *****************************/

void USBMSC_init(void);

void USBMSC_statusControl(void);

enum eUSB_STATE eUSBMSC_checkTransactionReturn(BDT_ENTRY   *pBDT);


#endif
