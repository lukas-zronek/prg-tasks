/*
 * @file lib.c
 * @author Lukas Zronek
 * @brief semaphore demo
 *
 */

#include "lib.h"

/* return -1 on error */
int create_sem(char *keyfile, int default_value)
{
	int semid = 0;
	key_t key = 0;
	union semun su;

	key = ftok(keyfile, 'b');

	if (key == -1) {
		fprintf(stderr, "Error: ftok failed.\n");
		return -1;
	}

	/* get existing semaphore */
	semid = semget(key, 1, 0);

	if (semid == -1) {
		/* create new semaphore */
		semid = semget(key, 1, IPC_CREAT | 0666);

		if (semid == -1) {
			perror("Error: semget");
			return -1;
		}

		su.val = default_value;
		semctl(semid, 0, SETVAL, su);
	}

	return semid;
}

int set_sem(int semid, int n)
{
	struct sembuf sb;

	sb.sem_num = 0;
	sb.sem_op = n;
	sb.sem_flg = SEM_UNDO;

	if (semop(semid, &sb, 1) == -1) {
		perror("Error: semop");
		return -1;
	}

	return 0;
}
