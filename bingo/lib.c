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
int delete_queue(long int id)
{
	if (msgctl(id, IPC_RMID, NULL) != 0) {
		perror("msgctl");
		return 1;
	}
	return 0;
}

/* returns 0 on success and 1 otherwise */
int send_message(long int id, long int type, char *text)
{
	msg m;
	size_t text_length = 0;
	size_t mtext_size = sizeof(m.mtext);

	m.mtype = type;

	assert(mtext_size > 1);

	text_length = strnlen(text, mtext_size - 1);

	if (text_length == 0) {
		fprintf(stderr, "Error: message text has zero length.\n");
		return 1;
	}

	if (text_length == mtext_size - 1) {
		if (text[text_length] != '\0') {
			fprintf(stderr, "Warning: message text truncated.\n");
		}
	}

	memset(m.mtext, 0, mtext_size);

	if (strncpy(m.mtext, text, text_length) != m.mtext) {
		fprintf(stderr, "Error: Copying text failed.\n");
		return 1;
	}

	if (msgsnd(id, &m, mtext_size, 0) == -1) {
		perror("msgsnd");
		return 1;
	}

	return 0;
}

char *receive_message(long int id, long int type)
{
	msg m;
	char *s = NULL;
	size_t s_length = 0;
	size_t mtext_size = sizeof(m.mtext);

	assert(mtext_size > 1);

	if (msgrcv(id, &m, mtext_size, type, 0) == -1) {
		perror("msgrcv");
	} else {
		s_length = strnlen(m.mtext, mtext_size - 1);

		if (s_length == 0) {
			fprintf(stderr, "Error: message text has zero length.\n");
			return NULL;
		} else if (s_length == mtext_size - 1) {
			if (m.mtext[s_length] != '\0') {
				fprintf(stderr, "Warning: message text truncated.\n");
			}
		}

		s = (char *)calloc(s_length + 1, sizeof(char));

		if (s == NULL) {
			fprintf(stderr, "Allocating string failed.\n");
			return NULL;
		}

		if (strncpy(s, m.mtext, s_length) != s) {
			fprintf(stderr, "Error: copying mtext failed.\n");
			free(s);
			s = NULL;
		}
	}

	return s;
}

/* returns 0 on success and 1 otherwise */
int send_int(long int id, long int type, int n)
{
	char buffer[MSGMAX] = {0};

	if (snprintf(buffer, sizeof(buffer), "%d", n) < 0) {
		fprintf(stderr, "Error: snprintf failed\n");
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
	int error = 0;

	*n = 0;

	text = receive_message(id, type);

	if (text == NULL) {
		return 1;
	}

        *n = (int) strtod(text, &endptr);

        if (*endptr != '\0') {
                fprintf(stderr, "strtod failed\n");
		error = 1;
        }

	free(text);
	text = NULL;
	endptr = NULL;

	return error;
}

/*
 * reads a line of size buffer_size from stdin. The Newline is removed from input.
 * returns: NULL on error or string
 *
 */
char *read_line(char *prompt, int buffer_size)
{
	char *buffer = NULL;
	char *result = NULL;
	int newline_span = 0;

	if (buffer_size <= 0) {
		return NULL;
	}

	buffer = (char *)malloc(buffer_size);

	if (buffer == NULL) {
		fprintf(stderr, "Allocating buffer failed.\n");
		return NULL;
	}

	printf("%s> ", prompt);

	if (fgets(buffer, buffer_size, stdin) != NULL) {
		newline_span = strcspn(buffer, "\n");
		if ((newline_span + 1) == buffer_size) {
			fprintf(stderr, "Warning: Input truncated.\n");

			/* clear stdin */
			fseek(stdin, 0, SEEK_SET);
		}
		buffer[newline_span] = '\0';
	} else {
		if (ferror(stdin)) {
			perror("Error: fgets failed");
		}
		clearerr(stdin);
		free(buffer);
		buffer = NULL;
		return NULL;
	}

	result = copy_string(buffer);

	free(buffer);
	buffer = NULL;

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
			free(input);
			input = NULL;
			continue;
		}

		result = (int)strtod(input, &input_endptr);

		if (*input_endptr != '\0') {
			free(input);
			input = NULL;
			input_endptr = NULL;
			continue;
		}

		break;
	}

	free(input);
	input = NULL;

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
		free(dst);
		dst = NULL;
		return NULL;
	}

	return dst;
}

/*
 * Warning: This function is not cryptographically secure and
 * may return deterministic values
 */
int get_rand(int max)
{
	static int seeded = 0;

	if (max <= 0) {
		return 0;
	}

	if (seeded == 0) {
		struct timeval t1;
		if (gettimeofday(&t1, NULL) != 0) {
			perror("gettimeofday");
			errno = 0;
			fprintf(stderr, "Warning: get_rand: No seed value is provided.");
			t1.tv_usec = 1;
			t1.tv_sec = 1;
		}
		srand(t1.tv_usec * t1.tv_sec);
		seeded = 1;
	}

	return rand() % max;
}
