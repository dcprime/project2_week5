/******************* nodes.cpp ***********************
*                                                    *
*  traverse and traverseR functions adapted from     *
*  W6Algorithms_RecursiveMethods.pptx slides on      *
*  Recursive Methods                                 *
*                                                    *
*****************************************************/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "nodes.h"
#include "audio.h"
#include "queues.h"

// ********* text message functions ********* //

// Execute a function on each node in order
void traverse(link h, void(*visit)(link)) {
    if (h == NULL) return;
    (*visit)(h);  // calls ‘visit’ before recursive call
    traverse(h->pNext, visit);
}

// Execute a function on each node in reverse
void traverseR(link h, void(*visit)(link)) {
    if (h == NULL) return;
    traverseR(h->pNext, visit);
    (*visit)(h);
}

void traverseP(link h, void(*visit)(link)) {

}

// Print the message content of a node
void visit(link print_node) {

	const short time_string_len = 26;
	char time_as_string[time_string_len];
	ctime_s(time_as_string, time_string_len, &(print_node->Data.timestamp));

	printf("\nText received on %s", time_as_string);
	printf(" - Priority %d\n", print_node->Data.priority);

	if (print_node->Data.compressed) {
		
		// create buffer to hold text data and uncompress it
		unsigned char print_buff[MSGSIZE];
		Huffman_Uncompress(print_node->Data.text, print_buff, print_node->Data.data_size, MSGSIZE);

		// print text message
		printf("%s\n", print_buff);
	}
	else {
		printf("%s\n", print_node->Data.text);
	}
	print_node->Data.accessed = 1;
}

// ********* audio message functions ********* //

// Execute a function on each audio node in order
void a_traverse(a_link h, void(*a_visit)(a_link)) {
	if (h == NULL) return;
	(*a_visit)(h);  // calls ‘visit’ before recursive call
	a_traverse(h->pNext, a_visit);
}

// Execute a function on each node in reverse
void a_traverseR(a_link h, void(*visit)(a_link)) {
	if (h == NULL) return;
	a_traverseR(h->pNext, a_visit);
	(*visit)(h);
}

// Play the audio message in the node
void a_visit(a_link audio_node) {
	printf("\nExtracting audio message...\n");
	play_audio_file(audio_node);
	audio_node->Data.accessed = 1;
}