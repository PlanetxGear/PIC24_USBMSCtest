/*******************************************************************************
 * serial monitor input
 * use     :UART1
 * author  :hiroshi murakami
 * created :20161225
 * modified:-
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vUART_CMND_C

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <p24FJ64GB002.h>
#include "integer.h"
#include "xprintf.h"
#include "vFIFO.h"
#include "vUART_CMND.h"

#define CMND_BUFFER_SIZE 64		//UART I/O buffer size
unsigned char cCmdBuf[CMND_BUFFER_SIZE + 1];	//command buffer, last 1 byte is for stored NULL
int iCIdx;		//command buffer insert position.

//xprintf FIFO
FIFO_STRUCT TxFifo;
BYTE TxBuff[64];

//******************************************************************************
//* initialization
//******************************************************************************
void vUART_CMND_init(void)
{
	vFIFO_init (&TxFifo, sizeof(TxBuff), &TxBuff[0]);
	iCIdx = 0;							// command buffer clear 
	xdev_out(vPutc_to_TxFifo);			// join xPutc to xPutc_to_TxFifo
}

//******************************************************************************
//* get command strings, and shift command status
//* 1byte get from RX and insert command buffer
//* echo back to UART
//******************************************************************************
enum eUART_CMND eUART_CMND_Getc(void)
{
	unsigned char ucRxData;

	if(UART1_IsRxReady()) 
	{
		ucRxData = UART1_Read();

		switch(ucRxData)	//check the data.
		{
		case NULL:
			break;

		case '\b':  // BS (Back Space)
			if(iCIdx > 0) 
			{
				iCIdx--;   //delete 1 byte from command buffer.
				//following logic mean clear 1 character on screen.
				xputs("\b \b");     //back to 1 byte and clear space.
			}
			break;

		case '\r':  // CR (unused)
			break;

		case '\n':  // LF (= 'Enter' mean finish command input)
			if(iCIdx < CMND_BUFFER_SIZE) 
			{
				//if there are some space in the command buffer...
				cCmdBuf[iCIdx] = NULL; //add NULL

			} else {
				//there isn't space in the command buffer 
				cCmdBuf[CMND_BUFFER_SIZE] = NULL;	//add NULL
			}
			iCIdx = 0;     //clear the command buffer
			xputs("\n");
			return eUART_CMND_Exec;
			break;

		case '\x7F':    // DEL (input cancel)
			iCIdx = 0 ;    //clear the command buffer
			xputs("\nDelete the Command\n");
			return eUART_CMND_Del;
			break;

		case '\x1B':    // ESC (Escape menu hierarchy)
			iCIdx = 0 ;    //clear the command buffer
			xputs("\nEscape the Command\n");
			return eUART_CMND_Esc;
			break;

		default:    // get command strings 1byte 1.
			if(iCIdx < CMND_BUFFER_SIZE)		//if there are some space in the command buffer..
			{
				cCmdBuf[iCIdx] = ucRxData;		//input 1byte in the command buffer
				xputc (ucRxData);	//echo back 1 byte to screen.
				iCIdx++;           //next position
			} else {
				iCIdx = 0 ;    //clear the command buffer
				xputs("\nCommand Over Flow\n");
				return eUART_CMND_Err;
			}
		}
	}
	return eUART_CMND_Wait;
}

// Put a byte into TxFifo
void vPutc_to_TxFifo(unsigned char d)
{
	while (TxFifo.dataCounter >= TxFifo.buffer_size) 
	{
		vUART_TxPutc();
	}	// Wait while Tx FIFO is full

	eFIFO_putc (&TxFifo, d);
}

// Put a byte from TxFifo to UART
void vUART_TxPutc(void)
{
	//while(!(UART1_StatusGet() & UART1_TX_FULL) && iFIFO_remains(&TxFifo)) 
	while(!(U1STAbits.UTXBF) && iFIFO_remains(&TxFifo)) 
	{
		UART1_Write(bFIFO_getc(&TxFifo));
	}
}
