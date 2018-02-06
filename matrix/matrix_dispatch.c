/*
 * @file matrix_dispatch.c
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

struct calc_args {
	const int *a;
	const int *b;
	int n;
	int row;
	int column;
};

int c[64] = {0};

typedef struct calc_args Args;

void *calc_matrix(void *ptr);

int main(void)
{
	Args args[64];
	struct timeval start;
	dispatch_queue_t queue;

	int x = 0;

	if (gettimeofday(&start, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}

	queue = dispatch_queue_create("Matrix", NULL);

	for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
		for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
			args[x].a = a;
			args[x].b = b;
			args[x].n = MATRIX_COLUMN_LENGTH;
			args[x].row = i;
			args[x].column = j;
			Args *a = args;
			dispatch_async(queue, ^{ calc_matrix(&a[x]); });
			x++;
		 }
	}

	dispatch_sync(queue, ^{
		struct timeval end;
		double diff;

		if (gettimeofday(&end, NULL) != 0) {
			perror("gettimeofday");
			exit(EXIT_FAILURE);
		}

		diff = (end.tv_sec + end.tv_usec * 0.000001) - (start.tv_sec + start.tv_usec * 0.000001);
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
	int sum = 0;
	Args *args = (Args *)ptr;

	for (int i = 0; i < args->n; i++) {
		sum += ARRAY(args->a, args->row, i) * ARRAY(args->b, i, args->column);
		#if DEBUG
		printf("[%d][%d] * [%d][%d]\n", args->row, i, i, args->column);
		#endif
	}

	#if DEBUG
	printf("sum: %d\n", sum);
	#endif

	ARRAY(c, args->row, args->column) = sum;

	return NULL;
}
