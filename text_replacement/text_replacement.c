/**
 * @file text_replacement.c
 * @author Lukas Zronek
 * @date 28.10.2016
 * @brief: allows to replace arbitrary, given strings within a text-file
 * @version: 1.1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef enum { false, true, empty } bool;

#define OUTPUT_TMP_FILENAME "output.txt"
#define BACKUP_TMP_FILENAME "backup.txt"

#define OLD_STRING_MAX_LENGTH 256

bool cmp_char(char, char, bool);

int main(int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	int opt = 0;
	char *old_string = NULL;
	char *new_string = NULL;
	char *input_filename = NULL;
	bool case_sensitive_flag = empty;
	bool unknown_opt_flag = false;
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	char input_char = 0;
	int matched_chars_count = 0;
	char *saved_input = NULL;
	int old_string_length = 0;

	while (1) {
		opt = getopt(argc, argv, "o:n:f:ish");

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 'o':
				old_string = optarg;
				break;
			case 'n':
				new_string = optarg;
				break;
			case 'f':
				input_filename = optarg;
				break;
			case 'i':
				case_sensitive_flag = false;
				break;
			case 's':
				case_sensitive_flag = true;
				break;
			default:
				unknown_opt_flag = true;
				break;
		}
	}

	if (unknown_opt_flag == true || old_string == NULL || new_string == NULL || input_filename == NULL || case_sensitive_flag == empty || optind < argc) {
		fprintf(stderr, "Usage: %s -o old_string -n new_string -f file [-i | -s]\n", argv[0]);
		return EXIT_FAILURE;
	}

	old_string_length = strnlen(old_string, OLD_STRING_MAX_LENGTH);

	if (old_string_length == OLD_STRING_MAX_LENGTH) {
		if (old_string[old_string_length] != '\0') {
			fprintf(stderr, "Warning: old string truncated! (max %d chars)\n", OLD_STRING_MAX_LENGTH);
		}
	}

	if (old_string_length < 1) {
		/* nothing to do */
		return EXIT_SUCCESS;
	}

	input_file = fopen(input_filename, "r");

	if (input_file == NULL) {
		perror("Error: opening the input file failed");
		return EXIT_FAILURE;
	}

	output_file = fopen(OUTPUT_TMP_FILENAME, "w+");

	if (output_file == NULL) {
		perror("Error: opening the temporary output file failed");
		ret = EXIT_FAILURE;
		goto cleanup_input;
	}

	saved_input = (char *)malloc(old_string_length);

	if (saved_input == NULL) {
		fprintf(stderr, "Error: malloc failed\n");
		ret = EXIT_FAILURE;
		goto cleanup_files;
	}

	while (1) {
		input_char = fgetc(input_file);

		if (input_char == EOF) {
			break;
		}

		if (cmp_char(input_char, old_string[matched_chars_count], case_sensitive_flag)) {
			if (matched_chars_count < (old_string_length - 1)) {
				saved_input[matched_chars_count] = input_char;
				saved_input[matched_chars_count + 1] = '\0';
				matched_chars_count++;
			} else {
				fprintf(output_file, "%s", new_string);
				matched_chars_count = 0;
			}
		} else {
			if (matched_chars_count > 0) {
				if (fprintf(output_file, "%s", saved_input) < 0) {
					fprintf(stderr, "Error: Could not write to output file.\n");
					ret = EXIT_FAILURE;
					break;
				}
				matched_chars_count = 0;
			}
			fputc(input_char, output_file);
		}
	}

	free(saved_input);
	saved_input = NULL;

cleanup_files:
	if (output_file != NULL && fclose(output_file) != 0) {
		perror("Error: closing output file failed");
		ret = EXIT_FAILURE;
	}

cleanup_input:
	if (input_file != NULL && fclose(input_file) != 0) {
		perror("Error: closing input file failed");
		ret = EXIT_FAILURE;
	}

	if (ret == EXIT_SUCCESS) {
		if (rename(input_filename, BACKUP_TMP_FILENAME) != 0) {
			perror("Error: renaming the input file failed");
			return EXIT_FAILURE;
		}

		if (rename(OUTPUT_TMP_FILENAME, input_filename) != 0) {
			perror("Error: renaming the temporary output file failed");
			return EXIT_FAILURE;
		}

		if (remove(BACKUP_TMP_FILENAME) != 0) {
			perror("Error: removing the backup input file failed");
			return EXIT_FAILURE;
		}
	}

	return ret;
}

bool cmp_char(char c1, char c2, bool case_sensitive_flag)
{
	if (case_sensitive_flag == false) {
		c1 = toupper(c1);
		c2 = toupper(c2);
	}

	return c1 == c2;
}
