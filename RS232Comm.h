#pragma once

// Prototype the functions to be used

void initPort(); 
void purgePort();
void outputToPort(LPCVOID buf, DWORD szBuf);
int inputFromPort(LPVOID buf, DWORD szBuf);
void sendMessToPort(unsigned char *msg_text, bool compression);
void endCOM(void);

// Sub functions
void createPortFile();
static int SetComParms();