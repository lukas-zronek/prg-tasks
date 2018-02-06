/*
 * @file upper_lower.c
 * @author Lukas Zronek
 * @brief reads-in some text and prints the text either in all uppercase or all lowercase letters.
 *
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAX_INPUT 256
#define MAX_LINES 100
#define TO_UPPER 'U'
#define TO_LOWER 'L'

char *convertCase(char *, int);
char *read_line(void);

int main(void)
{
	char *line = NULL;
	char **all_lines = NULL;
	char c = 0;
	int quit = 0;
	int i = 0;

	assert(MAX_LINES > 0);
	assert(MAX_INPUT > 0);

	all_lines = calloc(MAX_LINES, sizeof(char *));

	if (all_lines == NULL) {
		fprintf(stderr, "Allocating memory for all_lines failed\n");
		return EXIT_FAILURE;
	}

	printf("Enter your text (terminate with ENTER twice) >\n");

	for (i = 0; i < MAX_LINES - 1; i++) {
		line = read_line();

		if (line == NULL) {
			continue;
		} else {
			if (strncmp(line, "\n", 2) == 0) {
				quit++;
			} else {
				quit = 1;
			}
			all_lines[i] = line;
			all_lines[i + 1] = NULL;
		}

		if (quit >= 2) {
			break;
		}
	}

	while (c != TO_LOWER && c != TO_UPPER) {
		printf("Output in (U)ppercase or (L)owercase > ");

		line = read_line();

		if (line != NULL) {
			if (sscanf(line, "%c", &c) == 1) {
				c = toupper(c);
			} else {
				c = 0;
			}
		}
	}

	i = 0;

	while (all_lines[i] != NULL) {
		if (convertCase(all_lines[i], c) != NULL) {
			printf("%s", all_lines[i]);
			free(all_lines[i]);
			all_lines[i] = NULL;
		}
		i++;
	}

	free(all_lines);
	all_lines = NULL;

	return EXIT_SUCCESS;
}

/*
 * function converts char array inplace to case specified by f
 * returns NULL on error and returns s on success
 */
char *convertCase(char *s, int f)
{
	int i = 0;
	size_t length = strnlen(s, MAX_INPUT);

	for (i = 0; i < length; i++) {
		switch(f) {
			case TO_UPPER: s[i] = toupper(s[i]); break;
			case TO_LOWER: s[i] = tolower(s[i]); break;
			default: return NULL;
		}
	}

	return s;
}

char *read_line()
{
	char *line = NULL;

	line = calloc(MAX_INPUT, sizeof(char));

	if (line == NULL) {
		fprintf(stderr, "Allocating memory failed\n");
		return NULL;
	}

	if (fgets(line, MAX_INPUT, stdin) == NULL) {
		free(line);
		line = NULL;
		return NULL;
	}

	return line;
}
