/*
 * @file config.h
 * @author Lukas Zronek
 * @date 21.03.2017
 * @brief Shared Memory and Semaphores Training Task
 * @version 1.0
 *
 */

#ifndef _lz_config_
#define _lz_config_

#define SEM_COUNT 2
#define SEM_CLIENT 0
#define SEM_SERVER 1

#define SHM_KEY 10001L
#define SEM_KEY 10001L

#define RANDOM_MIN -20
#define RANDOM_MAX 40
#define RANDOM_COUNT 128

#define DEBUG 1
#define SLEEP 1

#endif
