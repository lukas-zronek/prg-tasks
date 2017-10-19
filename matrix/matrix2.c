/*
 * @file matrix2.c
 * @author Lukas Zronek
 * @brief Program that multiplies two n x n matrices
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "config.h"

#define ARRAY(var, row, column) *(var + (row * MATRIX_COLUMN_LENGTH) + column)

int c[64] = {0};

struct calc_args {
	int start[2];
	int end[2];
};
typedef struct calc_args Args;

void calc_matrix(Args args);

int main(void)
{
	Args args;
	args.start[0] = 0;
	args.start[1] = 0;
	args.end[0] = 7;
	args.end[1] = 7;

	struct timeval start_time, end_time;
	double diff;

	if (gettimeofday(&start_time, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}

	calc_matrix(args);

	if (gettimeofday(&end_time, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}
	diff = (end_time.tv_sec + end_time.tv_usec * 0.000001) - (start_time.tv_sec + start_time.tv_usec * 0.000001);
	printf("calculation took %f sec\n", diff);

	for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
		for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
			printf("%d ", ARRAY(c, i, j));
		}
		printf("\n");
	}

	return EXIT_SUCCESS;
}

void calc_matrix(Args args)
{
	int sum = 0;

	for (int row = args.start[0]; row <= args.end[0]; row++) {
		for (int column = args.start[1]; column <= args.end[1]; column++) {
			for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
				sum += ARRAY(a, row, i) * ARRAY(b, i, column);
				#if DEBUG
				printf("[%d][%d] * [%d][%d]\n", row, i, i, column);
				#endif
			}
			#if DEBUG
			printf("sum: %d\n", sum);
			#endif
			ARRAY(c, row, column) = sum;
			sum = 0;
		}
	}
}
