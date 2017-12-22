#include <Windows.h>								// Includes the functions for serial communication via RS232
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RS232Comm.h"

#define EX_FATAL 1
#define BUFSIZE 140									// Size of buffer
#define COMPORT "\\\\.\\COM10"								// COM port used for transmit / receive

extern long lBigBufSize;		// total number of samples in buffer
extern int audio_comp_in_size;	// bytes read on receiving

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
		audio_comp_in_size = NumberofBytesRead;
		return 1;	// success
	}
	else {
		return 0;	// error - no bytes received
	}
}

void sendMessToPort(char *msg_text) {
    outputToPort(msg_text, strlen(msg_text) + 1);
    Sleep(1000); // play with this number to see how short (or eliminate?)
    purgePort();
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

