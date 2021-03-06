/*	Filename: audio.cpp
Author: Michael Galle, Darren Conley
Date: Nov 2017
Details: Testing mainline for Windows sound API
Modifications:
Nov 24, 2017 - created cpp and header files to run audio operations from previously created main.cpp
*/

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "sound.h"
#include "audio.h"
#include "RS232Comm.h"
#include "huffman.h"
#include "nodes.h"
#include "queues.h"


char replay;
char c;                                                 // used to flush extra input
char filename[STRSIZE] = "recordings\\recording.dat";   // file to save/read audio
FILE* f;

extern short iBigBufIn[];	// receiving buffer
extern long lBigBufSize;	// total number of samples in buffer
unsigned char audio_out[audio_as_char];

unsigned char audio_compressed[huff_compressed_size];
int audio_comp_out_size;
int bytes_received;

int save_and_send(short* iBigBuf, long lBigBufSize, bool compression, short pri_value) {
	char send;
	printf("\nWould you like to send your message? (y/n): ");
	scanf_s("%c", &send, 1);
	while ((c = getchar()) != '\n' && c != EOF) {}		// Flush other input
	if ((send == 'y') || (send == 'Y')) {
		
		// convert audio from short array to char array
		memcpy(audio_out, iBigBuf, audio_as_char);

		if (compression) {
			printf("\nCompressing audio message...\n");
			// save return value from Huffman_Compress
			audio_comp_out_size = Huffman_Compress(audio_out, audio_compressed, audio_as_char);
		}

		// Message structure being sent to receiver
		Message message_out;
		message_out.message_type = audio;
		message_out.accessed = 0;
		message_out.vote[0] = false;
		message_out.vote[1] = false;
		message_out.priority = pri_value;

		printf("\nSending audio recording to receiver...\n");
		if (compression) {

			// set output Message fields
			message_out.compressed = true;
			message_out.vote[0] = true;		// values for error checking
			message_out.vote[1] = true;		// values for error checking
			message_out.data_size = audio_comp_out_size;
			memcpy(message_out.recording, audio_compressed, audio_comp_out_size);

			// set output data size
			int output_size = sizeof(Message) - audio_as_char + audio_comp_out_size;

			// convert output Message to char array
			char *message_as_char = (char*)calloc(output_size, sizeof(char));
			memcpy(message_as_char, &message_out, output_size);

			// send output Message to Port
			outputToPort(message_as_char, output_size);
			free(message_as_char);
		}
		else {
			// set output Message fields
			message_out.compressed = false;
			message_out.data_size = audio_as_char;
			memcpy(message_out.recording, audio_out, audio_as_char);

			// convert output Message to char array
			char *message_as_char = (char*)calloc(sizeof(Message), sizeof(char));
			memcpy(message_as_char, &message_out, sizeof(Message));

			// send output Message to Port
			outputToPort(message_as_char, sizeof(Message));
			free(message_as_char);
		}
		Sleep(1000); // play with this number to see how short (or eliminate?)
		purgePort();
		return 1;
	}
}



int play_audio_file(a_link audio_message) {

	InitializePlayback();

	// array of shorts to hold audio data
	short converted_audio[audio_as_char/(sizeof(short))];

	// extract audio as char array from received structure in Node
	if (audio_message->Data.compressed == true) {
		Huffman_Uncompress(audio_message->Data.recording, (unsigned char*)converted_audio, audio_message->Data.data_size, audio_as_char);
	}
	else {
		memcpy(converted_audio, audio_message->Data.recording, audio_message->Data.data_size);
	}

	const short time_string_len = 26;
	char time_as_string[time_string_len];
	ctime_s(time_as_string, time_string_len, &(audio_message->Data.timestamp));

	printf("\nAudio received on %s\n", time_as_string);
	printf(" - Priority %d\n", audio_message->Data.priority);
	PlayBuffer(converted_audio, lBigBufSize);
	ClosePlayback();
	return 1;
}

void StartListeningMode(void) {

	int run = TRUE;
	int success = 0;
	short dot_counter = 0;
	unsigned long timeout = 0;
	unsigned char messInAsChar[sizeof(Message)];
	Message messInAsMessage;

	while (run == TRUE) {
		
		// receive Message struct from port
		success = inputFromPort(messInAsChar, sizeof(Message));
		
		if (success == 1) {
			
			// convert messageIn to type Message
			memcpy(&messInAsMessage, messInAsChar, bytes_received);

			// add timestamp to message
			messInAsMessage.timestamp = time(NULL);

			// verify compression setting is accurately transmitted
			short verify = messInAsMessage.compressed + messInAsMessage.vote[0] + messInAsMessage.vote[1];
			if (verify > 1) {
				messInAsMessage.compressed = true;
			}
			else {
				messInAsMessage.compressed = false;
			}
			
			// parse message type and add to correct queue
			if (messInAsMessage.message_type == audio) {
				
				AddMessToAudioQueue(messInAsMessage);

			}
			else if (messInAsMessage.message_type == text) {

				AddMessToQueue(messInAsMessage);

			}

			// reset success status
			success = 0;
			run = FALSE;
		}
		else if (success == -1) {
			run = FALSE;
		}

		// listening mode time out
		timeout++;
		if (timeout == AUDIOTIMEOUT) {
			printf("\n\n------------- Waiting Mode timed out -------------\n");
			run = FALSE;
		}

	} // end while loop
}

short SetPriority(void) {

	char input;
	short value;
	bool run_loop = 1;
	const int ascii_offset = 48;

	while (run_loop) {
		printf("\nWhat is the message priority? [1-5]: ");

		scanf_s("%c", &input, 1);
		while ((c = getchar()) != '\n' && c != EOF) {}		// Flush other input

		// check if number is between 1 and 5 in ascii
		if ((input > ascii_offset) && (input < (ascii_offset + 6))) {
			value = input - ascii_offset;
			printf("\nPriority set at %d\n", value);
			return(value);
		}
		else {
			printf("\nPlease enter a value between 1 and 5\n");
		}
	}
}