/*
 * @file dec2bin.c
 * @author Lukas Zronek
 *
 */

#include "dec2bin.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char *dec2bin(int d, unsigned int digits)
{
	int i = 0;
	int j = 0;
	char *s = NULL;

	if (digits == 0) {
		digits = log10(d) / log10(2) + 1;
	}

	s = malloc(digits + 1);

	if (s == NULL) {
		fprintf(stderr, "malloc failed\n");
		return NULL;
	}

	for (i = digits - 1; i >= 0; i--) {
		j = digits - 1 - i;
		if (d & (1 << i)) {
			s[j] = '1';
		} else {
			s[j] = '0';
		}
	}

	s[i] = '\0';

	return s;
}
