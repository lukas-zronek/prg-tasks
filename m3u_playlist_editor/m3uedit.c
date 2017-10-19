/**
 * @file m3uedit.c
 * @author Lukas Zronek
 * @date 26.01.2017
 * @brief M3U Playlist Editor
 * @version 1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <util.h>

int main(int argc, char *argv[])
{
	int opt = 0;
	int eflag = 0, cflag = 0, aflag = 0, invalidflag = 0;
	char *m3u_filename = NULL;
	m3u_file *m3u_file = NULL;
	int error = 0;

	while (1) {
		opt = getopt(argc, argv, "e:c:a:h");

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 'e':
				eflag = 1;
				m3u_filename = optarg;
				break;
			case 'c':
				cflag = 1;
				m3u_filename = optarg;
				break;
			case 'a':
				aflag = 1;
				m3u_filename = optarg;
				break;
			default:
				invalidflag = 1;
				break;
		}
	}

	if (eflag + cflag + aflag + invalidflag > 1 || m3u_filename == NULL || (eflag == 1 && optind != argc) || ((cflag == 1 || aflag == 1) && optind == argc)) {
		fprintf(stderr, "Usage: %s [-e <FILENAME> | -c <FILENAME> | -a <FILENAME> | -h]\n", argv[0]);
		return EXIT_FAILURE;
	}

	printf("%s playlist \'%s\'\n", cflag == 1 ? "Creating" : "Loading" , m3u_filename);

	m3u_file = create_m3u_file(m3u_filename);

	if (m3u_file == NULL) {
		return EXIT_FAILURE;
	}

	if (cflag != 1) {
		if (parse_m3u_file(m3u_file) != 0) {
			FREE(m3u_file);
			return EXIT_FAILURE;
		} else {
			print_sll(m3u_file->head);
		}
	}

	if (eflag == 1) {
		if (interactive(m3u_file) != 0) {
			error = 1;
		}
	} else {
		if (add_mp3_files(m3u_file, &argv[optind], argc - optind) != 0) {
			error = 1;	
		} else {
			if (write_m3u_file(m3u_file) != 0) {
				error = 1;
			} else {
				printf("Playlist written to '%s'.\n", m3u_filename);
			}
		}
	}

	free_sll(&m3u_file->head);
	m3u_file->tail = NULL;
	FREE(m3u_file->filename);
	FREE(m3u_file);

	if (error == 1) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
