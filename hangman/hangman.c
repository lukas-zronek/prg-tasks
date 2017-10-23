/**
 * @file hangman.c
 * @author Lukas Zronek
 * @date 06.12.2016
 * @brief a simple Hangman game
 * @version 1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#ifndef ARG_MAX
#define ARG_MAX 1024
#endif

#define MAX_TRIES 10

#define SAFE_FREE(val) if (val != NULL) { free(val); val = NULL; }

struct linked_list {
	char *line;
	struct linked_list *next;
};

char *random_word_from_file(const char *);
void string_to_upper(char *);
void free_linked_list(struct linked_list **);

int main(int argc, char *argv[])
{
	char c = 0;
	char *word = NULL;
	size_t word_length = 0;
	char *guessed_word = NULL;
	char wrong_letters[MAX_TRIES + 1] = {0};
	unsigned int wrong_letters_count = 0;
	unsigned int i = 0;
	char *word_ptr = NULL;
	int tries = 0;
	int line_length = 0;
	char *buffer = NULL;

	switch (argc) {
		case 2:
			word = argv[1];
			break;
		case 3:
			if (strcmp(argv[1], "-f") == 0) {
				word = random_word_from_file(argv[2]);

				if (word == NULL) {
					return EXIT_FAILURE;
				}
			}
			break;
		default:
			break;
	}

	if (word == NULL) {
		fprintf(stderr, "Usage:\n\t%s WORD\n\t%s -f FILE\n\nFILE is expected to be a unix text file containing words separated by newlines.\n", argv[0], argv[0]);
		return EXIT_FAILURE; }

	/* convert to uppercase for incasesensitive comparision */
	string_to_upper(word);

	/* sizeof (char) is assumed to be 1 byte */
	buffer = (char *)malloc(ARG_MAX);
	
	if (buffer == NULL) {
		fprintf(stderr, "Error: malloc failed.\n");
		if (argc == 3) {
			SAFE_FREE(word);
		}
		return EXIT_FAILURE;
	}

	word_length = strlen(word);

	guessed_word = (char *)malloc(word_length + 1);

	if (guessed_word == NULL) {
		fprintf(stderr, "Error: malloc failed.\n");
		if (argc == 3) {
			SAFE_FREE(word);
		}
		SAFE_FREE(buffer);
	}

	memset(guessed_word, 0, word_length + 1);

	for (tries = MAX_TRIES; tries >= 0; tries--) {
		printf("\nMy word: ");
		for (i = 0; i < word_length; i++) {
			if (guessed_word[i] == 0) {
				printf("_ ");
			} else {
				printf("%c ", guessed_word[i]);
			}
		}

		if (strlen(guessed_word) == word_length) {
			printf("\nYou won!\n");
			break;
		} else if (tries == 0) {
			printf("\nYou lost!\n");
			break;
		}

		printf("\n\nYou have %d tr%s left.\nGuess a letter.", tries, (tries == 1) ? "y":"ies");

		i = 0;
		while (wrong_letters[i] != '\0') {
			if (i == 0) {
				printf("\nLetters not in my word: ");
			}
			printf("%c ", wrong_letters[i]);
			i++;
		}

		c = 0;

		while (c == 0) {
			printf("\n> ");

			if (fgets(buffer, ARG_MAX, stdin) != NULL) {
				line_length = strnlen(buffer, ARG_MAX - 1);
				if (line_length == 2 && buffer[1] == '\n') {
					c = buffer[0];
				} else if (line_length > 2 && buffer[line_length - 2] == '!') {
					/* overwrite exclamation mark */
					buffer[line_length - 2] = '\0';

					string_to_upper(buffer);

					if (strcmp(word, buffer) == 0) {
						printf("Correct! You won!\n");
					} else {
						printf("Wrong! You lost!\n");
					}


					SAFE_FREE(buffer);

					SAFE_FREE(guessed_word);

					if (argc == 3) {
						SAFE_FREE(word);
					}

					return EXIT_SUCCESS;
				} else {
					printf("Guess a single letter or a complete word followed by an exclamation mark.\n");
				}
			}
		}

		c = toupper(c);

		if (strchr(guessed_word, c) != NULL) {
			/* letter already guessed -> do not count this try */
			tries++;
			continue;
		} else if (strchr(wrong_letters, c) != NULL) {
			tries++;
			continue;
		} else {
			word_ptr = strchr(word, c);
			if (word_ptr == NULL) {
				printf("\nWrong!\n");
				wrong_letters[wrong_letters_count] = c;
				wrong_letters_count++;
				wrong_letters[wrong_letters_count] = '\0';
			} else {
				printf("\nCorrect!\n");
				tries++;

				while (1) {
					guessed_word[word_ptr - word] = word[word_ptr - word];
					word_ptr = strchr(word_ptr + 1, c);
					if (word_ptr == NULL) {
						break;
					}
				}
			}
		}
	}

	SAFE_FREE(buffer);

	SAFE_FREE(guessed_word);

	if (argc == 3) {
		SAFE_FREE(word);
	}

	return EXIT_SUCCESS;
}

