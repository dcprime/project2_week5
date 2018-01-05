#pragma once
/* nodes.h: Header file for node functions and definitions */

#include "sound.h"
#include "huffman.h"

enum audORtext {audio, text};
const int audio_as_char = SAMPLES_SEC * RECORD_TIME * (sizeof(short) / sizeof(char));
const int huff_compressed_size = audio_as_char + HUFFEXTRA;

/******************* message structure definitions ********************/

// Message structure
typedef struct message {
	enum audORtext message_type;
	bool compressed;
	short vote[2];
	int data_size;
    short priority;
	bool accessed;
	short sender_id;
	short receiver_id;
	time_t timestamp;	// use ctime(&timestamp) to convert to string
    char other[25];
	unsigned char text[141];
	unsigned char recording[huff_compressed_size];
} Message;

// ------------------- text nodes ------------------- //

// link is a pointer to a Node
typedef struct node* link;

// Node contains a pointer to next node and a Message structure
struct node { link pNext; Message Data; };

// Alias for Node = struct node
typedef struct node Node;

// ------------------- audio nodes ------------------- //

// a_link is a pointer to an a_node
typedef struct a_node* a_link;

// Node contains a pointer to next node and a Message structure
struct a_node { a_link pNext; Message Data; };

// Alias for Node = struct node
typedef struct a_node A_Node;

/******************** function prototypes ***********************/

void traverse(link h, void(*visit)(link));

void traverseR(link h, void(*visit)(link));

void traverseP(link h, void(*visit)(link));

void visit(link print_node);

void a_traverse(a_link h, void(*a_visit)(a_link));

void a_traverseR(a_link h, void(*a_visit)(a_link));

void a_visit(a_link audio_node);