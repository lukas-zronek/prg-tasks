/**
 * @file millionaire.c
 * @author Lukas Zronek
 * @date 10.01.2017
 * @brief a quiz game in the style of "Who wants to be a millionaire"
 * @version 1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define DELIMITER ';'
#define MAX_LEN 80
#define FREE(val) free(val); val = NULL;

typedef enum { false, true, empty } bool;

typedef struct linked_list_s {
	char *question;
	char *answers[4];
	int answer;
	struct linked_list_s *next;
} linked_list;

void list_linked_list(linked_list *p);
void free_linked_list(linked_list **p);
linked_list *parse_question_file(const char *filename, int *question_count);
char ask(const char *prompt, const char *allowed_answers, int visible_answer_end);
linked_list *randomize_questions(linked_list *l, int question_count);
void randomize_answers(linked_list *next);

int main(int argc, char *argv[])
{
	bool invalid_opt = false;
	int opt = 0;
	char *optarg_endptr = NULL;
	char *input_filename = NULL;
	int question_count = 0;
	int joker_count = -1;
	int second_chances_count = -1;
	char guess = '\0';
	linked_list *questions = NULL;
	linked_list *next = NULL;
	linked_list *prev = NULL;
	int error = 0;
	int i = 0;
	char possible_answers[7] = {0};
	int joker = 0;
	int j = 0;
	int random_number = 0;

	srand(time(NULL));

	while (1) {
		opt = getopt(argc, argv, "f:j:s:h");

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 'f':
				input_filename = optarg;
				break;
			case 'j':
				joker_count = (int)strtod(optarg, &optarg_endptr);
				if (*optarg_endptr != '\0' || joker_count < 0) {
					fprintf(stderr, "Error: Invalid 50/50 joker number\n");
					invalid_opt = true;
				}
				break;
			case 's':
				second_chances_count = (int)strtod(optarg, &optarg_endptr);
				if (*optarg_endptr != '\0' || second_chances_count < 0) {
					fprintf(stderr, "Error: Invalid second chances number\n");
					invalid_opt = true;
				}
				break;
			default:
				invalid_opt = true;
				break;
		}
	}

	if (invalid_opt == true || input_filename == NULL || optind < argc) {
		fprintf(stderr, "Usage: %s <-f FILENAME> [-j 50_50_JOKERS] [-s SECOND_CHANCES] [-h]\n", argv[0]);
		return EXIT_FAILURE;
	}

	questions = parse_question_file(input_filename, &question_count);

	if (questions == NULL) {
		return EXIT_FAILURE;
	}

	questions = randomize_questions(questions, question_count);

	if (joker_count == -1) {
		joker_count = question_count / 10 + 1;
	}

	if (second_chances_count == -1) {
		second_chances_count = question_count / 5 + 1;
	}

	printf("Read %d questions from %s\n\n", question_count, input_filename);

	printf("=== Who wants to be a millionaire ===\n\n\
Press [A/B/C/D] to guess\nPress [Q] to give up\nPress [J] to use a 50/50 joker\n");

	next = questions;

	while (next != NULL) {
		printf("\n%s\n\n", next->question);

		if (joker == 1) {
			joker = -1;

			for (i = 0; i < 2; i++) {
				random_number = (rand() % 4);

				if (random_number + 1 == next->answer) {
					if (next->answer == 4) {
						random_number = 1;
					} else {
						random_number++;
					}
				}

				if (next->answers[random_number][0] != '\0') {
					next->answers[random_number][0] = '\0';
				} else {
					i--;
				}
			}
		}

		j = 0;

		for (i = 0; i < 4; i++) {
			if (next->answers[i][0] != '\0') {
				possible_answers[j] = 65 + i;
				possible_answers[j + 1] = '\0';
				printf("\t%c: %s\n", possible_answers[j], next->answers[i]);
				j++;
			}
		}

		possible_answers[j] = 'Q';

		printf("\n");

		if (joker == -1) {
			printf("You can't use any more jokers this round.\n\n");
			possible_answers[j + 1] = '\0';
		} else {
			if (joker_count > 0) {
				printf("You have %d 50/50 jokers left [J]\n\n", joker_count);
			}

			if (second_chances_count > 0) {
				printf("You have %d 2nd chance jokers left\n\n", second_chances_count);
			}

			possible_answers[j + 1] = 'J';
			possible_answers[j + 2] = '\0';
		}

		guess = ask("Guess", possible_answers, j);

		if (guess == 0) {
			error = 1;
			break;
		} else if (guess == 'Q') {
			printf("\nQuit. You lost.\n");
			break;
		} else if (guess == 'J') {
			if (joker_count > 0) {
				joker = 1;
				joker_count--;
			} else {
				printf("\nNo 50/50 jokers left!\n");
			}
			continue;
		} else if (guess == (65 + next->answer - 1)) {
			printf("\nCorrect.");

			if (question_count == 1) {
				printf(" You Won.\n");
				break;
			} else {
				printf("\n");
			}
		} else {
			printf("\nWrong answer. ");

			if (second_chances_count > 0 && joker != -1) {
				if (ask("Use 2nd chance joker?", "YNQ", 2) == 'Y') {
					second_chances_count--;
					joker = -1;
					continue;
				}
			}
			printf("You lost.\n");
			break;
		}

		joker = 0;
		question_count--;

		prev = next->next;
		next = prev;
	}

	free_linked_list(&questions);

	if (error == 1) {
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}

void free_linked_list(linked_list **p)
{
	linked_list *l = NULL;
	linked_list *n = NULL;
	int i = 0;
	
	l = *p;

	while (l != NULL) {
		FREE(l->question);
		for (i = 0; i < 4; i++) {
			FREE(l->answers[i]);
		}
		l->answer = 0;
		n = l->next;
		FREE(l);
		l = n;
	}

	*p = NULL;
}

char ask(const char *prompt, const char *allowed_answers, int visible_answer_end)
{
	char c = '\0';
	bool quit = false;
	int i = 0;

	while (quit == false) {
		printf("%s [", prompt);
		for (i = 0; i < visible_answer_end; i++) {
			printf("%c", allowed_answers[i]);
		}
		printf("]> ");

		if (scanf("%c", &c) < 1) {
			fprintf(stderr, "Error: Invalid input or input error\n");
			return 0;
		}

		c = toupper(c);

		/* clear input buffer */
		while (getchar() != '\n') {
		}

		for (i = 0; allowed_answers[i] != '\0'; i++) {
			if (allowed_answers[i] == c) {
				quit = true;
				break;
			}
		}
	}

	return c;
}

