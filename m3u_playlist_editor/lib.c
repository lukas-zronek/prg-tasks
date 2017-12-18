/*
 * @file lib.c
 * @author Lukas Zronek
 * @date 26.01.2017
 * @brief M3U Playlist Editor
 * @version 1.0
 *
 */

#include <lib.h>

/*
 * creates an instance of the m3u_file structure and copies the filename into it
 * returns: m3u_file or NULL on error
 */
m3u_file *create_m3u_file(char *filename)
{
	m3u_file *f = NULL;

	f = (m3u_file *)malloc(sizeof (m3u_file));

	if (f == NULL) {
		fprintf(stderr, "Error: Allocating m3u_file structure failed.\n");
	} else {
		f->head = NULL;
		f->tail = NULL;
		f->count = 0;

		/* copy to heap */
		f->filename = copy_string(filename);

		if (f->filename == NULL) {
			free(f);
			f = NULL;
			return NULL;
		}
	}

	return f;
}

/*
 * adds an array of mp3 files to linked list
 * returns: 0 on success or 1 on error
 */
int add_mp3_files(m3u_file *m3u_file, char **mp3_files, int mp3_files_length)
{
	int i = 0;
	char *filename = NULL;

	for (i = 0; i < mp3_files_length; i++) {
		/* copy to heap */
		filename = copy_string(*(mp3_files + i));

		if (filename == NULL) {
			return 1;
		}

		if (read_id3(m3u_file, filename) != 0) {
			free(filename);
			filename = NULL;
			return 1;
		}
	}

	return 0;
}

/*
 * adds a single linked list to the bottom of an existing linked list and updates head and tail pointers
 * returns: 0 on success or 1 on error
 */
int add_sll(m3u_data *element, m3u_data **head, m3u_data **tail)
{
	m3u_data *t = NULL;

	if (element == NULL) {
		return 1;
	}

	if (*head == NULL) {
		*head = element;
	}

	if (*tail == NULL) {
		*tail = element;
		return 0;
	} else {
		t = *tail;

		if (t->next == NULL) {
			t->next = element;
			*tail = element;
		} else {
			fprintf(stderr, "Error: tail is not last element of sll.\n");
			return 1;
		}
	}

	return 0;
}

/*
 * parses the id3 tags of a mp3 file
 * returns: 0 on success or 1 on error
 */
int read_id3(m3u_file *m3u_file, char *filename)
{
	FILE *file = NULL;
	char buffer[MAX_ID3_FIELD_LENGTH] = {0};
	char *str = NULL;
	size_t str_length = 0;
	char *str_endptr = NULL;
	int error = 0;
	/* int id3_tags[] = {ARTIST_ID3, TITLE_ID3, LENGTH_ID3}; */
	int id3_tags[] = {ARTIST_ID3, TITLE_ID3};
	int length = 0;
	char *artist = NULL;
	char *title = NULL;

	if (m3u_file == NULL || filename == NULL) {
		return 0;
	}

	file = fopen(filename, "r");

	if (file == NULL) {
		perror("Could not open mp3 file");
		return 1;
	}

	for (int i = 0; i < (sizeof (id3_tags) / sizeof (int)); i++) {
		if (read_ID3_info(id3_tags[i], buffer, sizeof (buffer), file) == 1) {
			str_length = strlen(buffer);

			str = (char *)malloc(str_length + 1);

			if (str == NULL) {
				fprintf(stderr, "malloc failed.\n");
				error = 1;
				break;
			}

			if (strncpy(str, buffer, str_length + 1) != str) {
				fprintf(stderr, "strncpy failed.\n");
				error = 1;
				break;
			}
		} else {
			continue;
		}

		switch (id3_tags[i]) {
			case ARTIST_ID3:
				artist = str;
				str = NULL;
				break;
			case TITLE_ID3:
				title = str;
				str = NULL;
				break;
			case LENGTH_ID3:
				length = (int)strtod(str, &str_endptr);

				if (*str_endptr != '\0' || length < 0) {
					length = 0;
				}
				free(str);
				str = NULL;
				break;
			default:
				assert(1);
				/* error = 1; */
				break;
		}
	}

	if (file != NULL && fclose(file) != 0) {
		perror("fclose");
		error = 1;
	}

	if (error != 1 && add_m3u_data(m3u_file, filename, artist, title, length) == 0) {
		printf("Added track \'%s\'\n", filename);
		print_m3u_data(m3u_file->tail, m3u_file->count - 1);
	} else {
		error = 1;
	}

	if (error == 1) {
		free(str);
		str = NULL;
		free(artist);
		artist = NULL;
		free(title);
		title = NULL;
	}

	return error;
}

