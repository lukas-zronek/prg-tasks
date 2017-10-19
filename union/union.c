/*
 * @file union.c
 * @author Lukas Zronek
 * @brief union demo
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT 0
#define DOUBLE 1
#define STRING 2
#define ARRAY 3

union myun {
	int i;
	double d;
	char s[6];
	int a[10];
};

union strun {
	char a;
	char b;
	char c;
	char s[2];
};

void myfunc(int type, union myun un);

int main(void)
{
	union myun un;

	printf("union: %lu Bytes\n", sizeof (un));
	printf("i: %lu Bytes\n", sizeof (un.i));
	printf("d: %lu Bytes\n", sizeof (un.d));
	printf("s: %lu Bytes\n", sizeof (un.s));
	printf("a: %lu Bytes\n\n", sizeof (un.a));

	un.i = 7;

	myfunc(INT, un);

	un.d = 1.5;

	myfunc(DOUBLE, un);

	if (strncpy(un.s, "Hello", 6) != un.s) {
		fprintf(stderr, "Error: strncpy failed.\n");
		return EXIT_FAILURE;
	}

	myfunc(STRING, un);

	for (int i = 0; i < 10; i++) {
		un.a[i] = i;
	}

	myfunc(ARRAY, un);

	union strun mystring;

	mystring.s[0] = 'S';
	mystring.s[1] = '\0';
	mystring.a = 'A';
	mystring.b = 'B';
	mystring.c = 'C';

	printf("\nstring: %s\n", mystring.s);
	printf("a: %c\n", mystring.a);
	printf("b: %c\n", mystring.b);

	return EXIT_SUCCESS;
}

void myfunc(int type, union myun un)
{
	switch (type) {
		case INT:
			printf("Integer: %d\n", un.i);
			break;
		case DOUBLE:
			printf("Double: %g\n", un.d);
			break;
		case STRING:
			printf("String: %s\n", un.s);
			break;
		case ARRAY:
			printf("Array: ");
			for (int i = 0; i < sizeof (un.a) / sizeof (int); i++) {
				printf("%d ", un.a[i]);
			}
			printf("\n");
			break;
		default:
			fprintf(stderr, "Error: unknown type\n");
			break;
	}
}
