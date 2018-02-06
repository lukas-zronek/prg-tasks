/**
 * @file calculator.c
 * @author Lukas Zronek
 * @date 28.10.2016
 * @brief a user-friendly calculator (simple)
 * @version 1.0
 *
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

void print_usage(void);

int main(void)
{
	char *line = NULL;
	size_t line_length = 0;
	char *operand1 = NULL;
	char *operand2 = NULL;
	char *operator = NULL;
	double number1 = 0;
	double number2 = 0;
	double result = 0;

	printf("Press control-D to quit:\n");

	while (1) {
		printf("> ");

		errno = 0;

		/* if line != NULL getline() expands the buffer with realloc() */
		if (getline(&line, &line_length, stdin) != -1) {
			operand1 = strtok(line, " ");
			if (operand1 == NULL) {
				/* unreachable code:
				 * operand1 != NULL when line != NULL and line_length > 0 */
				assert(0);
			}

			operator = strtok(NULL, " ");
			if (operator == NULL) {
				print_usage();
				continue;
			}

			operand2 = strtok(NULL, "\n");
			if (operand2 == NULL) {
				print_usage();
				continue;
			}

			if (strcmp("Ans", operand1) == 0) {
				number1 = result;
			} else if (sscanf(operand1, "%lf", &number1) != 1) {
				print_usage();
				continue;
			}

			if (strcmp("Ans", operand2) == 0) {
				number2 = result;
			} else if (sscanf(operand2, "%lf", &number2) != 1) {
				print_usage();
				continue;
			}

			if (strcmp("+", operator) == 0) {
				result = number1 + number2;
			} else if (strcmp("-", operator) == 0) {
				result = number1 - number2;
			} else if (strcmp("*", operator) == 0) {
				result = number1 * number2;
			} else if (strcmp("/", operator) == 0) {
				if (number2 == 0) {
					fprintf(stderr, "ERROR: DIVISION BY ZERO (Operand 2)\n");
					continue;
				} else {
					result = number1 / number2;
				}
			} else if (strcmp("**", operator) == 0) {
				result = pow(number1, number2);
			} else if (strcmp("_/", operator) == 0) {
				if (number1 < 0) {
					fprintf(stderr, "ERROR: Operand 1 must be greater than zero\n");
					continue;
				} else if (number2 == 0) {
					fprintf(stderr, "ERROR: DIVISION BY ZERO (Operand 2)\n");
					continue;
				} else {
					result = pow(number1, 1 / number2);
				}
			} else if (strcmp("%", operator) == 0) {
				result = (number2 / 100) * number1;
			} else {
				fprintf(stderr, "ERROR: unknown operator\n");
				continue;
			}

			/* result = roundf(result * 100) / 100; */

			printf("Ans = %g\n", result);
		} else {
			if (errno > 0) {
				perror("ERROR: Could not read input");
			} else { /* probably EOF -> exit */
				break;
			}
		}
	}

	free(line);
	line = NULL;

	return EXIT_SUCCESS;
}

void print_usage()
{
	printf("Usage: <Number 1> <operator> <Number 2>\n");
}
