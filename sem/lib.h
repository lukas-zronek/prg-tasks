/*
 * @file lib.h
 * @author Lukas Zronek
 * @brief semaphore demo
 *
 */

#include <stdio.h>
#include <sys/sem.h>

#if 0
union semun {
	int val; /* value for SETVAL */
	struct semid_ds *buf; /* buffer for IPC_STAT & IPC_SET */
	u_short *array; /* array for GETALL & SETALL */
};
#endif

#define SEM_LOCK -1
#define SEM_UNLOCK 1

int create_sem(char *keyfile, int default_value);
int set_sem(int semid, int n);
