/*
 * @file matrix_thread.c
 * @author Lukas Zronek
 * @brief Program that multiplies two n x n matrices using multiple threads
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include "config.h"

#define ARRAY(var, row, column) *(var + (row * MATRIX_COLUMN_LENGTH) + column)
#define THREAD_ERROR(error, msg) do { errno = error; perror(msg); return EXIT_FAILURE; } while (0)

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
	struct timeval start, end;
	double diff;

	pthread_t thread[64];
	int tr;

	int x = 0;

	if (gettimeofday(&start, NULL) != 0) {
		perror("gettimeofday");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < MATRIX_COLUMN_LENGTH; i++) {
		for (int j = 0; j < MATRIX_COLUMN_LENGTH; j++) {
			args[x].a = a;
			args[x].b = b;
			args[x].n = MATRIX_COLUMN_LENGTH;
			args[x].row = i;
			args[x].column = j;
			tr = pthread_create(&thread[x], NULL, calc_matrix, (void *)&args[x]);
			if (tr != 0) {
				THREAD_ERROR(tr, "pthread_create");
			}
			x++;
		 }
	}

	for (int i = 0; i < 64; i++) {
		tr = pthread_join(thread[i], NULL);
		if (tr != 0) {
			THREAD_ERROR(tr, "pthread_join");
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
