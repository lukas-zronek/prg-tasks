/*
 * @file vowels.c
 * @author Lukas Zronek
 * @brief determines how many vowels (A, E, I, O, U) were entered by the user.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
	int vowel_count = 0;
	char c = 0;
	int quit = 0;

	printf("Enter your text (terminate with ENTER twice) >\n");

	while (quit < 2) {
		c = getchar();

		c = toupper(c);

		switch (c) {
			case '\n': quit++; break;
			case 'A':
			case 'E':
			case 'I':
			case 'O':
			case 'U': vowel_count++;
			default: quit = 0; break;
		}
	}

	printf("The text contained %d vowels.\n", vowel_count);

	return EXIT_SUCCESS;
}