/*
 * frees all elements of a single linked list and all variables of each element
 */
void free_sll(m3u_data **head)
{
        m3u_data *c = NULL;
        m3u_data *n = NULL;
        
        c = *head;

        while (c != NULL) {
		free(c->filename);
		c->filename = NULL;
		free(c->artist);
		c->artist = NULL;
		free(c->title);
		c->title = NULL;
		c->length = 0;
                n = c->next;
                free(c);
                c = n;
        }

        *head = NULL;
}

void print_sll(m3u_data *head)
{
	int count = 0;
	int total_length = 0;
	char *total_length_format = NULL;

	if (head == NULL) {
		return;
	}

	for (; head != NULL; head = head->next) {
		total_length += head->length;
		print_m3u_data(head, count);
		count++;
	}

	total_length_format = format_length(total_length);

	printf("Total Length: %s\n", total_length_format == NULL ? "00:00": total_length_format);

	free(total_length_format);
	total_length_format = NULL;
}

void print_m3u_data(m3u_data *d, int number)
{
	char *length_format = NULL;

	length_format = format_length(d->length);

	printf("[ %3d] %s %s -- %s\n", number, length_format == NULL ? "00:00" : length_format, d->artist, d->title);

	free(length_format);
	length_format = NULL;
}

/*
 * parses a m3u file 
 * returns: 0 on success, 1 when general error, 2 when parse error
 */
int parse_m3u_file(m3u_file *m3u_file)
{
	FILE *f = NULL;
	char buffer[MAX_ID3_FIELD_LENGTH] = {0};
	int error = 0;
	int length = 0;
	char *column = NULL;
	char *length_endptr = NULL;
	char *artist = NULL;
	char *title = NULL;
	char *filename = NULL;

	if (m3u_file == NULL || m3u_file->filename == NULL) {
		return 1;
	}

	f = fopen(m3u_file->filename, "r");

	if (f == NULL) {
		perror("Error: Could not open m3u file");
		return 1;
	}

	if (NULL == fgets(buffer, sizeof (buffer), f) || strcmp(buffer, "#EXTM3U\n") != 0) {
		error = 2;
	}

	while (error == 0) {
		if (NULL == fgets(buffer, sizeof (buffer), f)) {
			break;
		}

		if (strtok(buffer, ":") == NULL || strcmp(buffer, "#EXTINF")) {
			error = 2;
			break;
		}

		if ((column = strtok(NULL, ",")) == NULL) {
			error = 2;
			break;
		}

		length = (int)strtod(column, &length_endptr);

		if (*length_endptr != '\0' || length < 0) {
			error = 2;
			break;
		}

		if ((column = strtok(NULL, "-")) == NULL) {
			error = 2;
			break;
		}

		artist = copy_string(trim(column, ' '));

		if ((column = strtok(NULL, "\n")) == NULL) {
			error = 2;
			break;
		}

		title = copy_string(trim(column, ' '));

		if (NULL == fgets(buffer, sizeof (buffer), f)) {
			error = 2;
			break;
		}

		filename = copy_string(trim(buffer, '\n'));

		if (access(filename, F_OK) == -1) {
			fprintf(stderr, "Warning: mp3 file '%s' is missing.\n", filename);
			free(artist);
			artist = NULL;
			free(title);
			title = NULL;
			free(filename);
			filename = NULL;
		} else {
			if (add_m3u_data(m3u_file, filename, artist, title, length) != 0) {
				error = 1;
				break;
			}
		}
	}

	if (error == 2) {
		fprintf(stderr, "Error: Not a valid m3u file.\n");
	}

	if (f != NULL && fclose(f) != 0) {
		perror("fclose");
		error = 1;
	}

	if (error != 0) {
		free(artist);
		artist = NULL;
		free(title);
		title = NULL;
		free(filename);
		filename = NULL;
		length = 0;
	}

	return error;
}

/*
 * adds an element to sll
 * It prompts user if a valid mp3 file has no tags
 *
 */
