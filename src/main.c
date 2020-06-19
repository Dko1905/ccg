#include <stdio.h>
#include <stdint.h> // For uintmax_t
#include <inttypes.h> // For strtoumax
#include <errno.h> // For errno

typedef uintmax_t uintm;

#define BUFFERSIZE

void generate_to_stream(FILE* stream, uintm max_number);

// Macro for checking and printing error info
#define ERRBUFFERSIZE 255
#define ERR_CHECK(exp, message, filename) \
	if(exp){ \
		char str[ERRBUFFERSIZE]; \
		snprintf(str, ERRBUFFERSIZE, message, filename); \
		perror(str); \
		return errno; \
	}

int main(int argc, char* argv[]){
	if(argc < 3){ // Check for right amount of arguments
		fprintf(stderr, "Incorrect usage!\n");
		printf("Usage: ccg <filename> <max number>\n");
		return 1;
	}
	char* endptr; // From cppreference
	char* filename = argv[1]; // Unsafe
	uintm maxNumber = strtoumax(argv[2], &endptr, 10);
	if(maxNumber < 1){
		fprintf(stderr, "Incorrect/invalid number!\n");
		printf("Usage: ccg <filename> <max number>\n");
		return 1;
	}

	FILE* fptr = fopen(filename, "w");
	ERR_CHECK(fptr == NULL, "Failed to open %s", filename);

	generate_to_stream(fptr, maxNumber);
	ERR_CHECK(fclose(fptr) != 0, "Failed to close %s", filename);
}

void generate_to_stream(FILE* stream, uintm max_number){
	uintm steps;
	uintm n_temp;
	for(uintm n = 1; n < max_number; n++){
		n_temp = n; // by value
		steps = 0;
		while(n_temp != 1){
			if( (n_temp & 1) == 0){// If even
				n_temp >>= 1;
			}
			else{
				n_temp = n_temp * 3 + 1;
			}
			steps++;
		}
		fprintf(stream, "%llu,%llu\n", (unsigned long long)n, (unsigned long long)steps);
	}
}