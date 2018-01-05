#pragma once

#define STRSIZE 100
#define AUDIOLOOPDELAY 1
#define AUDIOTIMEOUT 30

#include "nodes.h"

/******************* function prototypes **********************/

// save audio file and send to receiver
int save_and_send(short* iBigBuf, long lBigBufSize, bool compression, short pri_value);

// play received audio file
int play_audio_file(a_link audio_message);

void StartListeningMode(void);

// get priority value for message
short SetPriority(void);