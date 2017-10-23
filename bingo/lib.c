/*
 * @file lib.c
 * @author Lukas Zronek
 *
 */

#include "lib.h"

/* return -1 on error */
long int create_queue(char *filename)
{
	long int msq = 0;
	key_t key = 0;

	key = ftok(filename, 23);

	if (key == -1) {
		fprintf(stderr, "ftok failed\n");
		return -1;
	}

	msq = msgget(key, IPC_CREAT | SEM_PERM);

	if (msq == -1) {
		perror("msgget");
	}

	return msq;
}

/* returns 0 on success and 1 otherwise */
int send_message(long int id, long int type, char *text)
{
	msg m;
	size_t text_size = 0;

	m.mtype = type;

	text_size = strlen(text) + 1;

	if (text_size > MSGMAX) {
		text_size = MSGMAX;

		fprintf(stderr, "Warning: mtext truncated.\n");
	}

	if (strncpy(m.mtext, text, text_size) != m.mtext) {
		fprintf(stderr, "Error: Copying text failed.\n");
		return 1;
	}

	if (MSGMAX > 0) {
		m.mtext[MSGMAX - 1] = '\0';
	} else {
		m.mtext[0] = '\0';
	}

	if (msgsnd(id, &m, text_size, 0) == -1) {
		perror("msgsnd");
		return 1;
	}

	return 0;
}

char *receive_message(long int id, long int type)
{
	msg m;
	char *s = NULL;
	size_t s_size = 0;

	if (msgrcv(id, &m, MSGMAX, type, 0) == -1) {
		perror("msgrcv");
	} else {
		s_size = strlen(m.mtext) + 1;

		s = (char *)malloc(s_size);

		if (s == NULL) {
			fprintf(stderr, "Allocating string failed.\n");
			return NULL;
		}

		if (strncpy(s, m.mtext, s_size) != s) {
			fprintf(stderr, "Error: copying mtext failed.\n");
			FREE(s);
		}
	}

	return s;
}

/* returns 0 on success and 1 otherwise */
int send_int(long int id, long int type, int n)
{
	char buffer[MSGMAX] = {0};

	if (sprintf(buffer, "%d", n) < 0) {
		fprintf(stderr, "Error: sprintf failed\n");
		return 1;
	}

	if (send_message(id, type, buffer) != 0) {
		return 1;
	}

	return 0;
}

int receive_int(long int id, long int type, int *n)
{
	char *text = NULL;
	char *endptr = NULL;

	*n = 0;

	text = receive_message(id, type);

	if (text == NULL) {
		return 1;
	}

        *n = (int) strtod(text, &endptr);

        if (*endptr != '\0' || *n < 0) {
                fprintf(stderr, "strtod failed\n");
		return 1;
        }

	return 0;
}

/*
 * reads a line of size buffer_size from stdin. The Newline is removed from input.
 * returns: NULL on error or string
 *
 */
char *read_line(char *prompt, int buffer_size)
{
	char *buffer = NULL;
	char *newline_ptr = NULL;
	char *result = NULL;

	buffer = (char *)malloc(buffer_size);

	if (buffer == NULL) {
		fprintf(stderr, "Allocating buffer failed.\n");
		return NULL;
	}

	while (1) {
		printf("%s> ", prompt);

		if (fgets(buffer, buffer_size, stdin) != NULL) {
			if ((newline_ptr = strchr(buffer, '\n')) != NULL) {
				*newline_ptr = '\0';
				break;
			} else {
				fprintf(stderr, "Error: Input too long.\n");

				/* clear stdin */
				fseek(stdin, 0, SEEK_SET);
			}
		} else {
			if (ferror(stdin) || feof(stdin) != 0) {
				FREE(buffer);
				return NULL;
			}
		}
	}

	result = copy_string(buffer);

	FREE(buffer);

	return result;
}

/*
 * reads integer from stdin
 *
 * n: will be overwritten with the entered number
 * buffer_size: how much will be read from stdin
 *
 * returns: 0 on success or 1 on error
 *
 */
int read_int(char *prompt, int *n, int buffer_size)
{
	char *input = NULL;
	char *input_endptr = NULL;
	int result = 0;

	while (1) {
		input = read_line(prompt, buffer_size);

		if (input == NULL) {
			return 1;
		}

		if (*input == '\0') {
			FREE(input);
			continue;
		}

		result = (int)strtod(input, &input_endptr);

		if (*input_endptr != '\0') {
			FREE(input);
			input_endptr = NULL;
			continue;
		}

		break;
	}

	FREE(input);

	*n = result;

	return 0;
}

/*
 * copies string to dynamically allocated string
 *
 * returns: pointer to string on success or NULL on error
 *
 */
char *copy_string(char *src)
{
	char *dst = NULL;
	int src_length = strlen(src);

	dst = (char *)malloc(src_length + 1);

	if (dst == NULL) {
		fprintf(stderr, "Allocating string failed.\n");
		return NULL;
	}

	if (strncpy(dst, src, src_length + 1) != dst) {
		fprintf(stderr, "strncpy failed.\n");
		FREE(dst);
		return NULL;
	}

	return dst;
}

int get_rand(int max)
{
	static int seeded = 0;

	if (max <= 0) {
		return 0;
	}

	if (seeded == 0) {
		struct timeval t1;
		gettimeofday(&t1, NULL);
		srand(t1.tv_usec * t1.tv_sec);
		seeded = 1;
	}

	return rand() % max;
}
