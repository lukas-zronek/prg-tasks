/**
 * @file util.h
 * @author Lukas Zronek
 * @date 26.01.2017
 * @brief M3U Playlist Editor
 * @version 1.0
 *
 */

#ifndef _M3UEDIT_UTIL_H_
#define _M3UEDIT_UTIL_H_

#define FREE(val) free(val); val = NULL;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <read_id3.h>
#include <lib.h>

char *trim(char *str, char c);
char *copy_string(char *src);
char *read_line(char *prompt, int buffer_size);
int read_int(char *prompt, int *n, int buffer_size);
char *format_length(int n);

#endif