linked_list *parse_question_file(const char *filename, int *question_count)
{
	FILE *input_file = NULL;
	char input_char = '\0';
	char buffer[MAX_LEN + 1] = {0};
	char *buffer_endptr = NULL;
	int char_count = 0;
	int field_index = 0;
	char *str = NULL;
	size_t str_len = 0;
	linked_list *head = NULL;
	linked_list *next = NULL;
	linked_list *prev = NULL;
	int error = 0;
	int i = 0;

	*question_count = 0;

	input_file = fopen(filename, "r");

	if (input_file == NULL) {
		perror("Error: Opening the question file failed");
		return NULL;
	}

	while (1) {
		input_char = fgetc(input_file);

		if (input_char == EOF) {
			break;
		} else if (char_count > MAX_LEN) {
			fprintf(stderr, "Error: Parsing question file failed: Column longer than %d characters.\n", MAX_LEN);
			error = 1;
			break;
		} else if (input_char == DELIMITER) {
			buffer[char_count] = '\0';

			if (field_index == 0) {
				next = (linked_list *)malloc(sizeof (linked_list));

				if (next == NULL) {
					fprintf(stderr, "Error: Parsing question file failed: Malloc failed\n");
					error = 1;
					break;
				}
				next->next = NULL;
				next->question = NULL;
				for (i = 0; i < 4; i++) {
					next->answers[i] = NULL;
				}
				next->answer = 0;

				if (head == NULL) {
					head = next;
				} else if (prev != NULL) {
					prev->next = next;
				}
			}

			str_len = strlen(buffer) + 1;

			str = (char *)malloc(str_len);

			if (str == NULL) {
				fprintf(stderr, "Error: Parsing question file failed: Malloc failed\n");
				error = 1;
				break;
			}

			if (strncpy(str, buffer, str_len) != str) {
				fprintf(stderr, "Error: Parsing question file failed: strncpy() failed\n");
				error = 1;
				FREE(str);
				break;
			}

			switch (field_index) {
				case 0:
					next->question = str;
					break;
				case 1:
					next->answers[0] = str;
					break;
				case 2:
					next->answers[1] = str;
					break;
				case 3:
					next->answers[2] = str;
					break;
				case 4:
					next->answers[3] = str;
					break;
				default:
					fprintf(stderr, "Error: Parsing question file failed: question record has too many columns\n");
					FREE(str);
					error = 1;
					break;
			}

			if (error == 1) {
				break;
			}

			char_count = 0;
			field_index++;
			continue;
		} else if (input_char == '\n') {
			if (field_index != 5) {
				fprintf(stderr, "Error: Parsing question file failed: record has too little columns\n");
				error = 1;
				break;
			}

			next->answer = (int)strtod(buffer, &buffer_endptr);

			if (*buffer_endptr != '\0' || next->answer < 1 || next->answer > 4) {
				fprintf(stderr, "Error: Parsing question file failed: Invalid number\n");
				error = 1;
				break;
			}

			randomize_answers(next);

			buffer[char_count] = '\0';
			field_index = 0;
			char_count = 0;
			(*question_count)++;
			prev = next;
			continue;
		} else {
			buffer[char_count] = input_char;
			buffer[char_count + 1] = '\0';
			char_count++;
		}
	}

	if (fclose(input_file) != 0) {
                perror("Error: Closing question file failed");
		error = 1;
        }

	if (error == 0 && *question_count == 0) {
		fprintf(stderr, "Error: Question file is empty\n");
		error = 1;
	}

	if (error == 1) {
		free_linked_list(&head);
		return NULL;
	} else {
		return head;
	}
}

linked_list *randomize_questions(linked_list *l, int question_count)
{
	unsigned long random_number = 0;
	linked_list *c = NULL;
	linked_list *n = NULL;
	linked_list *h = NULL;
	int i = 0;
	int j = 0;

	for (i = 0; i < question_count; i++) {
		random_number = rand() % question_count;

		if (random_number == 0) {
			continue;
		}

		h = l->next;

		j = 0;

		for (c = l; c != NULL; c = c->next) {
			if (j == random_number) {
				n = c->next;
				c->next = l;
				l->next = n;
				break;
			}
			j++;
		}

		l = h;
	}

	return h;
}

void randomize_answers(linked_list *l)
{
	int random_number = 0;
	char *a = NULL;
	int i = 0;

	for (i = 0; i < 8; i++) {
		random_number = rand() % 4;

		if (random_number == 0) {
			continue;
		}

		if (l->answer == 1) {
			l->answer = random_number + 1;
		} else if (l->answer == (random_number + 1)) {
			l->answer = 1;
		}

		a = l->answers[random_number];
		l->answers[random_number] = l->answers[0];
		l->answers[0] = a;
	}
}
