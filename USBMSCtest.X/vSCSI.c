/*******************************************************************************
 * SCSI COMAND CONTROL
 *		
 * author  :hiroshi murakami
 * created :20190728
 * modified:-
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vSCSI_C

#include <string.h>		// for memset

#include "mcc_generated_files/system.h"
#include "xprintf.h"

#include "v.h"
#include "vSCSI.h"
#include "vUSBMSC.h"
//#include "vTMR1.h"


// SCSI condition
SCSI_CONDITION SCSIcondition;
enum eSCSI_STATE eSCSI_status;


// SCSI command
const UINT8 SCSIrequestSense[] = {	// REQUEST SENSE command contents(SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x03,
	0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIreadCapacity[] = {	// READ CAPACITY command contents(SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x25,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIreadCmd[] = {	// READ command contents (SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x0c, 0x28,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 SCSIwriteCmd[] = {	// WRITE command contents (SCSI)
	0x55, 0x53, 0x42, 0x43, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x2a,
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT16 ScsiTable[] =	// SCSI command table
{(UINT16)SCSIrequestSense, (UINT16)SCSIreadCapacity, (UINT16)SCSIreadCmd, (UINT16)SCSIwriteCmd};// 0,1,2,3

//******************************************************************************
//	Function Name :vSCSI_init
//	Outline		:SCSI CONTROL initializing 
//	Input		:
//	Output		:
//******************************************************************************
void SCSI_init(void)
{
    eSCSI_status = eSCSI_start;
}

//******************************************************************************
//	SCSI transaction return
//******************************************************************************
void SCSI_checkTransactionReturn(void)
{
    if (eUSB_status == eUSB_waitDataTransmission) 
    {
        eSCSI_status++;		//next step
    }else if((eUSB_status >= eUSB_Err))
    {
        xputs("SCSI USB error\n");
        eSCSI_status = eSCSI_END;
//        eUSB_status = eUSB_init;
    }else if((eUSB_status < eUSB_waitDataTransmission))
    {
        xputs("SCSI USB status error\n");
        eSCSI_status = eSCSI_END;
//        eUSB_status = eUSB_init;
    }

}

//******************************************************************************
//	Set sector number into SCSI command contents
//******************************************************************************
void SetSector(UINT32 secterNumber, UINT8 buf[])
{
	buf[17] = secterNumber >> 24;	// Set sector number 1
	buf[18] = secterNumber >> 16;	// Set sector number 2
	buf[19] = secterNumber >> 8;	// Set sector number 3
	buf[20] = secterNumber & 255;	// Set sector number 4
}

//******************************************************************************
//	check DSW parse
//******************************************************************************
enum eSCSI_ERR_STATE SCSI_isErrInCSW(INT16 dataSize, UINT8 buf[])
{
//UINT8 *s;
	if(dataSize != 13)	// Check size first
    {
        SCSIcondition.ErrorCode = eSCSI_ERR_dataSize;	// error return
		return SCSIcondition.ErrorCode;
    }
//	s = UsbBufCMD64;	// Set parse pointer
//	if(*buf != 'U' || *(buf + 1) != 'S' || *(buf + 2) != 'B'|| *(buf + 3) != 'S')
	if(buf[0] != 'U' || buf[1] != 'S' || buf[2] != 'B'|| buf[3] != 'S')
    {
        SCSIcondition.ErrorCode = eSCSI_ERR_signature;  // Check signature and failed return
		return SCSIcondition.ErrorCode;
    }
//	if(*(buf + 12))	// Some error (01 cmd err 02 phase error) found?
	if(buf[12])	// Some error (01 cmd err 02 phase error) found?
    {
        SCSIcondition.ErrorCode = eSCSI_ERR_some;  
		return SCSIcondition.ErrorCode;
    }
    SCSIcondition.ErrorCode = eSCSI_NO_ERR;  
    return SCSIcondition.ErrorCode; // Normal return
}

//******************************************************************************
//	SCSI are there remaining data? then decide next step
//******************************************************************************
void SCSI_checkRemaingData(void)
{
	SCSIcondition.DataLength = SCSIcondition.DataLength - 64;
	if (SCSIcondition.DataLength > 0)
	{
		SCSIcondition.UsbBuffAddr = SCSIcondition.UsbBuffAddr + 64;
		eSCSI_status = eSCSI_putData;	//next data..
	}else
	{
		DEBUG_SCSI1PUTS("SCSI putDATA End\n");
		eSCSI_status = eSCSI_CSW;		//exit data putting.
	}
}

//******************************************************************************
//	SCSI transaction status control 
//******************************************************************************
void SCSI_statusControl(void)
{
int	i;
#ifdef _V_DEBUG_SCSI2
char* ptr;
int ofs;
#endif

	switch (eSCSI_status) 
    {
	case  	eSCSI_start :
		break;
	case  	eSCSI_waitCommand :
		SCSIcondition.SCSIcommand = 0;
		break;

	////////////////////////////////////////////////////////////////////////////
	case  	eSCSI_requestSense :
        DEBUG_SCSI1PUTS("SCSI requestSense command\n");
//		SCSIcondition.SCSIcommand = eSCSI_requestSense;
		memcpy(UsbBufCMD64, SCSIrequestSense, 31);	// SCSIrequestSense command.
		
		SCSIcondition.SCSIcommand = eSCSI_requestSense;
		SCSIcondition.DataLength = 64;		//data 64 byte.
		SCSIcondition.UsbBuffAddr = UsbBufDAT512;		//data buffer address.
        USBcondition.SOFCountEx = USBcondition.SOFCount;        //for wait new SOF
		SCSIcondition.SCSIcommand = eSCSI_status;
		eSCSI_status = eSCSI_CBW;		//go SCSI command
		break;
	case  	eSCSI_requestSenseEND :
        DEBUG_SCSI2PUTS("SCSI requestSense commandEND\n");
		eSCSI_status = eSCSI_waitCommand;
		break;

	////////////////////////////////////////////////////////////////////////////
	case  	eSCSI_readCapacity :
        DEBUG_SCSI1PUTS("SCSI readCapacity command\n");
//		SCSIcondition.SCSIcommand = eSCSI_readCapacity;
		memcpy(UsbBufCMD64, SCSIreadCapacity, 31);	// SCSI readCapacity command.

		SCSIcondition.DataLength = 64;		//data 64 byte.
		SCSIcondition.UsbBuffAddr = UsbBufDAT512;		//data buffer address.
        USBcondition.SOFCountEx = USBcondition.SOFCount;        //for wait new SOF
		SCSIcondition.SCSIcommand = eSCSI_status;
		eSCSI_status = eSCSI_CBW;		//go SCSI command
		break;
	case  	eSCSI_readCapacityEND :
        DEBUG_SCSI2PUTS("SCSI readCapacity END\n");
		//if(res == 8)	// Device must return 8 byte capacity information?
		for(i = 0; i < 4; i++)	// Get 4 byte value
		{
			SCSIcondition.MscTotal = (SCSIcondition.MscTotal << 8) + UsbBufDAT512[i];	// Get 32bit number
		}
		DEBUG_SCSI2PRINTF("LastSecter:0x%04x\n",SCSIcondition.MscTotal);
		eSCSI_status = eSCSI_waitCommand;
		break;

	////////////////////////////////////////////////////////////////////////////
	case  	eSCSI_read :
        DEBUG_SCSI1PUTS("SCSI Read command\n");
        DEBUG_SCSI1PRINTF("SOF START:%u\n", USBcondition.SOFCount);
        #ifdef _V_DEBUG_SCSI2
            USBcondition.SOFCountSt = USBcondition.SOFCount;
        #endif
//		SCSIcondition.SCSIcommand = eSCSI_read;
		memcpy(UsbBufCMD64, SCSIreadCmd, 31);	//SCSI read command.
		SetSector(SCSIcondition.MscTotal,UsbBufCMD64);		//set final sector number to command.

		SCSIcondition.DataLength = 512;		//data 512 byte.
		SCSIcondition.UsbBuffAddr = UsbBufDAT512;		//data buffer address.
        USBcondition.SOFCountEx = USBcondition.SOFCount;        //for wait new SOF
		SCSIcondition.SCSIcommand = eSCSI_status;
		eSCSI_status = eSCSI_CBW;		//go SCSI command
		break;
	case  	eSCSI_readEND :
        DEBUG_SCSI2PRINTF("SOF END:%u=>%d \n", USBcondition.SOFCount,(USBcondition.SOFCount - USBcondition.SOFCountSt));
        #ifdef _V_DEBUG_SCSI2
            DEBUG_SCSI1PUTS("SCSI Read END\n");
			for (ptr=(char*)UsbBufDAT512, ofs = 0; ofs < 512; ptr += 16, ofs += 16)
            {
				put_dump((BYTE*)ptr, ofs, 16, DW_CHAR);
            }
        #endif
		eSCSI_status = eSCSI_waitCommand;
		break;

    case  	eSCSI_write :
        DEBUG_SCSI1PUTS("SCSI Write command\n");
        DEBUG_SCSI1PRINTF("SOF START:%u\n", USBcondition.SOFCount);
        #ifdef _V_DEBUG_SCSI2
            USBcondition.SOFCountSt = USBcondition.SOFCount;
        #endif
//		SCSIcondition.SCSIcommand = eSCSI_write;
		memcpy(UsbBufCMD64, SCSIwriteCmd, 31);	//SCSI write command.
		SetSector(SCSIcondition.MscTotal,UsbBufCMD64);		//set final sector number to command.

		SCSIcondition.DataLength = 512;		//data 512 byte.
		SCSIcondition.UsbBuffAddr = UsbBufDAT512;		//data buffer address.
        USBcondition.SOFCountEx = USBcondition.SOFCount;        //for wait new SOF
		SCSIcondition.SCSIcommand = eSCSI_status;
		eSCSI_status = eSCSI_CBW;		//go SCSI command
		break;
	case  	eSCSI_writeEND :
        DEBUG_SCSI2PRINTF("SOF END:%u=>%d \n", USBcondition.SOFCount,(USBcondition.SOFCount - USBcondition.SOFCountSt));
        DEBUG_SCSI1PUTS("SCSI Read END\n");
		eSCSI_status = eSCSI_waitCommand;
		break;

	////////////////////////////////////////////////////////////////////////////
	case  	eSCSI_CBW :
   		eSCSI_status++;		//next step
		break;
	case  	eSCSI_CBW_start :
        DEBUG_SCSI1PUTS("SCSI CBW\n");
		BDT_OUT.ADR = (UINT16)UsbBufCMD64;	// the command buffer address to BDT_OUT buffer address.
        BDT_OUT.UOWN_COUNT = 0x801f;	// Setup BD to send 31 byte. It must be appropriate data size. 
		USB_DATAOUT();
		eSCSI_status++;		//next step
		break;
	case  	eSCSI_CBWwait :
        SCSI_checkTransactionReturn();
		break;

	case  	eSCSI_getputBranch :
        if(SCSIcondition.SCSIcommand == eSCSI_write)
        {
            eSCSI_status = eSCSI_putData;
        }else
        {
            eSCSI_status = eSCSI_getData;
        }
		break;
		

	case  	eSCSI_getData :
        DEBUG_SCSI1PUTS("SCSI getDATA Start\n");
//		memset(UsbBufDAT512, 0, 64);		// clear the data buffer
//		BDT_IN.ADR = (UINT16)UsbBufDAT512;	// the data buffer address to BDT_IN buffer address.
		BDT_IN.ADR = (UINT16)SCSIcondition.UsbBuffAddr;	// the data buffer address to BDT_IN buffer address.
		USB_DATAIN();
		eSCSI_status++;		//next step
		break;
	case  	eSCSI_getDataWait :
        SCSI_checkTransactionReturn();
		break;
	case  	eSCSI_getDataNext :
        DEBUG_SCSI1PRINTF("SCSI getDATA DataLength:%d, BufAddr:0x%04x \n",SCSIcondition.DataLength ,SCSIcondition.UsbBuffAddr);
		SCSIcondition.DataLength = SCSIcondition.DataLength - 64;
		if (SCSIcondition.DataLength > 0)
		{
			SCSIcondition.UsbBuffAddr = SCSIcondition.UsbBuffAddr + 64;
			eSCSI_status = eSCSI_getData;	//next data..
		}else
		{
            DEBUG_SCSI1PUTS("SCSI getDATA End\n");
			eSCSI_status = eSCSI_CSW;		//exit data getting.
		}
		break;

		
    case  	eSCSI_putData :
        DEBUG_SCSI1PUTS("SCSI putDATA Start\n");
//		memset(UsbBufDAT512, 0, 64);		// clear the data buffer
//		BDT_IN.ADR = (UINT16)UsbBufDAT512;	// the data buffer address to BDT_IN buffer address.
		BDT_OUT.ADR = (UINT16)SCSIcondition.UsbBuffAddr;	// the data buffer address to BDT_IN buffer address.
        BDT_OUT.UOWN_COUNT = 0x8040;	// Setup BD to send 64 byte. It must be appropriate data size. 
		USB_DATAOUT();
		eSCSI_status++;		//next step
		break;
	case  	eSCSI_putDataWait :
        SCSI_checkTransactionReturn();
		break;
	case  	eSCSI_putDataNext :
        DEBUG_SCSI1PRINTF("SCSI putDATA. DataLength:%d, BufAddr:0x%04x \n",SCSIcondition.DataLength ,SCSIcondition.UsbBuffAddr);
		SCSIcondition.DataLength = SCSIcondition.DataLength - 64;
		if (SCSIcondition.DataLength > 0)
		{
			SCSIcondition.UsbBuffAddr = SCSIcondition.UsbBuffAddr + 64;
			eSCSI_status = eSCSI_putData;	//next data..
		}else
		{
            DEBUG_SCSI1PUTS("SCSI putDATA End\n");
			eSCSI_status = eSCSI_CSW;		//exit data putting.
		}
		break;
        

	case  	eSCSI_CSW :
   		eSCSI_status++;		//next step
		break;
	case  	eSCSI_CSW_start :
        DEBUG_SCSI1PUTS("SCSI CSW\n");
		memset(UsbBufCMD64, 0, 64);		// clear the command buffer
		BDT_IN.ADR = (UINT16)UsbBufCMD64;	// the command buffer address to BDT_IN buffer address.
		USB_DATAIN();
		eSCSI_status++;		//next step
		break;
	case  	eSCSI_CSWwait :
        SCSI_checkTransactionReturn();
		break;
		
	case	eSCSI_parseCSW:
        if (SCSI_isErrInCSW(USBcondition.BDbyteCount, UsbBufCMD64))	//if error..
        {
            xprintf("parseCSW:%d", SCSIcondition.ErrorCode);
            eSCSI_status = eSCSI_END;
		}else
        {
            eSCSI_status = ++SCSIcondition.SCSIcommand ;	//Nomal return to command next..
        }
		break;

	////////////////////////////////////////////////////////////////////////////
	case	eSCSI_END:
		break;

	default:
		eSCSI_status = eSCSI_start;
	}

}

