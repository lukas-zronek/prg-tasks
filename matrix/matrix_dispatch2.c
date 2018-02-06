/*
 * @file matrix_dispatch2.c
 * @author Lukas Zronek
 * @brief Program that multiplies two n x n matrices using libdispatch
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dispatch/dispatch.h>
#include "config.h"

#define ARRAY(var, row, column) *(var + (row * MATRIX_COLUMN_LENGTH) + column)

int c[64] = {0};

struct calc_args {
	int start[2];
	int end[2];
};
typedef struct calc_args Args;

void *calc_matrix(void *ptr);

int main(void)
{
	Args args[2];
	Args *a = args;

	struct timeval start_time;

	dispatch_queue_t queue;

	args[0].start[0] = 0;
	args[0].start[1] = 0;
	args[0].end[0] = 3;
	args[0].end[1] = 7;
	args[1].start[0] = 4;
	args[1].start[1] = 0;
	args[1].end[0] = 7;
	args[1].end[1] = 7;

	if (gettimeofday(&start_time, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}

	queue = dispatch_queue_create("Matrix", NULL);

	dispatch_async(queue, ^{ calc_matrix(&a[0]); });
	dispatch_async(queue, ^{ calc_matrix(&a[1]); });

	dispatch_sync(queue, ^{
		struct timeval end;
		double diff;

		if (gettimeofday(&end, NULL) != 0) {
			perror("gettimeofday");
			exit(EXIT_FAILURE);
		}

		diff = (end.tv_sec + end.tv_usec * 0.000001) - (start_time.tv_sec + start_time.tv_usec * 0.000001);
		printf("calculation took %f sec\n", diff);

		for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
			for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
				printf("%d ", ARRAY(c, i, j));
			}
			printf("\n");
		}

		exit(EXIT_SUCCESS);
	});

	/* NOTREACHED */
	return EXIT_SUCCESS;
}

void *calc_matrix(void *ptr)
{
	Args *args = (Args *)ptr;
	int sum = 0;

	for (int row = args->start[0]; row <= args->end[0]; row++) {
		for (int column = args->start[1]; column <= args->end[1]; column++) {
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

	return NULL;
}
