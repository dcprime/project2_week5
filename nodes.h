#pragma once
/* nodes.h: Header file for node functions and definitions */

#include "sound.h"

/******************* message structure definitions ********************/

// Message structure
typedef struct message {
    char text[141];
    short sender_id;
    short receiver_id;
    char priority;
    short sequence;
    char other[25];
} Message;

// link is a pointer to a Node
typedef struct node* link;

// Node contains a pointer to next node and a Message structure
struct node { link pNext; Message Data; };

// Alias for Node = struct node
typedef struct node Node;

/******************* audio message structure definitions ********************/

// Audio Message structure
typedef struct a_message {
	short recording[SAMPLES_SEC * RECORD_TIME];
	short sender_id;
	short receiver_id;
	char priority;
	short sequence;
	char other[25];
} A_Message;

// link is a pointer to a Node
typedef struct a_node* a_link;

// Node contains a pointer to next node and a Message structure
struct a_node { a_link pNext; A_Message Data; };

// Alias for Node = struct node
typedef struct a_node A_Node;

/******************** function prototypes ***********************/

void traverse(link h, void(*visit)(link));

void traverseR(link h, void(*visit)(link));

void visit(link print_node);

void a_traverse(a_link h, void(*a_visit)(a_link));

void a_traverseR(a_link h, void(*a_visit)(a_link));

void a_visit(a_link audio_node);