#include <Windows.h>								// Includes the functions for serial communication via RS232
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RS232Comm.h"
#include "queues.h"
#include "huffman.h"

#define EX_FATAL 1
#define BUFSIZE 140									// Size of buffer
#define COMPORT "\\\\.\\COM10"								// COM port used for transmit / receive

extern long lBigBufSize;		// total number of samples in buffer
extern int bytes_received;		// bytes read on receiving

// Ensure that default character set is not Unicode
// Communication variables and parameters
HANDLE hCom;										// Pointer to a COM port
int nComRate = 57600;								// Baud (Bit) rate in bits/second 
int nComBits = 8;									// Number of bits per frame
COMMTIMEOUTS timeout;								// A commtimout struct variable

// Initializes the port and sets the communication parameters
void initPort() {
	createPortFile();								// Initializes hCom to point to PORT5 (port 5 is used by USB-Serial adapter on my laptop)
	purgePort();									// Purges the COM port
	SetComParms();									// Uses the DCB structure to set up the COM port
	purgePort(); 
}

// Purge any outstanding requests on the serial port (initialize)
void purgePort() {
	PurgeComm(hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

// Output message to port
void outputToPort(LPCVOID buf, DWORD szBuf) {
	int i=0;
	DWORD NumberofBytesTransmitted;
	LPDWORD lpErrors=0;
	LPCOMSTAT lpStat=0; 

	i = WriteFile(
		hCom,										// Write handle pointing to COM port
		buf,										// Buffer size
		szBuf,										// Size of buffer
		&NumberofBytesTransmitted,					// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		printf("\nSuccessful transmission -  %ld bytes transmitted\n", NumberofBytesTransmitted);
}

int inputFromPort(LPVOID buf, DWORD szBuf) {
	int i = 0;
	DWORD NumberofBytesRead;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;
	i = ReadFile(
		hCom,										// Read handle pointing to COM port
		buf,										// Buffer size
		szBuf,  									// Size of buffer - Maximum number of bytes to read
		&NumberofBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
		return -1; // error - comm timeout
	}
	else if (NumberofBytesRead != 0) {
		printf("\n\nSuccessful reception! There were %ld bytes read\n", NumberofBytesRead);
		bytes_received = NumberofBytesRead;
		return 1;	// success
	}
	else {
		return 0;	// error - no bytes received
	}
}

void sendMessToPort(unsigned char *msg_text, bool compression) {

	Message text_message_out;
	int text_comp_out_size;

	text_message_out.data_size = MSGSIZE;
	text_message_out.compressed = false;
	text_message_out.message_type = text;
	text_message_out.accessed = 0;

	if (compression) {
		unsigned char compressed_text[MSGSIZE + HUFFEXTRA];
		text_comp_out_size = Huffman_Compress(msg_text, compressed_text, MSGSIZE);

		if (text_comp_out_size < MSGSIZE) {
			text_message_out.data_size = text_comp_out_size;
			text_message_out.compressed = true;
			memcpy(text_message_out.text, compressed_text, text_comp_out_size);
		}
		else {
			memcpy(text_message_out.text, msg_text, MSGSIZE);
		}
	}
	else {
		memcpy(text_message_out.text, msg_text, MSGSIZE);
	}

	// convert output Message to char array
	char *message_as_char = (char*)calloc(sizeof(Message), sizeof(char));
	memcpy(message_as_char, &text_message_out, sizeof(Message));

	// send output Message to Port
	printf("\nSending text message to receiver\n");
	outputToPort(message_as_char, sizeof(Message));
    Sleep(1000); // play with this number to see how short (or eliminate?)
    purgePort();
	free(message_as_char);
}

void endCOM(void) {
	CloseHandle(hCom);
}


// Sub functions called by above functions
/**************************************************************************************/

// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
void createPortFile() {
	// Call the CreateFile() function 
	hCom = CreateFile(
		COMPORT,									// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE,				// Open for read and write
		NULL,										// No sharing allowed
		NULL,										// No security
		OPEN_EXISTING,								// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,						// Do not set any file attributes --> Use synchronous operation
		NULL										// No template
	);

	if (hCom == INVALID_HANDLE_VALUE) {
		printf("\nFatal Error 0x%x: Unable to open\n", GetLastError());
	}
	/*else {
		printf("\nCOM is now open\n");
	}*/
}

static int SetComParms() {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(hCom, &dcb))
		return(0);

	// Set our own parameters from Globals
	dcb.BaudRate = nComRate;						// Baud (bit) rate
	dcb.ByteSize = (BYTE)nComBits;					// Number of bits(8)
	dcb.Parity = 0;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(hCom, &dcb))
		return(0);

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	memset((void *)&timeout, 0, sizeof(timeout));
	timeout.ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout.ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout.ReadTotalTimeoutConstant = 1000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(hCom, &timeout);
	return(1);
}

