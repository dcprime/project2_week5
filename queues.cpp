/******************* queues.cpp **********************
*                                                    *
*  Functions adapted from ESE lecture slides         *
*  on creating queue functions                       *
*                                                    *
*****************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <Windows.h>
#include "nodes.h"
#include "queues.h"
#include "RS232Comm.h"
#include "sound.h"



static Node *pNode, *pHead, *pTail;				// text message queue pointers
static A_Node *a_pNode, *a_pHead, *a_pTail;		// audio message queue pointers


//************************* text message functions *************************// 

void InitQueue(void)
{
    pHead = pTail = NULL;
}

// Returns non-zero if queue is empty. 
int  IsQueueEmpty(void)
{
    return (pHead == NULL);
}

void  AddToQueue(Node *pn)
{
    if (pHead == NULL) {
        pHead = pn; //make first item ‘head’
    }
    else {
        pTail->pNext = pn;// make the last 
    }           //input point to the new item 
    pn->pNext = NULL;
    pTail = pn; // make this last input
} // item the ‘tail’   

// Return item at the head
Node *DeQueue(void)
{
    Node *pTemp;
    if (pHead == NULL) return(NULL);
    pTemp = pHead;
    pHead = pHead->pNext; // make next item new ‘head’
    return(pTemp);	  // return old ‘head’ 	
}

void AddMessToQueue(char* msg_text) {
    // create node to hold message content
    if (IsQueueEmpty()) {
        pNode = (link)malloc(sizeof(Node)); 	          // Make first Node
        pHead = pNode;				                          // save its location
        pTail = pNode;
    }
    else {
        pNode->pNext = (link)malloc(sizeof(Node));      // Make Node i
        pNode = pNode->pNext;			                      // Get pointer to Node i
    }

    // add message content to node and add node to queue
    memcpy(pNode->Data.text, msg_text, strlen(msg_text) + 1);
    AddToQueue(pNode);
    printf("\n--- Message added to queue ---\n");
}

void PrintMessages(void) {
    printf("\n--- Messages in queue from oldest to newest ---\n\n");
    traverse(pHead, visit);
    printf("\n--- End of messages ---\n");
}

/************************ waiting mode (text) ************************/
void StartWaitingMode(int* unreadMessages, int* totalMessages) {
    int run = TRUE;
    char msgIn[MSGSIZE];        // buffer to hold incoming message
    int success = 0;
    short dot_counter = 0;
    unsigned long timeout = 0;
    
    while (run == TRUE) {
        success = inputFromPort(msgIn, MSGSIZE);	// Receive string from port
        if (success == 1) {
            // add message to node and node to queue
            AddMessToQueue(msgIn);

            // increment number of unread messages
            (*unreadMessages)++;
			(*totalMessages)++;

            // update the listening status 
            printf("\n%d unread messages in queue\n\n", *unreadMessages);

            // reset success status
            success = 0;
        }
        else if (success == -1) {
            run = FALSE;
        }

        // print a dot every LOOPDELAY times through the loop to show Listening Mode is active
        dot_counter++;
        if (dot_counter == LOOPDELAY) {
            printf(".");
            dot_counter = 0;
        }

        // listening mode time out
        timeout++;
        if (timeout == TIMEOUT) {
            printf("\n\n------------- Waiting Mode timed out -------------\n");
            run = FALSE;
        }

    } // end while loop
}

//************************* audio message functions *************************//

void InitAudioQueue(void)
{
	a_pHead = a_pTail = NULL;
}

// Returns non-zero if queue is empty. 
int  IsAudioQueueEmpty(void)
{
	return (a_pHead == NULL);
}

void  AddToAudioQueue(A_Node *pn)
{
	if (a_pHead == NULL) {
		a_pHead = pn; //make first item ‘head’
	}
	else {
		a_pTail->pNext = pn;// make the last 
	}           //input point to the new item 
	pn->pNext = NULL;
	a_pTail = pn; // make this last input
} // item the ‘tail’   

  // Return item at the head
A_Node *DeQueueAudio(void)
{
	A_Node *pTemp;
	if (a_pHead == NULL) return(NULL);
	pTemp = a_pHead;
	a_pHead = a_pHead->pNext; // make next item new ‘head’
	return(pTemp);	  // return old ‘head’ 	
}

void AddMessToAudioQueue(short* audio_file) {
	// create node to hold audio message content
	if (IsAudioQueueEmpty()) {
		a_pNode = (a_link)malloc(sizeof(A_Node)); 	        // Make first Node
		a_pHead = a_pNode;				                    // save its location
		a_pTail = a_pNode;
		a_pNode->Data.sequence = 1;							// set sequence number of first node
	}
	else {
		a_pNode->pNext = (a_link)malloc(sizeof(A_Node));			// Make Node i
		a_pNode->pNext->Data.sequence = a_pNode->Data.sequence + 1;	// Set sequence number of new node
		a_pNode = a_pNode->pNext;									// Get pointer to Node i
	}

	// add audio message content to node and add node to queue
	memcpy(a_pNode->Data.recording, audio_file, SAMPLES_SEC * RECORD_TIME * sizeof(short));
	AddToAudioQueue(a_pNode);
	printf("\n--- Audio Message added to queue ---\n");
}

void PlayMessages(void) {
	printf("\n--- Audio message in queue from oldest to newest ---\n\n");
	a_traverse(a_pHead, a_visit);
	printf("\n--- End of audio messages ---\n");
}