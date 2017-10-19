/*
 * @file main.c
 * @author Lukas Zronek
 * @brief semaphore demo
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

#include <unistd.h>

int main(void)
{
	int semid = 0;

	semid = create_sem("/etc/hosts", 1);

	if (semid == -1) {
		return EXIT_FAILURE;
	}

	if (set_sem(semid, SEM_LOCK) == -1) {
		return EXIT_FAILURE;
	}

	printf("start\n");
	sleep(10);
	printf("end\n");

	if (set_sem(semid, SEM_UNLOCK) == -1) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
