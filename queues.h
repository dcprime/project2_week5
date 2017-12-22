#pragma once
/* queues.h: Header file for queue functions and definitions */

#include "nodes.h"  // Header file includes Prototypes defined in nodes.cpp

#define TRUE 1
#define FALSE 0
#define MSGSIZE 141
#define AUDIOSIZE 200000
#define LOOPDELAY 1      // number of times to loop in listening mode before printing '.'
#define TIMEOUT 30      // number of times to loop in listening mode before timeout

/******************** function prototypes ***********************/

// initialize the queue
void InitQueue(void);

// check if queue is empty
int  IsQueueEmpty(void);

// add Node to queue
void  AddToQueue(Node *);

// remove 1st Node from queue and return pointer to it
Node *DeQueue(void);

// add a message to a node and add the node to the queue
void AddMessToQueue(char* msg_text);

// print all messages in queue from oldest to newest
void PrintMessages(void);

// begin loop to receive messages from transmitting terminal
void StartWaitingMode(int* unreadMessages, int* totalMessages);

// initialize the Audio queue
void InitAudioQueue(void);

// check if queue Audio is empty
int  IsAudioQueueEmpty(void);

// add Audio Node to queue
void  AddToAudioQueue(A_Node *);

// remove 1st Audio Node from queue and return pointer to it
A_Node *DeQueueAudio(void);

// add a message to an Audio node and add the Audio node to the queue
void AddMessToAudioQueue(short* audio_file);

void PlayMessages(void);