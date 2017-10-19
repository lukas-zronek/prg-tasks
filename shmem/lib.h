/*
 * @file lib.h
 * @author Lukas Zronek
 * @date 21.03.2017
 * @brief Shared Memory and Semaphores Training Task
 * @version 1.0
 *
 */

#ifndef _lz_lib_
#define _lz_lib_

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <assert.h>

#define SEM_OPEN 1
#define SEM_CLOSED 0

#define SEM_LOCK -1
#define SEM_UNLOCK 1

#define SEM_PERMISSIONS 0666
#define SHM_PERMISSIONS 0666

#if (defined(__GNU_LIBRARY__) && defined(_SEM_SEMUN_UNDEFINED)) || !defined(__FreeBSD__)
/* under GNU/Linux semun is usually not defined */
union semun {
     int val; /* value for SETVAL */
     struct semid_ds *buf; /* buffer for IPC_STAT & IPC_SET */
     u_short *array; /* array for GETALL & SETALL */
};
#endif

int shm_init(key_t key, size_t size, void **ptr);
int sem_init(key_t key, int count, int default_values[]);
int sem_set(int semid, int num, int value);

int random_int(int min, int max);
void randomize_array(int *array, int size, int min, int max);
float calc_mean(int *array, int size);
float calc_median(int *array, int size);
void print_array(int *array, int size);

#endif
