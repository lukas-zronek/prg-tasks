/*
 * @file send.c
 * @author Lukas Zronek
 *
 */

#include <stdlib.h>
#include "lib.h"

#define MSG_TYPE_IN 1
#define MSG_TYPE_OUT 2
#define MSG_ID_FILE "./send.c"

#define START "I have thought of a random number in between 0 and 100? What's your guess?\n"
#define CORRECT "BINGO\n"
#define WRONG1 "Wrong guess, you have one more tries.\n"
#define WRONG2 "Wrong guess, you have two more tries.\n"
#define WRONG3 "Wrong guess, you have three more tries.\n"
#define LOST "You lost.\n"

int main(int argc, char **argv)
{
	long int msq = 0;
	int n = 0;
	int r = 0;

	msq = create_queue(MSG_ID_FILE);

	if (msq < 0) {
		return EXIT_FAILURE;
	}

	if (argc > 1) {
		if (strcmp(argv[1], "cleanup") == 0) {
			if (delete_queue(msq) == 0) {
				return EXIT_SUCCESS;
			}
		}
		return EXIT_FAILURE;
	}

	r = get_rand(100);

	printf("r: %d\n", r);

	for (int i = 0; i <= 4; i++) {
		if (i == 0) {
			if (send_message(msq, MSG_TYPE_OUT, START) != 0) {
				return EXIT_FAILURE;
			}
		} else if (n == r) {
			if (send_message(msq, MSG_TYPE_OUT, CORRECT) != 0) {
				return EXIT_FAILURE;
			}
			break;
		} else {
			if (i == 1) {
				if (send_message(msq, MSG_TYPE_OUT, WRONG3) != 0) {
					return EXIT_FAILURE;
				}
			} else if (i == 2) {
				if (send_message(msq, MSG_TYPE_OUT, WRONG2) != 0) {
					return EXIT_FAILURE;
				}
			} else if (i == 3) {
				if (send_message(msq, MSG_TYPE_OUT, WRONG1) != 0) {
					return EXIT_FAILURE;
				}
			} else {
				if (send_message(msq, MSG_TYPE_OUT, LOST) != 0) {
					return EXIT_FAILURE;
				}
			}
		}

		if (i == 4) {
			break;
		}

		if (receive_int(msq, MSG_TYPE_IN, &n) != 0) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
