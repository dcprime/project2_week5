/*******************project2_week4_main.cpp ************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include "queues.h"  // Header file includes Prototypes defined in queues.cpp
#include "nodes.h"  // Header file includes Prototypes defined in nodes.cpp
#include "sound.h" // Header file includes Prototypes defined in sound.cpp
#include "audio.h" // Header file includes Prototypes defined in audio.cpp
#include "RS232Comm.h" // Header file includes Prototypes defined in RS232Comm.cpp

#define STRSIZE 100


void main(int argc, char *argv[]) {

    extern short iBigBuf[];									// buffer
    extern long  lBigBufSize;								// total number of samples

    int run = TRUE;
    int unreadText = 0;
    int totalText = 0;
	int unlistenedAudio = 0;
	int totalAudio = 0;
	short priority = 0;
    char choice[STRSIZE];
    unsigned char msg_text[MSGSIZE];
	bool compression = true;

    // initialize the message queue
    InitQueue();
	InitAudioQueue();
	initPort();

    while (run == TRUE) {

		// update read and unread text counts
		totalText = get_count(text);
		unreadText = get_unaccessed(text);

		// update listened and unlistened audio counts
		totalAudio = get_count(audio);
		unlistenedAudio = get_unaccessed(audio);

        // menu
        printf("\nChoose an option:\n");
		printf("[L]isten for Incoming Messages\n");
        printf("[R]ecord and Send Audio Message\n");
        printf("[P]lay Audio Message ");
		printf("- %d audio messages in queue (%d unheard)\n", totalAudio, unlistenedAudio);
		printf("[T]oggle compression on/off - compression currently ");
		if (compression) {
			printf("ON\n");
		}
		else
			printf("OFF\n");
        printf("[S]end Text Message\n");
        printf("[C]heck Text Messages ");
        printf("- %d messages in queue (%d unread)\n", totalText, unreadText);
        printf("[Q]uit\n");
        printf("\nEnter L, R, P, T, S, C, or Q: ");

        fgets(choice, STRSIZE, stdin);

        switch (tolower(choice[0])) {
		case 'l':
			printf("\nListening Mode active\n");
			StartListeningMode();
			break;
        case 'r':
            InitializeRecording();
            RecordBuffer(iBigBuf, lBigBufSize);
            CloseRecording();
			priority = SetPriority();	// set message priority
            save_and_send(iBigBuf, lBigBufSize, compression, priority);
            break;
        case 'p':
			if (IsAudioQueueEmpty()) {
				printf("\n--- Audio queue is empty ---\n");
				break;
			}
			else {
				PlayMessages();
			}
            break;
		case 't':
			compression = !compression;
			break;
        case 's':
            // get message input from user
            printf("\nEnter message text (up to %d characters) and press ENTER\n", MSGSIZE);
            printf("-> ");
            fgets((char*)msg_text, MSGSIZE, stdin);
			priority = SetPriority();	// set message priority

            // send message to com port
            sendMessToPort(msg_text, compression, priority);
            break;
        case 'c':
            if (IsQueueEmpty()) {
                printf("\n--- Message queue is empty ---\n");
                break;
            }
            else {
                // print all messages in queue from oldest to newest
                PrintMessages();
				unreadText = 0;
                break;
            }
        case 'q':
            run = FALSE;
			endCOM();
            break;
        default:
            printf("\nError: That option is not valid\n\n");
            break;
        }
		// system("CLS");
    } // end while main menu

}