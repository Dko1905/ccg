#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_BUFFER_SIZE 0xff
#ifndef SHOW_PROGRESS
#define SHOW_PROGRESS 1
#include <pthread.h>
#endif

void gen_binary(size_t min, size_t max, size_t bufsiz, FILE *output, float *progress);
void binary_to_csv(FILE* in, FILE* out, size_t bufsiz, float *progress);

void *progress_func(void *args);

const size_t max = 1000;

int main(int argc, char *argv[]) {
	/* Options:
	 * argv[1] = mode
	 */
	float progress = 0.f;
	int status = EXIT_SUCCESS;
	#if SHOW_PROGRESS == 1
	pthread_t p_thread;
	#endif

	if (strcmp(argv[1], "g") == 0) {
		FILE *out;
		if (strcmp("-", argv[2]) == 0) {
			out = stdout;
		} else {
			out = fopen(argv[2], "w");
		}
		if (out != NULL) {
			size_t min, max;
			if (sscanf(argv[3], "%zu", &min) == 1 && sscanf(argv[4], "%zu", &max) == 1) {
				size_t bufsiz = DEFAULT_BUFFER_SIZE;
				if (argc == 6) {
					if (sscanf(argv[5], "%zu", &bufsiz) != 1) {
						fprintf(stderr, "%s: Failed to parse bufsiz: %s", argv[0], strerror(errno));
						return EXIT_FAILURE;
					}
				}
				#if SHOW_PROGRESS == 1
				if (pthread_create(&p_thread, NULL, progress_func, &progress) != 0) {
					fprintf(stderr, "%s: Failed to create pthread: %s", argv[0], strerror(errno));
					status = EXIT_FAILURE;
				}
				#endif

				if (status == EXIT_SUCCESS) {
					gen_binary(min, max, bufsiz, out, &progress);
					progress = 2.f;
					pthread_join(p_thread, NULL);
				}
			} else {
				fprintf(stderr, "%s: Failed to parse min or max: %s\n", argv[0], strerror(errno));
				status = EXIT_FAILURE;
			}

			fclose(out);
		} else {
			fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
			status = EXIT_FAILURE;
		}
	} else if (strcmp(argv[1], "c") == 0) {
		FILE *in;
		if (strcmp("-", argv[2]) == 0) {
			in = stdin;
		} else {
			in = fopen(argv[2], "r");
		}
		if (in != NULL) {
			FILE *out;
			if (strcmp("-", argv[3]) == 0) {
				out = stdout;
			} else {
				out = fopen(argv[3], "w");
			}
			if (out != NULL) {
				size_t bufsiz = DEFAULT_BUFFER_SIZE;
				if (argc > 4 && sscanf(argv[4], "%zu", &bufsiz) != 1) {
					fprintf(stderr, "%s: Failed to parse min: %s", argv[0], strerror(errno));
					status = EXIT_FAILURE;
				}

				#if SHOW_PROGRESS == 1
				if (pthread_create(&p_thread, NULL, progress_func, &progress) != 0) {
					fprintf(stderr, "%s: Failed to create pthread: %s", argv[0], strerror(errno));
					status = EXIT_FAILURE;
				}
				#endif

				if (status == EXIT_SUCCESS) {
					binary_to_csv(in, out, bufsiz, &progress);
					progress = 2.f;
					pthread_join(p_thread, NULL);
				}

				fclose(out);
			} else {
				fprintf(stderr, "%s: %s: %s", argv[0], argv[3], strerror(errno));
			}

			fclose(in);
		} else {
			fprintf(stderr, "%s: %s: %s", argv[0], argv[1], strerror(errno));
		}
	} else {
		fprintf(stderr, "%s: Invalid usage", argv[0]);
	}

	return status;
}

void *progress_func(void *args) {
	float *progress = args;

	while (*progress != 2.f) {
		printf("%.2f %%\n", *progress * 100);
		struct timespec a = {
			.tv_sec = 0,
			.tv_nsec = 100000000
		};
		nanosleep(&a, NULL);
	}

	return NULL;
}

void binary_to_csv(FILE* in, FILE* out, size_t bufsiz, float *progress) {
	size_t min = 0, max = 0;
	size_t *buffer = NULL;

	buffer = calloc(bufsiz, sizeof(size_t));
	if (buffer == NULL) {
		perror("Failed to allocate buffer");
		return;
	}

	/* Read header. */
	fread(&min, sizeof(size_t), 1, in);
	fread(&max, sizeof(size_t), 1, in);

	while (min < max) {
		/* Update progress. */
		*progress = (float)min / max;

		if (max - min >= bufsiz) { /* What's left. */
			fread(buffer, sizeof(size_t), bufsiz, in);
			for (size_t i = 0; i < bufsiz; ++i) {
				fprintf(out, "%zu,%zu\n", min + i, buffer[i]);
			}
			min += bufsiz;
		} else {
			fread(buffer, sizeof(size_t), max - min, in); /* Read the rest. */
			for (size_t i = 0; i < max - min; ++i) {
				fprintf(out, "%zu,%zu\n", min + i, buffer[i]);
			}
			min += max - min;
		}
	}
	/* Final progress. */
	*progress = 1.f;

	free(buffer);
}

void gen_binary(size_t min, size_t max, size_t bufsiz, FILE *output, float *progress) {
	size_t n = 0; /* The current index. */
	size_t s = 0; /* Steps. */
	size_t *buffer = NULL;
	size_t buffree = 0; /* Free index in buffer. */

	buffer = calloc(bufsiz, sizeof(size_t));
	if (buffer == NULL) {
		perror("Failed to allocate buffer in gen_binary");
		return;
	}

	/* Write header in binary file. */
	fwrite(&min, sizeof(size_t), 1, output);
	fwrite(&max, sizeof(size_t), 1, output);

	/* Go through min to max, excluding max. */
	while ((n = min++) < max) {
		s = 0;

		/* Logic. */
		while (n > 1) {
			if ((n & 1) == 0) { /* even */
				n >>= 1; /* Divide by 2. */
				++s;
			} else { /* odd */
				n = (n * 3 + 1) >> 1; /* (3 * n + 1) / 2 */
				s += 2;
			}
		}

		/* Save logic. */
		buffer[buffree++] = s;
		if (buffree >= bufsiz) {
			fwrite(buffer, sizeof(size_t), buffree, output);
			buffree = 0;
		}

		/* Update progress. */
		*progress = (float)min / max;
	}
	/* Print if anything is left. */
	if (buffree > 0) {
		fwrite(buffer, sizeof(size_t), buffree, output);
		buffree = 0;
	}
	/* Final progress. */
	*progress = 1.f;

	free(buffer);
}
