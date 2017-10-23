/*
 * @file pipes.c
 * @author Lukas Zronek
 * @brief generates a random number and checks whether the number is prime
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>

#define BUFFER_SIZE 10
#define PIPES_NONBLOCKING 1

int is_prime(int n);
int get_rand(int max);
int read_int(char *s);

int main(int argc, char **argv)
{
	pid_t pid = 0;
	int pipe_to_child[2] = {0};
	int pipe_to_parent[2] = {0};
	char buffer[BUFFER_SIZE] = {0};
	int n = 0;
	int r = 0;
	int error = 0;

	if (pipe(pipe_to_child) < 0) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	if (pipe(pipe_to_parent) < 0) {
		error = 1;
		goto cleanup_child;
	}

#if PIPES_NONBLOCKING
	if (fcntl(pipe_to_child[0], F_SETFL, fcntl (pipe_to_child[0], F_GETFL) | O_NONBLOCK) < 0) {
		perror ("fcntl");
		return EXIT_FAILURE;
	}

	if (fcntl (pipe_to_child[1], F_SETFL, fcntl (pipe_to_child[1], F_GETFL) | O_NONBLOCK) < 0) {
		perror ("fcntl");
		return EXIT_FAILURE;
	}
#endif

	pid = fork();

	if (pid < 0 ) {
		perror("fork");
		error = 1;
		goto cleanup;
	} else if (pid > 0) {
		int status = 0;

		n = get_rand(128);

		if (sprintf(buffer, "%d", n) < 0) {
			fprintf(stderr, "sprintf failed\n");
			error = 1;
			goto cleanup;
		}

		if (write(pipe_to_child[1], buffer, BUFFER_SIZE) != BUFFER_SIZE) {
			perror("write");
			error = 1;
			goto cleanup;
		}

		if (read(pipe_to_parent[0], buffer, BUFFER_SIZE) != BUFFER_SIZE) {
			perror("read");
			error = 1;
			goto cleanup;
		}

		r = read_int(buffer);

		if (r < 0) {
			error = 1;
			goto cleanup;
		}

		printf("%d is %sa prime\n", n, r ? "" : "not ");

		wait(&status);

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != EXIT_SUCCESS) {
				fprintf(stderr, "child failed\n");
				error = 1;
			}
		}
	} else {
		if (read(pipe_to_child[0], buffer, BUFFER_SIZE) != BUFFER_SIZE) {
				perror("read");
				error = 1;
				goto cleanup;
		}

		n = read_int(buffer);

		if (n < 0) {
			error = 1;
			goto cleanup;
		}

		if (sprintf(buffer, "%d", is_prime(n)) < 0) {
			fprintf(stderr, "sprintf failed\n");
			error = 1;
			goto cleanup;
		}

		if (write(pipe_to_parent[1], buffer, BUFFER_SIZE) != BUFFER_SIZE) {
			perror ("write");
			error = 1;
			goto cleanup;
		}
	}

cleanup:
	if (close(pipe_to_parent[0]) != 0) {
		fprintf(stderr, "close() failed\n");
		error = 1;
	}

	if (close(pipe_to_parent[1]) != 0) {
		fprintf(stderr, "close() failed\n");
		error = 1;
	}

cleanup_child:
	if (close(pipe_to_child[0]) != 0) {
		fprintf(stderr, "close() failed\n");
		error = 1;
	}

	if (close(pipe_to_child[1]) != 0) {
		fprintf(stderr, "close() failed\n");
		error = 1;
	}

	if (error) {
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}

int get_rand(int max)
{
	static int seeded = 0;

	if (max <= 0) {
		return 0;
	}

	if (seeded == 0) {
		struct timeval t1;
		gettimeofday(&t1, NULL);
		srand(t1.tv_usec * t1.tv_sec);
		seeded = 1;
	}

	return rand() % max;
}

/* checks if n is a prime number
 * returns 1 if true and 0 otherwise
 */
int is_prime(int n)
{
	int start = 2;
	int end = 0;

	if (n < start) {
		return 0;
	} else if (n == start) {
		return 1;
	}

	/* Trail division test */

	end = (int) sqrt((double) n);

	end++; /* always round up, for the sake of simplicity even .0 */

	for (int i = start; i <= end; i++) {
		if (n % i == 0) {
			return 0;
		}
	}

	return 1;
}

/* returns -1 on error */
int read_int(char *s)
{
	int n = 0;
	char *endptr = NULL;

	n = (int) strtod(s, &endptr);

	if (*endptr != '\0' || n < 0) {
		fprintf(stderr, "strtod failed\n");
		n = -1;
	}

	return n;
}