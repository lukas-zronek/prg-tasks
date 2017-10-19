/*
 * @file matrix.c
 * @author Lukas Zronek
 * @brief Program that multiplies two n x n matrices
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "config.h"

#define ARRAY(var, row, column) *(var + (row * MATRIX_COLUMN_LENGTH) + column)

struct calc_args {
	const int *a;
	const int *b;
	int n;
	int row;
	int column;
};

typedef struct calc_args Args;

int calc_matrix(Args *args);

int main(void)
{
	int c[64] = {0};
	struct timeval start, end;
	double diff;
	Args args;

	args.a = a;
	args.b = b;
	args.n = MATRIX_COLUMN_LENGTH;

	if (gettimeofday(&start, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
		for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
			args.row = i;
			args.column = j;
			ARRAY(c, i, j) = calc_matrix(&args);
		}
	}

	if (gettimeofday(&end, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}
	diff = (end.tv_sec + end.tv_usec * 0.000001) - (start.tv_sec + start.tv_usec * 0.000001);
	printf("calculation took %f sec\n", diff);

	for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
		for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
			printf("%d ", ARRAY(c, i, j));
		}
		printf("\n");
	}

	return 0;
}

int calc_matrix(Args *args)
{
	int sum = 0;

	for (int i = 0; i < args->n; i++) {
		sum += *(args->a + (args->row * args->n) + i)  * *(args->b + (i * args->n) + args->column);
		#if DEBUG
		printf("[%d][%d] * [%d][%d]\n", args->row, i, i, args->column);
		#endif
	}

	#if DEBUG
	printf("sum: %d\n", sum);
	#endif

	return sum;
}
