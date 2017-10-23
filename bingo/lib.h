/*
 * @file lib.h
 * @author Lukas Zronek
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <limits.h>
#include <sys/time.h>

#define MTYPE 42
#define MSGMAX 1024 /* must be greater than 0 */
#define SEM_PERM 0600
#define MSG_ID_FILE "./keyfile"

#define FREE(val) if (val != NULL) { free(val); val = NULL; }

typedef struct msg_s {
	long mtype;
	char mtext[MSGMAX];
} msg;

long int create_queue(char *);
int delete_queue(long int);
int send_message(long int, long int, char *);
char *receive_message(long int, long int);
int send_int(long int, long int, int);
char *receive_message(long int, long int);
int receive_int(long int, long int, int *);

char *read_line(char *, int);
int read_int(char *, int *, int);
char *copy_string(char *);
int get_rand(int);
