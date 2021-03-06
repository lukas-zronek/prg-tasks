/*
 * @file area.c
 * @author Lukas Zronek
 * @brief calculates the area of a cirlce, square, or triangle.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 256

double rectangle(void);
double triangle(void);
double square(void);
double circle(void);
char input_char(void);
double *input_multiple_double(size_t, char *);

char buffer[MAX_INPUT] = {0};

int main(void)
{
	char type = 0;
	char quit = 0;
	double result = 0;

	while (quit != 'Y') {
		quit = 0;

		printf("Calculate the area of a (R)ectangle, (T)riangle, (S)quare, (C)ircle > ");

		type = input_char();

		switch (toupper(type)) {
			case '\0': return EXIT_FAILURE;
			case 'R': result = rectangle(); break;
			case 'T': result = triangle(); break;
			case 'S': result = square(); break;
			case 'C': result = circle(); break;
			default: continue;
		}

		printf("The area is: %g\n", result);

		while (quit != 'Y' && quit != 'N') {
			printf("Do you want to quit ? (Y/N) > ");
			quit = toupper(input_char());

			if (quit == '\0') {
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

double rectangle()
{
	double *input = NULL;
	double a, b;
	double area = 0.0;

	input = input_multiple_double(2, "two sides");

	if (input != NULL) {
		a = input[0];
		b = input[1];
		free(input);
		input = NULL;

		area = a * b;
	}

	return area;
}

double triangle()
{
	double *input = NULL;
	double a, b, c;
	double area = 0.0;

	while (1) {
		input = input_multiple_double(3, "three sides");

		if (input != NULL) {
			a = input[0];
			b = input[1];
			c = input[2];
			free(input);
			input = NULL;

			if ( c >= a + b || b >= a + c || a >= c + b) {
				printf("Error: One side of the triangle must not be longer than the sum of the two other sides!\n");
			} else {
				area = sqrt((a+b+c)*(a+b-c)*(b+c-a)*(c+a-b)) / 4.0;
				break;
			}
		}
	}

	return area;
}

double square()
{
	double *input = NULL;
	double a;
	double area = 0.0;

	input = input_multiple_double(1, "one side");

	if (input != NULL) {
		a = input[0];
		free(input);
		input = NULL;

		area = a * a;
	}

	return area;
}

double circle()
{
	double *input = NULL;
	double r;
	double area = 0.0;

	input = input_multiple_double(1, "radius");

	if (input != NULL) {
		r = input[0];
		free(input);
		input = NULL;

		area = pow(r, 2.0) * M_PI;
	}
	
	return area;
}

/*
 * returns NUL on error
 */
char input_char()
{
	char c = '\0';

	if (fgets(buffer, MAX_INPUT, stdin) != NULL) {
		if (sscanf(buffer, "%c", &c) != 1) {
			return EOF;
		}
	} else {
		if (feof(stdin)) {
			c = EOF;
		} else if (ferror(stdin)) {
			perror("Error: fgets failed");
		}
		clearerr(stdin);
	}

	return c;
}

double *input_multiple_double(size_t count, char *prompt)
{
	char *token = NULL;
	double *result = NULL;
	double d = 0;
	int i = 0;

	if (count == 0) {
		return NULL;
	}

	result = calloc(count, sizeof (double));

	if (result == NULL) {
		fprintf(stderr, "Allocating memory failed\n");
		return NULL;
	}

	while (1) {
		printf("Specify %s > ", prompt);

		if (fgets(buffer, MAX_INPUT, stdin) != NULL) {
			token = buffer;
			for (i = 0; i < count; i++) {
				token = strtok(token, " ");
				if (token != NULL) {
					if (sscanf(token, "%lf", &d) == 1) {
						if (d <= 0) {
							printf("Error: input > 0!\n");
							break;
						}
						result[i] = d;
					}
					token = NULL;
				} else {
					break;
				}
			}

			if (count == i) {
				break;
			}
		} else {
			if (ferror(stdin)) {
				perror("Error: fgets failed");
			}
			clearerr(stdin);
		}
	}

	return result;
}
