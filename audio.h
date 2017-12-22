#pragma once

#define STRSIZE 100
#define AUDIOLOOPDELAY 1
#define AUDIOTIMEOUT 30

#include "nodes.h"

/******************* function prototypes **********************/

// save audio file and send to receiver
int save_and_send(short* iBigBuf, long lBigBufSize, bool compression);

// play received audio file
int play_audio_file(a_link audio_message);

void StartListeningMode(int* unlistenedAudio, int* totalAudio, bool compressed);