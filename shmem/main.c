/*
 * @file main.c
 * @author Lukas Zronek
 * @date 21.03.2017
 * @brief Shared Memory and Semaphores Training Task
 * @version 1.0
 *
 */

#include <stdlib.h>
#include <stdio.h> 
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "lib.h"
#include "config.h"

int cleanup_shm(void);
void sigint_handler(int sig);
int compare(const void *a, const void *b);
int *shm_ptr = NULL;

int main(int argc, char **argv)
{
	enum m { client, server, cleanup, usage } mode;
	int sem_id = 0;
	int sem_default_values[SEM_COUNT] = {SEM_OPEN, SEM_CLOSED};
	int shm_id = 0;
	int ret = EXIT_SUCCESS;

	mode = usage;

	if (argc == 2) {
		if (strcmp(argv[1], "client") == 0) {
			mode = client;
		} else if (strcmp(argv[1], "server") == 0) {
			mode = server;
		} else if (strcmp(argv[1], "cleanup") == 0) {
			mode = cleanup;
		}
	}

	if (mode == usage) {
		fprintf(stderr, "Usage: %s client | server | cleanup\n\n\tclient  ... start as client\n\tserver  ... start as server\n\tcleanup ... delete semaphore and shared memory\n\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal");
		return EXIT_FAILURE;
	}

	shm_id = shm_init(SHM_KEY, RANDOM_COUNT * sizeof (int), (void **)&shm_ptr);

	if (shm_id == -1 || shm_ptr == NULL) {
		return EXIT_FAILURE;
	}

	sem_id = sem_init(SEM_KEY, SEM_COUNT, sem_default_values);

	if (sem_id == -1) {
		cleanup_shm();
		return EXIT_FAILURE;
	}

	if (mode == cleanup) {
		printf("delete semaphore set with id: %d\n", sem_id);

		if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
			perror("semctl");
			ret = EXIT_FAILURE;
		}

		printf("delete shared memory with id: %d\n", shm_id);

		if (cleanup_shm() == 0) {
			if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
				perror("shmctl");
				ret = EXIT_FAILURE;
			}
		} else {
			ret = EXIT_FAILURE;
		}
	} else if (mode == client) {
		while (1) {
			if (sem_set(sem_id, SEM_CLIENT, SEM_LOCK) == -1) {
				cleanup_shm();
				return EXIT_FAILURE;
			}

			randomize_array(shm_ptr, RANDOM_COUNT, RANDOM_MIN, RANDOM_MAX);

			#if DEBUG
			print_array(shm_ptr, RANDOM_COUNT);
			#endif
		
			#if SLEEP
			sleep(1); /* don't burn cpu cycles */
			#endif

			if (sem_set(sem_id, SEM_SERVER, SEM_UNLOCK) == -1) {
				cleanup_shm();
				return EXIT_FAILURE;
			}
		}
	} else {
		int n = 0;

		while (1) {
			if (sem_set(sem_id, SEM_SERVER, SEM_LOCK) == -1) {
				cleanup_shm();
				return EXIT_FAILURE;
			}

			qsort(shm_ptr, RANDOM_COUNT, sizeof (int), compare);

			#if DEBUG
			print_array(shm_ptr, RANDOM_COUNT);
			#endif

			printf("%4d | Min: %3d | Max: %3d | Median: %4.1f | Mean: %5.2f\n",
				n,
				shm_ptr[0],
				shm_ptr[RANDOM_COUNT - 1],
				calc_median(shm_ptr, RANDOM_COUNT),
				calc_mean(shm_ptr, RANDOM_COUNT)
			);

			n++;

			#if SLEEP
			sleep(1); /* don't burn cpu cycles */
			#endif

			if (sem_set(sem_id, SEM_CLIENT, SEM_UNLOCK) == -1) {
				cleanup_shm();
				return EXIT_FAILURE;
			}
		}		
	}

	return ret;
}

/* Note: uses global variale shm_ptr */
int cleanup_shm(void)
{
	if (shm_ptr != NULL && shmdt(shm_ptr) == -1) {
		perror("shmdt");
		return 1;
	}

	return 0;
}

void sigint_handler(int sig)
{
	int ret = EXIT_SUCCESS;

	if (signal(sig, SIG_IGN) == SIG_ERR) {
		perror("signal");
		ret = EXIT_FAILURE;
	}

	if (cleanup_shm() != 0) {
		ret = EXIT_FAILURE;
	}

	printf("Quit.\n");

	exit(ret);
}

int compare(const void *a, const void *b)
{
   return *(int *)a - *(int *)b;
}
