/*******************************************************************************
 * SCSI COMAND CONTROL
 *		
 * author  :hiroshi murakami
 * created :20190728
 * modified:-
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#ifndef _vSCSI_H
#define _vSCSI_H

#include "integer.h"

/*****************************
 * STATE
 *****************************/
enum eSCSI_STATE {
	eSCSI_start,
	eSCSI_waitCommand,

	eSCSI_requestSense,
	eSCSI_requestSenseEND,
	eSCSI_readCapacity,
	eSCSI_readCapacityEND,
	eSCSI_read,
	eSCSI_readEND,
	eSCSI_write,
	eSCSI_writeEND,

	eSCSI_CBW,
	eSCSI_CBW_start,
	eSCSI_CBWwait,
    eSCSI_getputBranch,
    
	eSCSI_getData,
	eSCSI_getDataWait,
	eSCSI_getDataNext,
	
	eSCSI_putData,
	eSCSI_putDataWait,
	eSCSI_putDataNext,
	
    eSCSI_CSW,
	eSCSI_CSW_start,
	eSCSI_CSWwait,
    eSCSI_parseCSW,
	
	eSCSI_END
};

enum eSCSI_ERR_STATE {
	eSCSI_NO_ERR = 0,
	eSCSI_ERR_dataSize, //data size error
    eSCSI_ERR_signature,//signature error
    eSCSI_ERR_some      //some error (01 cmd err 02 phase error)
};

/*****************************
 * DEFINES
 *****************************/
#define SCSI_REQUEST_SENSE			(eSCSI_status = eSCSI_requestSense)
#define SCSI_READ_CAPACITY			(eSCSI_status = eSCSI_readCapacity)
#define SCSI_READ					(eSCSI_status = eSCSI_read)
#define SCSI_WRITE					(eSCSI_status = eSCSI_write)

typedef struct __SCSI_CONDITION
{
	enum eSCSI_STATE		SCSIcommand;
	enum eSCSI_ERR_STATE    ErrorCode;
// 	UINT16					TransmissionByte;
   UINT32					MscTotal;		// SCSI final sector number
   INT16					DataLength;		// transmission data length
   UINT8*					UsbBuffAddr;	// transmission data length
} SCSI_CONDITION;

/*****************************
 * VARIABLES
 *****************************/
extern SCSI_CONDITION SCSIcondition;
extern enum eSCSI_STATE eSCSI_status;


/*****************************
 * PROTOTYPES
 *****************************/

void SCSI_init(void);
void SCSI_statusControl(void);


#endif