/*
 * reads file line by line and returns a linked list or NULL on error
 */
char *random_word_from_file(const char *filename)
{
	FILE *input_file = NULL;
	char *buffer = NULL;
	char *line = NULL;
	struct linked_list *head = NULL;
	struct linked_list *current = NULL;
	struct linked_list *next = NULL;
	size_t line_length = 0;
	unsigned long list_length = 0;
	unsigned long random_number = 0;
	char *result = NULL;
	unsigned int i = 0;
	int error = 0;
	size_t result_length = 0;
	
	/* sizeof (char) is assumed to be 1 byte */
	buffer = (char *)malloc(ARG_MAX);
	
	if (buffer == NULL) {
		fprintf(stderr, "Error: malloc failed.\n");
		return NULL;
	}

	input_file = fopen(filename, "r");

	if (input_file == NULL) {
		perror("Error: Could not read file");
		SAFE_FREE(buffer);
		return NULL;
	}

	while (fgets(buffer, ARG_MAX, input_file) != NULL) {
		line_length = strnlen(buffer, ARG_MAX - 1);

		if (line_length < 2) {
			continue;
		} else if (line_length == (ARG_MAX - 1) && buffer[ARG_MAX - 2] != '\n') {
			/* A warning is incorrectly reported if the word is exactly as large as ARG_MAX
			or when the line does not contain a newline. */
			fprintf(stderr, "Warning: Input truncated.\n");
		} else if (buffer[line_length - 1] == '\n') {
			buffer[line_length - 1] = '\0';
			line_length--;
		}

		/* sizeof (char) is assumed to be 1 byte */
		line = (char *)malloc(line_length + 1);

		if (line == NULL) {
			fprintf(stderr, "Error: malloc failed.\n");
			error = 1;
			break;
		}

		if (strncpy(line, buffer, line_length + 1) != line) {
			fprintf(stderr, "Error: strncpy failed.\n");
			error = 1;
			SAFE_FREE(line);
			break;
		}

		next = (struct linked_list *)malloc(sizeof (struct linked_list));

		if (next == NULL) {
			fprintf(stderr, "Error: malloc failed\n");
			error = 1;
			SAFE_FREE(line);
			break;
		}

		next->line = line;

		if (head != NULL && current != NULL) {
			current->next = next;
			current = next;
		} else {
			head = current = next;
		}

		current->next = NULL;

		list_length++;
	}

	line = NULL;
	current = next = NULL;

	SAFE_FREE(buffer);

	if (input_file != NULL && fclose(input_file) != 0) {
		perror("Error: closing input file failed");
		error = 1;
	}

	if (error == 1) {
		free_linked_list(&head);
		return NULL;
	}

	srand(time(NULL));

	random_number = rand() % list_length;

	current = head;

	for (i = 0; i < list_length; i++) {
		if (i == random_number) {
			result_length = strnlen(current->line, ARG_MAX);
			if (result_length > 0) {
				result = (char *)malloc(result_length + 1);
				if (result == NULL) {
					fprintf(stderr, "Error: malloc failed.\n");
				} else {
					if (strncpy(result, current->line, result_length + 1) != result) {
						SAFE_FREE(result);
						break;
					}
				}
				break;
			}
			break;
		}
		current = current->next;
	}

	current = NULL;

	free_linked_list(&head);

	return result;
}

void string_to_upper(char *s)
{
	int i = 0;

	for (; s[i] != '\0'; i++) {
		s[i] = toupper(s[i]);
	}
}

void free_linked_list(struct linked_list **p)
{
	struct linked_list *l = NULL;
	struct linked_list *n = NULL;
	
	l = *p;

	while (l != NULL) {
		SAFE_FREE(l->line);
		n = l->next;
		SAFE_FREE(l);
		l = n;
	}

	*p = NULL;
}