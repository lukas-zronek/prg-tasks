/*
 * @file print_ascii_table.c
 * @author Lukas Zronek
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>

#define MAX_READ_LINE_INPUT 12

#define ASCII 1
#define HEX 2
#define BIN 4
#define OCTAL 8
#define DEC 16

char *read_line(char *, size_t);
long read_int(char *);
long parse_int(char *);
char *dec2bin(int, int);

int main(void)
{
	char input_buffer[MAX_READ_LINE_INPUT] = {0};
	size_t input_length = 0;
	int i = 0;
	int format = 0;
	long start = 0, end = 0;
	char *nonprintable_ascii_table[] = {"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US"};
	char *bin_string = NULL;

	assert(MAX_READ_LINE_INPUT > 0);
	assert(MAX_READ_LINE_INPUT <= INT_MAX);

	while (format == 0) {
		printf("Specify the output format (A)scii, (H)ex, (B)in, (O)ctal, (D)ec > ");

		if (read_line(input_buffer, sizeof(input_buffer)) != NULL) {
			input_length = strlen(input_buffer);

			for (i = 0; i < input_length; i++) {
				input_buffer[i] = toupper(input_buffer[i]);

				switch(input_buffer[i]) {
					case 'A': { format += ASCII; } break;
					case 'H': { format += HEX; } break;
					case 'B': { format += BIN; } break;
					case 'O': { format += OCTAL; } break;
					case 'D': { format += DEC; } break;
					default: break;
				}
			}
		}
	}

	while (start == 0 && end == 0) {
		start = read_int("Specify the start > ");

		end = read_int("Specify the end > ");

		if (start >= end) {
			printf("\nThe specified range is not valid: Start < End\n\n");
			start = end = 0;
		}
	}

	printf("\nASCII Table\n\n");

	for (i = start; i <= end; i++) {
		if (format & ASCII) {
			if (i >= 0 && i < 32) {
				printf("%s\t", nonprintable_ascii_table[i]);
			} else {
				printf("%c\t", i);
			}
		}
		if (format & OCTAL) {
			printf("%03o\t", i);
		}
		if (format & BIN) {
			bin_string = dec2bin(i, 8);

			if (bin_string != NULL) {
				printf("%s\t", bin_string);

				free(bin_string);
				bin_string = NULL;
			}
		}
		if (format & HEX) {
			printf("0x%.2X\t", i);
		}
		if (format & DEC) {
			printf("%3d\t", i);
		}
		printf("\n");
	}

	return EXIT_SUCCESS;
}

char *read_line(char *buf, size_t buf_size)
{
	int length = 0;
	char ch = 0;

	if (buf == NULL || buf_size == 0) {
		return NULL;
	}

	/* fgets expects an int as second parameter and not a size_t */
	if (buf_size > INT_MAX) {
		fprintf(stderr, "Warning: Input truncated.\n");
		buf_size = INT_MAX;
	}

	if (NULL != fgets(buf, (int)buf_size, stdin)) {
		length = strlen(buf);

		if (length > 0) {
			length--;
		}

		if (buf[length] != '\n') {
			do {
				ch = getchar();
			} while (ch != '\n' && ch != EOF);
		} else {
			buf[length] = '\0';
		}

		return buf;
        } else {
		if (ferror(stdin)) {
			perror("fgets");
		}

		if (feof(stdin) != 0) {
			exit(EXIT_SUCCESS);
		}
	}

	return NULL;
}

long read_int(char *prompt) {
	char input_buffer[MAX_READ_LINE_INPUT] = {0};
	long n = -1;

	while (n == -1) {
		printf("%s", prompt);

		if (read_line(input_buffer, sizeof(input_buffer)) != NULL) {
			n = parse_int(input_buffer);
		}
	}

	return n;
}


long parse_int(char *input)
{
	long result = 0;
	char *endptr = NULL;
	int errnum = 0;

	errno = 0;

	result = strtol(input, &endptr, 10);

	errnum = errno;

	if ((result == 0 && errnum > 0) || input == endptr || result < 0 || result > 255) {
		return -1;
	} else {
		return result;
	}
}

char *dec2bin(int d, int digits)
{
	int i = 0;
	int j = 0;
	char *s = NULL;

	if (digits <= 0) {
		return NULL;
	}

	s = (char *)malloc(sizeof(char) * (digits + 1));

	if (s == NULL) {
		fprintf(stderr, "malloc failed\n");
		return NULL;
	}

	for (i = digits - 1; i >= 0; i--) {
		j = digits - 1 - i;
		if (d & (1 << i)) {
			s[j] = '1';
		} else {
			s[j] = '0';
		}
	}

	s[digits] = '\0';

	return s;
}
