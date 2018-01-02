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
void AddMessToQueue(Message text_message);

// print all messages in queue from oldest to newest
void PrintMessages(void);

// count the number of Nodes in the queue (text)
int node_count(link h);

// count the number of Nodes with unread text messages
int unread_count(link h);

// initialize the Audio queue
void InitAudioQueue(void);

// check if queue Audio is empty
int  IsAudioQueueEmpty(void);

// add Audio Node to queue
void  AddToAudioQueue(A_Node *);

// remove 1st Audio Node from queue and return pointer to it
A_Node *DeQueueAudio(void);

// add a message to an Audio node and add the Audio node to the queue
void AddMessToAudioQueue(Message audio_message);

void PlayMessages(void);

// count the number of Nodes in the queue (text)
int a_node_count(a_link h);

// count the number of Nodes with unlistened audio messages
int unlistened_count(a_link h);

// interface for audio or text node count
int get_count(audORtext choice);

// interface for audio or text unaccessed message count
int get_unaccessed(audORtext choice);