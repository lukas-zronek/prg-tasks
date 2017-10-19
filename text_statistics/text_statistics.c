/*
 * text_statistics.c
 * Description: a program that prints some statistics regarding the provided text. (based on vowels.c)
 * Author: Lukas Zronek
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
	char c = 0;
	char saved_c = 0;
	int quit = 0;

	int alpha_count = 0;
	int punctuation_count = 0;
	int space_count = 0;
	int numeric_count = 0;
	int line_count = 0;
	int word_count = 0;

	printf("Enter your text (terminate with ENTER twice) >\n");

	while (quit < 2) {
		c = getchar();

		if (c == '\n') {
			line_count++;
			quit++;
			continue;
		} else {
			quit = 0;
		}

		if (isalpha(c) != 0) {
			alpha_count++;
		} else if (c == ' ') {
	       		if (saved_c != ' ') {
				word_count++;
			}

			space_count++;
		} else if (ispunct(c) != 0) {
			punctuation_count++;
		} else if (isdigit(c) != 0) {
			numeric_count++;
		}

		saved_c = c;
	}

	word_count += line_count - 1;

	printf("Text Statistics:\nAlphanumeric Characters: %d\nPunctuation Characters (without SPACE): %d\nSPACES: %d\nNumeric Characters: %d\nWords: %d\nLines: %d\n", alpha_count, punctuation_count, space_count, numeric_count, word_count, line_count);

	return EXIT_SUCCESS;
}
