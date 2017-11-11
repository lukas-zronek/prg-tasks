/**
 * @file util.c
 * @author Lukas Zronek
 * @date 26.01.2017
 * @brief M3U Playlist Editor
 * @version 1.0
 *
 */

#include <util.h>

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
		FREE(buffer);
		return NULL;
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

/*
int trim_newline(char *str)
{
	char *p = NULL;

	if (str == NULL) {
		return 1;
	}

	p = strchr(str, '\n');

	if (p == NULL) {
		fprintf(stderr, "strchr.\n");
		return 1;
	}

	*p = '\0';
	
	return 0;
}
*/

/*
 * remove c from the beginning (by moving the pointer)
 * and the end of str (by setting NULL terminator)
 *
 * returns: pointer to str
 *
 */
char *trim(char *str, char c)
{
	char *end = NULL;
	size_t str_length = 0;

	while (*str == c) {
		str++;
	}

	str_length = strlen(str);

	if (str_length < 1) {
		return str;
	}

	end = &str[--str_length];

	while (*end == c) {
		*end = '\0';
		end++;
	}

	return str;
}

char *format_length(int n)
{
	int minutes = 0;
	int seconds = 0;
	char *format = NULL;
	const int max_length = 6;

        /* limit output to 59:59 (3599 = 59 * 60 + 59) */
        if (n < 0 || n > 3599) {
                fprintf(stderr, "Error: 0 < n < 3599\n");
                return NULL;
        }

	format = (char *)malloc(max_length);

	if (format == NULL) {
		fprintf(stderr, "Malloc failed.\n");
		return NULL;
	}

	minutes = n / 60;
	seconds = n % 60;

	if (snprintf(format, max_length, "%02d:%02d", minutes, seconds) < 0) {
		return NULL;
	}

	return format;
}
