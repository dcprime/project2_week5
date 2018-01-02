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

void AddMessToQueue(Message text_message) {
    // create node to hold message content
    if (IsQueueEmpty()) {
        pNode = (link)malloc(sizeof(Node)); 	          // Make first Node
        pHead = pNode;				                          // save its location
        pTail = pNode;
    }
    else {
        pNode->pNext = (link)malloc(sizeof(Node));      // Make Node i
        pNode = pNode->pNext;			                // Get pointer to Node i
    }

	// add message content to node and add node to queue
	memcpy(&(pNode->Data), &text_message, sizeof(Message));
    AddToQueue(pNode);
    printf("\n--- Message added to queue ---\n");
}

void PrintMessages(void) {
    printf("\n--- Messages in queue from oldest to newest ---\n\n");
    traverse(pHead, visit);
    printf("\n--- End of messages ---\n");
}

int node_count(link h) {
	if (h == NULL) return (0);
	return(node_count(h->pNext) + 1);
}

int unread_count(link h) {
	if (h == NULL) return (0);
	if (h->Data.accessed == 0) {
		return(unread_count(h->pNext) + 1);
	}
	else {
		return(unread_count(h->pNext) + 0);
	}
	
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

void AddMessToAudioQueue(Message audio_message) {
	// create node to hold audio message content
	if (IsAudioQueueEmpty()) {
		a_pNode = (a_link)malloc(sizeof(A_Node)); 	        // Make first Node
		a_pHead = a_pNode;				                    // save its location
		a_pTail = a_pNode;
	}
	else {
		a_pNode->pNext = (a_link)malloc(sizeof(A_Node));			// Make Node i
		a_pNode = a_pNode->pNext;									// Get pointer to Node i
	}

	// add audio message content to node and add node to queue
	memcpy(&(a_pNode->Data), &audio_message, sizeof(Message));
	AddToAudioQueue(a_pNode);
	printf("\n--- Audio Message added to queue ---\n");
}

void PlayMessages(void) {
	printf("\n--- Audio message in queue from oldest to newest ---\n\n");
	a_traverse(a_pHead, a_visit);
	printf("\n--- End of audio messages ---\n");
}

int a_node_count(a_link h) {
	if (h == NULL) return (0);
	return(a_node_count(h->pNext) + 1);
}

int unlistened_count(a_link h) {
	if (h == NULL) return (0);
	if (h->Data.accessed == 0) {
		return(unlistened_count(h->pNext) + 1);
	}
	else {
		return(unlistened_count(h->pNext) + 0);
	}

}

// ************************** public count functions ************************** //

int get_count(audORtext choice) {
	if (choice == audio) {
		return (a_node_count(a_pHead));
	}
	else {
		return (node_count(pHead));
	}
}

int get_unaccessed(audORtext choice) {
	if (choice == audio) {
		return (unlistened_count(a_pHead));
	}
	else {
		return (unread_count(pHead));
	}
}