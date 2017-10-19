/*
 * @file main.c
 * @author Lukas Zronek
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "dec2bin.h"

int main(int argc, char *argv[])
{
	int n = 0;
	char *bin_string = NULL;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s INTEGER\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (sscanf(argv[1], "%d", &n) != 1) {
		fprintf(stderr, "wrong input\n");
		return EXIT_FAILURE;
	}

	if (n < 0) {
		fprintf(stderr, "n >= 0\n");
		return EXIT_FAILURE;
	}

	bin_string = dec2bin(n, 0);

	if (bin_string != NULL) {
		printf("%d = %s\n", n, bin_string);

		free(bin_string);
		bin_string = NULL;
	}

	return EXIT_SUCCESS;
}
