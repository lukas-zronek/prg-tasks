/**
 * @file lib.h
 * @author Lukas Zronek
 * @date 26.01.2017
 * @brief M3U Playlist Editor
 * @version 1.0
 *
 */

#ifndef _M3UEDIT_LIB_H_
#define _M3UEDIT_LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <read_id3.h>
#include <unistd.h>
#include <util.h>

#define MAX_ID3_FIELD_LENGTH 256
#define MAX_FILENAME_LENGTH 256
#define MAX_INPUT_LENGTH 256

typedef struct m3u_data_s {
	char *filename;
        char *artist;
        char *title;
        int length;
        struct m3u_data_s *next;
} m3u_data;

typedef struct m3u_file_s {
	char *filename;
        m3u_data *head;
        m3u_data *tail;
	int count;
} m3u_file;

m3u_file *create_m3u_file(char *filename);
int parse_m3u_file(m3u_file *m3u_file);
int add_mp3_files(m3u_file *m3u_file, char **mp3_files, int mp3_files_length);
int add_m3u_data(m3u_file *m3u_file, char *filename, char *artist, char *title, int length);
int write_m3u_file(m3u_file *m3u_file);
int read_id3(m3u_file *m3u_file, char *filename);
void free_sll(m3u_data **head);
int add_sll(m3u_data *element, m3u_data **head, m3u_data **tail);
void print_sll(m3u_data *head);
int interactive(m3u_file *m3u_file);
void search_sll(m3u_file *m3u_file, char *search_word);
int delete_mp3_file(m3u_file *m3u_file, int track);
int shift_mp3_file(m3u_file *m3u_file, int track, char option);
int is_mp3_file_without_tags(char *filename);
void print_m3u_data(m3u_data *d, int number);

#endif