int add_m3u_data(m3u_file *m3u_file, char *filename, char *artist, char *title, int length)
{
	char *input = NULL;
	m3u_data *d = NULL;

	if (m3u_file == NULL || filename == NULL) {
		return 1;
	}

	if (artist == NULL && title == NULL) {
		if (is_mp3_file_without_tags(filename) == 0) {
			fprintf(stderr, "Error: '%s' is not a valid mp3 file.\n", filename);
			return 1;
		}
		printf("Track '%s' contains no ID3 tag. Enter details:\n", filename);

		while (1) {
			input = read_line("Title        ", MAX_ID3_FIELD_LENGTH);

			if (input == NULL || *input == '\0') {
				free(input);
				input = NULL;
			} else {
				title = input;
				break;
			}
		}

		input = NULL;

		while (1) {
			input = read_line("Interpret    ", MAX_ID3_FIELD_LENGTH);

			if (input == NULL || *input == '\0') {
				free(input);
				input = NULL;
			} else {
				artist = input;
				break;
			}
		}

		input = NULL;

		while (read_int("Duration (s) ", &length, MAX_ID3_FIELD_LENGTH) == 1 || length < 0) {
		}
	}

	d = (m3u_data *)malloc(sizeof (m3u_data));

	if (d == NULL) {
		fprintf(stderr, "Error: Allocating m3u_data structure failed.\n");
		free(artist);
		artist = NULL;
		free(title);
		title = NULL;
		return 1;
	} else {
		d->filename = filename;
		d->artist = artist;
		d->title = title;
		d->next = NULL;
		d->length = length;
	}

	if (add_sll(d, &m3u_file->head, &m3u_file->tail) != 0) {
		free_sll(&d);
		return 1;
	} else {
		m3u_file->count++;
	}

	return 0;
}

/*
 * writes the content of sll to file
 * It prompts user if a file would be overwritten
 *
 */
int write_m3u_file(m3u_file *m3u_file)
{
	FILE *f = NULL;
	m3u_data *head = NULL;
	char *confirm = NULL;

	if (m3u_file == NULL || m3u_file->filename == NULL) {	
		return 0;
	}

	if (access(m3u_file->filename, F_OK) != -1) {
		while (1) {
			printf("Overwrite '%s'?", m3u_file->filename);
			confirm = read_line("", 3);

			if (confirm != NULL && *confirm != '\0') {
				*confirm = toupper(*confirm);

				if (*confirm == 'Y') {
					free(confirm);
					confirm = NULL;
					break;
				} else if (*confirm == 'N') {
					free(confirm);
					confirm = NULL;
					return 1;
				}
			}

			free(confirm);
			confirm = NULL;
		}
	}

	f = fopen(m3u_file->filename, "w");

	if (f == NULL) {
		perror("Error: Could not open m3u file for writing");
		return 1;
	}

	fprintf(f, "#EXTM3U\n");

	for (head = m3u_file->head; head != NULL; head = head->next) {
		if (fprintf(f, "#EXTINF:%d,%s - %s\n%s\n", head->length, head->artist, head->title, head->filename) < 0) {
			fprintf(stderr, "Error: Could not write to m3u file.\n");
			return 1;
		}
	}

	printf("Written to '%s'.\n", m3u_file->filename);

	return 0;
}

/*
 * implements the interactive editing mode
 *
 */
int interactive(m3u_file *m3u_file)
{
	int quit = 0;
	int error = 0;
	char *option = NULL;
	char *input = NULL;
	int track = -1;

	printf("\nEnter command below. Use 'h' for help.\n");

	while (quit == 0) {
		option = read_line("", 3);

		if (option != NULL) {
			switch (toupper(*option)) {
				case 'A':
					while (1) {
						input = read_line("Filename", MAX_FILENAME_LENGTH);

						if (input != NULL && *input != '\0' && add_mp3_files(m3u_file, &input, 1) == 0) {
							free(input);
							input = NULL;
							break;
						}

						free(input);
						input = NULL;
					}
					break;
				case 'D':
					track = -1;

					while (read_int("Track", &track, MAX_INPUT_LENGTH) == 1 || track < 0) {
					}

					if (delete_mp3_file(m3u_file, track) == 0) {
						printf("Deleted Track %d\n", track);
					} else {
						printf("Error: No such track\n");
					}
					
					break;
				case '+':
				case '-':
					track = -1;

					while (read_int("Track", &track, MAX_INPUT_LENGTH) == 1 || track < 0) {
					}

					if (shift_mp3_file(m3u_file, track, *option) != 0) {
						printf("Error: No such track\n");
					}
					break;
				case 'S':
					while (1) {
						input = read_line("Search word", MAX_ID3_FIELD_LENGTH);

						if (input != NULL && *input != '\0') {
							search_sll(m3u_file, input);
							free(input);
							input = NULL;
							break;
						}

						free(input);
						input = NULL;
					}

					break;
				case 'V':
					print_sll(m3u_file->head);
					break;
				case 'W':
					while (1) {
						printf("Filename [%s]", m3u_file->filename);

						input = read_line("", MAX_FILENAME_LENGTH);

						if (input != NULL) {
							break;
						}
					}

					if (*input != '\0') {
						free(m3u_file->filename);
						m3u_file->filename = input;
					} else {
						free(input);
						input = NULL;
					}

					write_m3u_file(m3u_file);

					break;
				case 'X':
					quit = 1;
					break;
				case 'H':
					printf("\na -- Add MP3\nd -- Delete MP3\n+ -- Shift title up\n- -- Shift title down\ns -- Search for MP3 in playlist\nv -- View playlist\nw -- Write playlist to file\nx -- Exit\n\n");
					break;
				case '?':
				default:
					fprintf(stderr, "Error: Unknown command\n");
					break;
			}
		free(option);
		option = NULL;
		}
	}

	if (error == 1) {
		return 1;
	}

	return 0;
}

