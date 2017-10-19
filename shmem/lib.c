/*
 * @file lib.c
 * @author Lukas Zronek
 * @date 21.03.2017
 * @brief Shared Memory and Semaphores Training Task
 * @version 1.0
 *
 */

#include "lib.h"

/*
 * Gets an existing shared memory or creates a new one
 * return: -1 on error
 */
int shm_init(key_t key, size_t size, void **ptr)
{
	int shm_id = 0;

	shm_id = shmget(key, size, IPC_CREAT | SHM_PERMISSIONS);

	if (shm_id == -1) {
		perror("shmget");
		*ptr = NULL;
		return -1;
	}

	*ptr = shmat(shm_id, NULL, 0);

	if (*ptr == (void *) -1) {
		perror("shmat");
		*ptr = NULL;
		return -1;
	}

	return shm_id;
}

/*
 * Gets an existing semaphore or creates a new one
 * return: -1 on error or id of semaphore on success
 */
int sem_init(key_t key, int count, int default_values[])
{
	int semid = 0;
	union semun su;

	/* look for existing semaphore */
	semid = semget(key, count, 0);

	if (semid == -1) {
		/* create new semaphore */
		semid = semget(key, count, IPC_CREAT | SEM_PERMISSIONS);

		if (semid == -1) {
			perror("semget");
			return -1;
		}

		for (int i = 0; i < count; i++) {
			su.val = default_values[i];

			if (semctl(semid, i, SETVAL, su)) {
				perror("semctl");
				return -1;
			}
		}
	}

	return semid;
}

/*
 * set value of semaphore
 * return: -1 on error, 0 on success
 */
int sem_set(int semid, int num, int value)
{
	struct sembuf sb;

	sb.sem_num = num;
	sb.sem_op = value;
	sb.sem_flg = SEM_UNDO;

	if (semop(semid, &sb, 1) == -1) {
		perror("semop");
		return -1;
	}

	return 0;
}

/*
 * generates a random number between min and max, max should be greater than min
 * random number generator is seeded on first use
 */
int random_int(int min, int max)
{
        static int seeded = 0;

	if (max < min) {
		assert(0);
	}

        if (seeded == 0) {
		srand(time(NULL));
                seeded = 1;
        }

        return (rand() % (++max - min)) + min;
}

/*
 * fills array with random numbers
 */
void randomize_array(int *array, int size, int min, int max)
{
	for (int i = 0; i < size; i++) {
		array[i] = random_int(min, max);
	}
}
/*
 * Calculates mean of integer array
 */
float calc_mean(int *array, int size)
{
	int sum = 0;

	if (size == 0) {
		assert(0);
	}

	for (int i = 0; i < size; i++) {
		sum += array[i];
	}

	return sum / (float)size;
}

float calc_median(int *array, int size)
{
	if (size % 2 == 0) {
		/* calculate mean of two elements in the middle */
		return((array[size / 2] + array[size / 2 - 1]) / 2.0);
	} else {
		/* return the element in the middle */
		return array[size / 2];
	}
}

/*
 * print content of int array 
 */
void print_array(int *array, int size)
{
	for (int i = 0; i < size; i++) {
		printf("%d ", array[i]);
	}

	printf("\n");
}
