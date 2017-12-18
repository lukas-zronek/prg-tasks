/*
 * @file recv.c
 * @author Lukas Zronek
 *
 */

#include <stdlib.h>
#include <string.h>
#include "lib.h"

#define MSG_TYPE_OUT 1
#define MSG_TYPE_IN 2

#define PROMPT "Enter your guess"

int main(int argc, char **argv)
{
	long int msq = 0;
	char *text = NULL;
	int n = 0;

	msq = create_queue(MSG_ID_FILE);

	if (msq == -1) {
		return EXIT_FAILURE;
	}

	while (1) {
		text = receive_message(msq, MSG_TYPE_IN);

		if (text == NULL) {
			return EXIT_FAILURE;
		}

		printf("%s", text);
		free(text);
		text = NULL;

		while (read_int(PROMPT, &n, MSGMAX) == 1) {
		}

		if (send_int(msq, MSG_TYPE_OUT, n) != 0) {
			return EXIT_FAILURE;
		}

		if (n == -1) {
			break;
		}

	}

	return EXIT_SUCCESS;
}