/*
 * Search for matching artist or title in sll
 * It only finds artist or title that starts with search_word or exactly matches
 *
 */
void search_sll(m3u_file *m3u_file, char *search_word)
{
	m3u_data *head = NULL;
	int count = 0;
	int found_something = 0;
	size_t search_word_length = 0;

	if (m3u_file == NULL || m3u_file->head == NULL || search_word == NULL) {
		return;
	}

	search_word_length = strlen(search_word);

	if (search_word_length < 1) {
		return;
	}

	for (head = m3u_file->head; head != NULL; head = head->next) {
		if (strncmp(head->artist, search_word, search_word_length) == 0 || strncmp(head->title, search_word, search_word_length) == 0) {
			printf("[ %d] 00:00 %s -- %s\n", count, head->artist, head->title);
			found_something = 1;
		}
		count++;
	}

	if (found_something == 0) {
		printf("Nothing found.\n");
	}
}

/*
 * deletes an element of sll by number
 *
 * track: the number of the element that should be removed
 *
 * returns: 0 on success or 1 on error
 */
int delete_mp3_file(m3u_file *m3u_file, int track)
{
	m3u_data *head = NULL;
	m3u_data *p = NULL;
	int count = 0;

	if (m3u_file == NULL || m3u_file->head == NULL || track < 0) {
		return 1;
	}

	for (head = m3u_file->head; head != NULL; head = head->next) {
		if (count == track) {
			if (p == NULL) {
				m3u_file->head = head->next;
				head->next = NULL;
				free_sll(&head);
			} else {
				p->next = head->next;
				head->next = NULL;
				free_sll(&head);
			}
			m3u_file->count--;
			return 0;
		}
		p = head;
		count++;
	}

	return 1;
}

/*
 * move an element up or down in a single linked list
 *
 * track: the number of the element that should be moved
 * option: '+' ... move up, '-' ... move down
 *
 * returns: 0 on success or 1 on error
 */
int shift_mp3_file(m3u_file *m3u_file, int track, char option)
{
	m3u_data *head = NULL;
	m3u_data *p = NULL;
	m3u_data *pp = NULL;
	m3u_data *n = NULL;
	int count = 0;

	if (m3u_file == NULL || m3u_file->head == NULL || track < 0) {
		return 1;
	}

	if (option != '+' && option != '-') {
		assert(1);
	}

	/* TODO: use a double linked list */
	for (head = m3u_file->head; head != NULL; head = head->next) {
		if (count == track) {
			if (option == '+') {
				if (p == NULL && pp == NULL) {
					printf("Track %d is already the first entry.\n", track);
				} else if (pp == NULL) {
					m3u_file->head = head;
					p->next = head->next;
					head->next = p;
				} else {
					pp->next = head;
					p->next = head->next;
					head->next = p;
				}
			} else {
				if (head->next != NULL && p != NULL) {
					n = head->next;
					head->next = n->next;
					p->next = n;
					n->next = head;
				} else if (head->next != NULL && p == NULL) {
					n = head->next;
					head->next = n->next;
					m3u_file->head = n;
					n->next = head;
				} else {
					printf("Track %d is already the last entry.\n", track);
				}
			}
			return 0;
		}
		pp = p;
		p = head;
		count++;
	}

	return 1;
}

/*
 * chechs if mp3 file starts with 0xFF 0xFB
 * returns: 1 if true or 0 when false
 */
int is_mp3_file_without_tags(char *filename)
{
	FILE *f = NULL;
	unsigned char buffer[2] = {0};
	int ret = 0;

	f = fopen(filename, "r");

	if (f == NULL) {
		perror("Could not open mp3 file");
		return 0;
	}

	if (fread(buffer, 1, 2, f) == 2) {
		if (buffer[0] == 0xFF && buffer[1] == 0xFB) {
			ret = 1;
		}
	} else {
		fprintf(stderr, "Could not read mp3 file.\n");
	}

	if (fclose(f) != 0) {
		perror("fclose");
	}

	return ret;
}
